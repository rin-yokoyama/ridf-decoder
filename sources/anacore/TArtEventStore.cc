/*
 *    TArtEventStore.cc
 *    Created       : Jan 19 2007 02:48:33
 *    Last Modified : 2009/01/19 13:30:56 JST
 *--------------------------------------------------------
 *    Comment :
 *
 *--------------------------------------------------------
 *    Copyright (C)2007 by ota
 */
#include "TArtStoreManager.hh"
#include "TArtEventStore.hh"
#include "TArtEventInfo.hh"
#include "TArtFileDataSource.hh"
#include "TArtKafkaDataSource.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#include "TArtParserFactory.hh"
// TArtEventStore* TArtEventStore::fgEventStore = 0;

//////////////////////////////////////////////////
//

TArtEventStore::TArtEventStore()
    : fDataSource(0), fParser(0), pInterrupt(NULL)
{
  std::cout << "TArtEventStore: Constructor" << std::endl;
  sman = TArtStoreManager::Instance();

  rawevent_ = new TArtRawEventObject();
  sman->SetRawEventPtr(rawevent_);

  runinfo_ = new TArtRunInfo();
  sman->SetRunInfoPtr(runinfo_);

  eventinfo_ = new TArtEventInfo();
  sman->SetEventInfoPtr(eventinfo_);
}

TArtEventStore::~TArtEventStore()
{
  if (rawevent_)
  {
    delete rawevent_;
    rawevent_ = NULL;
  }

  if (runinfo_)
  {
    delete runinfo_;
    runinfo_ = NULL;
  }
  if (eventinfo_)
  {
    delete eventinfo_;
    eventinfo_ = NULL;
  }
  if (fDataSource)
  {
    delete fDataSource;
    fDataSource = NULL;
  }
  if (fParser)
  {
    delete fParser;
    fParser = NULL;
  }
}

void TArtEventStore::ClearData()
{
  TArtEventInfo *ei = eventinfo_;
  if (ei)
    ei->Clear();
  if (rawevent_)
    rawevent_->Clear();
}

bool TArtEventStore::Open()
{
  return Open(0);
}

bool TArtEventStore::Open(const char *filename)
{
  fDataSource = new TArtFileDataSource();
  std::cout << "TArtEventStore: File Open: " << filename << std::endl;
  bool fopened = fDataSource->FileOpen(filename);
  if (!fopened)
  {
    std::cerr << "TArtEventStore: Failed to open the file : " << filename << std::endl;
    return false;
  }

  fParser = TArtParserFactory::Create(fDataSource);
  return true;
}

bool TArtEventStore::Open(const std::string &bootstrap_servers, const std::string &topic_name, const std::string &group_id, const std::string &client_id, const u_int64_t &timestamp)
{
  fDataSource = new TArtKafkaDataSource(bootstrap_servers, topic_name, group_id, client_id, timestamp);
  fParser = TArtParserFactory::Create(fDataSource);
  return true;
}

int TArtEventStore::SkipEvent(int neve)
{
  int nskip = 0;
  std::cout << "Skip " << neve << " events" << std::endl;
  fParser->SetSkip(true);
  for (int i = 0; i < neve; i++)
  {
    if (GetNextEvent())
      nskip++;
  }
  fParser->SetSkip(false);
  return nskip;
}

bool TArtEventStore::GetNextEvent()
{

  if (!rawevent_)
  {
    std::cout << "TArtEventStore: TArtRawEventObject is not defined" << std::endl;
    return false;
  }
  if (fDataSource->GetFileStatus() == kNOTEXIST)
    return false;

  // run info will not be cleared.
  TArtEventInfo *ei = eventinfo_;
  ei->Clear();
  rawevent_->Clear();

  while (rawevent_->GetNumSeg() == 0)
  { // pooling till segment > 0
    bool getdata = fParser->Parse(fDataSource, rawevent_);
    // return false, when data can not read out.
    if (!getdata && fDataSource->GetDataSourceType() != kSM)
    {
      std::cout << "TArtEventStore: return false, when data can not read out." << std::endl;
      return false;
    }

    if (pInterrupt)
    {
      if (true == *pInterrupt)
      {
        std::cout << "TArtEventStore: Interrupted... Stop to get data." << std::endl;
        return false;
      }
    }
    if (fDataSource->GetDataSourceType() == kSM)
    {
      std::cout << "TArtEventStore: waiting for a new Kafka message..." << std::endl;
      sleep(1);
    }
  }

  ei->SetRunNumber(rawevent_->GetRunNumber());
  ei->SetEventNumber(rawevent_->GetEventNumber());
  ei->SetTimeStamp(rawevent_->GetTimeStamp());

  return true;
}
