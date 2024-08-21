#include "TArtKafkaDataSource.hh"

TArtKafkaDataSource::TArtKafkaDataSource(const std::string &bootstrap_servers, const std::string &topic_name, const std::string &group_id, const std::string &client_id, const u_int64_t &timestamp)
    : TArtDataSource(), topic_name_(topic_name)
{
    // Initialize a Kafka consumer
    char errstr[512];
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    rd_kafka_conf_set(conf, "client.id", client_id.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "group.id", group_id.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "bootstrap.servers", bootstrap_servers.c_str(), errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "auto.offset.reset", "latest", errstr, sizeof(errstr));
    // rd_kafka_topic_conf_t *tconf = rd_kafka_topic_conf_new();
    // rd_kafka_topic_conf_set(tconf, "auto.offset.reset", "latest", errstr, sizeof(errstr));
    // rd_kafka_conf_set_default_topic_conf(conf, tconf);
    rk_ = std::unique_ptr<rd_kafka_t, RdKafkaDeleter>(rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr)));
    if (!SetOffset(timestamp))
    {
        std::cerr << "Failed to set offset." << std::endl;
        rk_.reset();
    }
    rd_kafka_poll_set_consumer(rk_.get());
    rd_kafka_topic_partition_list_t *topics = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(topics, topic_name_.c_str(), RD_KAFKA_PARTITION_UA);

    auto err = rd_kafka_subscribe(rk_.get(), topics);
    if (err)
        std::cout << rd_kafka_err2str(err) << std::endl;
    rd_kafka_topic_partition_list_destroy(topics);
    fStatus = 0;
    fFileStatus = kEXIST;
    fDataSourceType = kSM;
}

TArtKafkaDataSource::~TArtKafkaDataSource()
{
}

int TArtKafkaDataSource::Seek(long offset, int origin)
{
    return 0;
}

int TArtKafkaDataSource::Read(char *buf, const int &size, const int &offset)
{
    int len = 0;
    if (fStatus == 0)
        std::cout << "TArtKafkaDataSource: waiting for the first event... " << std::endl;
    rkmessage_ = std::unique_ptr<rd_kafka_message_t, RdKafkaMessageDeleter>(rd_kafka_consumer_poll(rk_.get(), 1000));
    if (rkmessage_.get())
    {
        if (rkmessage_->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
        {
            len = 0; // The partition is empty
        }
        else if (rkmessage_->err)
        {
            std::cout << rd_kafka_err2str(rkmessage_->err) << std::endl;
            len = 0;
        }
        else
        {
            len = rkmessage_->len;
            if (len > 0)
            {
                std::memcpy(buf, rkmessage_->payload, len);
                fStatus = 1;
            }
        }
    }
    else
    {
        std::cout << "TArtKafkaDataSource: waiting for the Kafka stream... " << std::endl;
    }
    return len;
}

void TArtKafkaDataSource::Close()
{
}

bool TArtKafkaDataSource::SetOffset(const u_int64_t &timestamp)
{
    if (!timestamp)
        return true; // "latest" if timestamp = 0

    // Create metadata structure to get partition information
    const rd_kafka_metadata_t *metadata;
    if (rd_kafka_metadata(rk_.get(), 0, nullptr, &metadata, 5000) != RD_KAFKA_RESP_ERR_NO_ERROR)
    {
        std::cerr << "Failed to fetch metadata" << std::endl;
        return false;
    }

    // Find the metadata for the specific topic
    const rd_kafka_metadata_topic_t *topic_metadata = nullptr;
    for (int i = 0; i < metadata->topic_cnt; ++i)
    {
        if (strcmp(metadata->topics[i].topic, topic_name_.c_str()) == 0)
        {
            topic_metadata = &metadata->topics[i];
            break;
        }
    }

    if (!topic_metadata)
    {
        std::cerr << "Topic not found" << std::endl;
        rd_kafka_metadata_destroy(metadata);
        return false;
    }

    // Create a topic partition list with the desired timestamp for each partition
    rd_kafka_topic_partition_list_t *tpl = rd_kafka_topic_partition_list_new(topic_metadata->partition_cnt);
    for (int i = 0; i < topic_metadata->partition_cnt; ++i)
    {
        rd_kafka_topic_partition_t *tp = rd_kafka_topic_partition_list_add(tpl, topic_name_.c_str(), topic_metadata->partitions[i].id);
        tp->offset = timestamp;
    }

    // Query offsets by timestamp for all partitions
    if (rd_kafka_offsets_for_times(rk_.get(), tpl, 10000) != RD_KAFKA_RESP_ERR_NO_ERROR)
    {
        std::cerr << "Failed to fetch offsets for timestamp" << std::endl;
        rd_kafka_topic_partition_list_destroy(tpl);
        rd_kafka_metadata_destroy(metadata);
        return false;
    }

    // Check for invalid offsets and set them for the partitions
    for (int i = 0; i < tpl->cnt; ++i)
    {
        if (tpl->elems[i].offset == RD_KAFKA_OFFSET_INVALID)
        {
            std::cerr << "No offset available for the given timestamp for partition " << tpl->elems[i].partition << std::endl;
            rd_kafka_topic_partition_list_destroy(tpl);
            rd_kafka_metadata_destroy(metadata);
            return false;
        }
    }

    // Assign the obtained offsets to the consumer
    if (rd_kafka_assign(rk_.get(), tpl) != RD_KAFKA_RESP_ERR_NO_ERROR)
    {
        std::cerr << "Failed to assign offsets" << std::endl;
        rd_kafka_topic_partition_list_destroy(tpl);
        rd_kafka_metadata_destroy(metadata);
        return false;
    }

    rd_kafka_topic_partition_list_destroy(tpl);
    rd_kafka_metadata_destroy(metadata);
    fStatus = 1;
    return true;
}