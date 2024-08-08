#include <iostream>
#include <unistd.h>
#include <fstream>
#include <memory>
#include <librdkafka/rdkafka.h>
#include <TArtEventStore.hh>
#include "mira_streaming.hpp"
#include "ParquetWriter.hpp"

// word size for RIDF (=4)
static const size_t kWordSize = 4;

// Global variables for Kafka (assuming they are defined somewhere else)
bool kafka_ready = false; // Placeholder

/** prints usage **/
void usage(char *argv0)
{
    std::cout << "[ridfstream_decoder]: Usage: \n"
              << argv0 << " -i [input_topic_name] (default: ridf)\n"
              << "-o [output_topic_name] (default: decoded)"
              << "-s [bootstrap_servers] "
              << "-g [group_id] (default: ridfstream_decoder)"
              << "-t [unix_timestamp_offset] (default: latest)"
              << "-b [output_record_batch_length] (default: 1)"
              << std::endl;
}

int main(int argc, char **argv)
{

    std::string input_topic_name = "ridf";       // Kafka topic name for the consumer
    std::string output_topic_name = "decoded";   // Kafka topic name for the producer
    std::string group_id = "ridfstream_decoder"; // Kafka group.id for the consumer
    std::string bootstrap_servers = "default";   // Kafka bootstrap.servers
    u_int64_t timestamp_offset = 0;              // Kafka offset (0=latest)
    u_int32_t batch_length = 1;                  // Number of events in an output record batch

    int opt = 0;
    while ((opt = getopt(argc, argv, "i:o:n:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            input_topic_name = optarg;
            break;
        case 'o':
            output_topic_name = optarg;
            break;
        case 'g':
            group_id = optarg;
            break;
        case 's':
            bootstrap_servers = optarg;
            break;
        case 't':
            timestamp_offset = std::stoull(optarg);
            break;
        case 'b':
            batch_length = std::stoul(optarg);
            break;
        default:
            usage(argv[0]);
            return 1;
            break;
        }
    }
    if (bootstrap_servers == "default")
    {
        std::cerr << "[ridfstream_decoder]: No bootstrap.servers specified\nexitting..." << std::endl;
        return 1;
    }

    std::string client_id = std::to_string(getpid());

    // initialize a Kafka producer
    auto topic = mira::init_producer(client_id, bootstrap_servers, output_topic_name);

    // main loop
    TArtEventStore *estore = new TArtEventStore();
    estore->Open(bootstrap_servers, input_topic_name, group_id, client_id, timestamp_offset);
    TArtRawEventObject *rawevent = estore->GetRawEventObject();

    ParquetWriter writer;
    u_int64_t n_event = 0;
    while (estore->GetNextEvent())
    {
        writer.Fill(rawevent, n_event);

        estore->ClearData();
        ++n_event;

        if (!(n_event % batch_length))
        {
            auto record_batch = writer.GenerateRecordBatch();
            auto stream = writer.WriteStream(record_batch);
            mira::produce(topic, stream->size(), (void *)stream->data());
        }
    }
    rd_kafka_flush(mira::rk_producer, 1000 * 10);
    rd_kafka_topic_destroy(topic);
    rd_kafka_destroy(mira::rk_producer);
    return 0;
}