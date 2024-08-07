/*
 *    ARTEventStore.hh
 *    Created       : Jan 19 2007 01:09:13
 *    Last Modified : 2008/10/30 22:05:58 JST
 *--------------------------------------------------------
 *    Comment : EventStore is a singleton for anaroot and anapaw
 *
 *--------------------------------------------------------
 *    Copyright (C)2007-2008 by ota
 */
#ifndef TARTEVENTSTORE_H
#define TARTEVENTSTORE_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fstream>
#include "TArtParser.hh"
#include "TArtDataSource.hh"
#include "TArtEventInfo.hh"
#include "TArtRunInfo.hh"
#include "TArtRawEventObject.hh"
#include "TArtStoreManager.hh"

#define MAXFILENAME 1024
#define MB 1048576          // Unit of MByte (1024*1024)
#define MAXBLOCKSIZE 131072 // maximum block size 128kByte
#define WORDSIZE 2          // Byte

/** TArtEventStore
 *
 *
 */
class TArtEventStore
{
public:
  virtual ~TArtEventStore();
  virtual bool Open();
  // virtual bool Open(const int sid);
  virtual bool Open(const char *filename);
  // virtual bool Open(const char *hostname, const int port);
  void ClearData();

  // pointer to interupt event loop.
  void SetInterrupt(bool *p) { pInterrupt = p; }
  bool GetNextEvent();
  //  void SkipEvent(int neve); // function to skip events
  int SkipEvent(int neve); // function to skip events
  TArtRawEventObject *GetRawEventObject() { return rawevent_; }
  TArtEventInfo *GetEventInfoObject() { return eventinfo_; }
  TArtRunInfo *GetRunInfo() { return runinfo_; }
  void SetObjectPointer(TArtRawEventObject *raweve) { rawevent_ = raweve; }

private:
  TArtDataSource *fDataSource;
  TArtParser *fParser;
  TArtEventInfo *eventinfo_;
  TArtRunInfo *runinfo_;
  TArtRawEventObject *rawevent_;
  bool *pInterrupt;
  char *fMapFileName;
  TArtStoreManager *sman;

public:
  TArtEventStore();
};
#endif // end of #ifdef TARTEVENTSTORE_H
