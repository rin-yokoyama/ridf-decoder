#ifndef _TARTRUNINFO_H_
#define _TARTRUNINFO_H_

#include <string>

class TArtRunInfo
{

public:
  TArtRunInfo() { Clear(); }
  virtual ~TArtRunInfo() {}
  virtual void Clear()
  {
    runname.clear();
    runnumber.clear();
    starttime.clear();
    stoptime.clear();
    date.clear();
    rev.clear();
    header.clear();
    ender.clear();
  }

  virtual void SetRunName(char *val) { runname = val; }
  virtual void SetRunNumber(char *val) { runnumber = val; }
  virtual void SetStartTime(char *val) { starttime = val; }
  virtual void SetStopTime(char *val) { stoptime = val; }
  virtual void SetDate(char *val) { date = val; }
  virtual void SetHeader(char *val) { header = val; }
  virtual void SetEnder(char *val) { ender = val; }

  virtual std::string GetRunName() { return runname; }
  virtual std::string GetRunNumber() { return runnumber; }
  virtual std::string GetStartTime() { return starttime; }
  virtual std::string GetStopTime() { return stoptime; }
  virtual std::string GetDate() { return date; }
  virtual std::string GetHeader() { return header; }
  virtual std::string GetEnder() { return ender; }

private:
  std::string runname;
  std::string runnumber;
  std::string starttime;
  std::string stoptime;
  std::string date;
  std::string rev;
  std::string header;
  std::string ender;
};

#endif
