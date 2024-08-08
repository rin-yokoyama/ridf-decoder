#ifndef TARTKAFKADATASOURCE_H
#define TARTKAFKADATASOURCE_H
#include "TArtDataSource.hh"
#include <iostream>
#include <cstring>
#include <memory>
#include <sys/stat.h>
#include <librdkafka/rdkafka.h>
using namespace std;

struct RdKafkaDeleter
{
    void operator()(rd_kafka_t *rk) const
    {
        if (rk)
        {
            rd_kafka_destroy(rk);
        }
    }
};

struct RdKafkaMessageDeleter
{
    void operator()(rd_kafka_message_t *msg) const
    {
        if (msg)
        {
            rd_kafka_message_destroy(msg);
        }
    }
};

class TArtKafkaDataSource : public TArtDataSource
{
public:
    TArtKafkaDataSource(const std::string &bootstrap_servers, const std::string &topic_name, const std::string &group_id, const std::string &client_id, const u_int64_t &timestamp);
    ~TArtKafkaDataSource();
    int GetRunNumber() { return fRunNumber; }
    void SetRunNumber(int n) { fRunNumber = n; }
    int Read(char *buf, const int &size, const int &offset = 0);
    void Close();
    virtual int Seek(long offset, int origin);
    bool SetOffset(const u_int64_t &timestamp);

private:
    int fRunNumber;
    int fStatus;
    const std::string topic_name_;
    std::unique_ptr<rd_kafka_t, RdKafkaDeleter> rk_;
    std::unique_ptr<rd_kafka_message_t, RdKafkaMessageDeleter> rkmessage_;
};
#endif // end of #ifdef TArtKafkaDataSource_H
