/* $Id: TArtFileDataSource.cc 4 2009-09-02 15:59:29Z sweetbluemoon $ */
/**
 * @file   TArtFileDataSource.cc
 * @date   Created : Mar 29, 2008 16:29:43 JST
 *   Last Modified : 2009/01/19 13:31:26 JST
 * @author RIPS Taro <rips@entrance.ripsnet>
 *
 *
 *    Copyright (C)2008
 */

#include "TArtFileDataSource.hh"
#include "TArtStoreManager.hh"
#include "TArtRunInfo.hh"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Size */
#define RIDF_COMMENT_RUNINFO_ASC_SIZE 1024

/** RIDF comment header structure */
struct ridf_comment_runinfost
{
  char runname[100];
  char runnumber[100];
  char starttime[20];
  char stoptime[20];
  char date[20];
  char rev[40];
  char header[100];
  char ender[100];
};

TArtFileDataSource::TArtFileDataSource()
    : fFile(0)
{
}
TArtFileDataSource::~TArtFileDataSource()
{
}

bool TArtFileDataSource::FileOpen(const char *sourceName)
{
  if (!sourceName)
  {
    fFileStatus = kNOTEXIST;
    return false;
  }
  if (stat(sourceName, &fFileStat))
  {
    fFileStatus = kNOTEXIST;
    return false;
  }

  if (fFile)
    gzclose(fFile);
  fFile = gzopen(sourceName, "rb");
  if (!fFile)
  {
    printf("<TArtFileDataSource::Open> unexpected error at line %d\n",
           __LINE__);
    return false;
  }
  std::cout << "TArtFileDataSource: File = " << sourceName << std::endl;
  std::cout << "TArtFileDataSource: Size = " << fFileStat.st_size / 1024. / 1024. << " (MB)" << std::endl;
  //  long pos = ftell(fFile);
  // not ftell, use ftello for large file
  //  long pos = ftello(fFile);
  // use gztell also for gz file
  long pos = gztell(fFile);
  unsigned int header[2];
  struct ridf_comment_runinfost info;
  char start[20], stop[20];

  fRunNumber = 0;
  gzread(fFile, (char *)header, sizeof(int) * 2);
  if (header[1] != 0)
  {
    // RIDF format
    std::cout << "TArtFileDataSource: Format = RIDF" << std::endl;

    gzread(fFile, (char *)header, sizeof(int) * 2);

    if (((header[0] >> 22) & 0x3f) == 5)
    {
      gzread(fFile, (char *)header, sizeof(int) * 2);

      if (header[1] == 1)
      {
        gzread(fFile, (char *)&info, 1 * sizeof(info));
        memset(start, 0, sizeof(start));
        memset(stop, 0, sizeof(stop));

        memcpy(start, info.starttime + 9, sizeof(start) - 9);
        memcpy(stop, info.stoptime + 8, sizeof(stop) - 8);

        std::cout << "TArtFileDataSource: Run name   = " << info.runname << std::endl;
        std::cout << "TArtFileDataSource: Run number = " << info.runnumber << std::endl;
        std::cout << "TArtFileDataSource: Start time = " << start << std::endl;
        std::cout << "TArtFileDataSource: Stop time  = " << stop << std::endl;
        std::cout << "TArtFileDataSource: Date       = " << info.date << std::endl;
        std::cout << "TArtFileDataSource: Header     = " << info.header << std::endl;
        std::cout << "TArtFileDataSource: Ender      = " << info.ender << std::endl;

        fRunNumber = strtol(info.runnumber, NULL, 10);

        TArtStoreManager *sman = TArtStoreManager::Instance();
        TArtRunInfo *ri = sman->GetRunInfoPtr();

        ri->SetRunName(info.runname);
        ri->SetRunNumber(info.runnumber);
        ri->SetStartTime(start);
        ri->SetStopTime(stop);
        ri->SetDate(info.date);
        ri->SetHeader(info.header);
        ri->SetEnder(info.ender);
      }
    }
  }
  else
  {
    std::cout << "TArtFileDataSource: Format = RDF" << std::endl;
  }

  // not fseek, use fseeko for large file
  //  fseeko(fFile,pos,0);
  gzseek(fFile, pos, 0);
  fFileStatus = kEXIST;
  fDataSourceType = kFILE;
  return true;
}

void TArtFileDataSource::Close()
{
  if (fFile)
    gzclose(fFile); // fclose(fFile);
  fFile = 0;
  fFileStatus = kNOTEXIST;
}

int TArtFileDataSource::Seek(long offset, int origin)
{
  if (!fFile)
    return -1;
  // not fseek, use fseeko for large file
  //   return fseeko(fFile,offset,origin);
  return gzseek(fFile, offset, origin);
}

int TArtFileDataSource::Read(char *buf, const int &size, const int & /* offset */)
{
  if (!fFile)
    return 0;
  //   return fread(buf,sizeof(char),size,fFile;)
  return gzread(fFile, buf, sizeof(char) * size);
}
