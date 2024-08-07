#ifndef TARTSTOREMANAGER_H
#define TARTSTOREMANAGER_H

#include "TArtRawEventObject.hh"
#include "TArtRunInfo.hh"
#include "TArtEventInfo.hh"

class TArtStoreManager
{

protected:
  TArtRunInfo *runinfo_;
  TArtEventInfo *eventinfo_;
  TArtRawEventObject *rawevent_;

  TArtStoreManager();
  static TArtStoreManager *sStoreManager;

public:
  static TArtStoreManager *Instance();
  virtual ~TArtStoreManager();

  TArtRunInfo *GetRunInfoPtr() { return runinfo_; };
  void SetRunInfoPtr(TArtRunInfo *runinfo) { runinfo_ = runinfo; };
  TArtEventInfo *GetEventInfoPtr() { return eventinfo_; };
  void SetEventInfoPtr(TArtEventInfo *eventinfo) { eventinfo_ = eventinfo; };
  TArtRawEventObject *GetRawEventPtr() { return rawevent_; };
  void SetRawEventPtr(TArtRawEventObject *rawevent) { rawevent_ = rawevent; };
};

#endif // end of #ifdef TARTMAPPER_H
