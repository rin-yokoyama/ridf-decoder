/*
 * @file TArtParserRIDF.cc
 * @date  Created : 2008/10/20 13:20:43 JST<BR>
 *  Last Modified : Dec 04, 2008 22:42:46 JST
 *--------------------------------------------------------
 *    Comment :
 *
 *--------------------------------------------------------
 *    Copyright (C)2008 by Shinsuke OTA <ota@ribf.riken.jp>
 */
#include "TArtParserRIDF.hh"
#include "TArtDecoderFactory.hh"

#include <iostream>
#include <math.h>
#include <stdlib.h>

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

TArtParserRIDF::TArtParserRIDF()
{
}
TArtParserRIDF::~TArtParserRIDF()
{
}

void TArtParserRIDF::GetNextBlock(TArtDataSource *source)
{
  fOffset = 0;
  fNext = 0;
  /// Kafka source
  if (source->GetDataSourceType() == kSM)
  {
    /// read byte data from message
    fBlockSize = source->Read((char *)fBlockBuffer, 0);
    if (fBlockSize < sizeof(fHeader))
    {
      source->SetBlockStatus(kEOF);
      fHeader.BYTE = 0;
      return;
    }
    /// copy header bytes to fHeader
    std::memcpy(&fHeader, fBlockBuffer, sizeof(fHeader));
    fBlockSize = fHeader.BIT.fSize * sizeof(short) - sizeof(fHeader);
    fBlockBuffer = fBlockBuffer + sizeof(fHeader);
    fEOB = false;
    return;
  }

  if (!source->IsPrepared())
    return;
  if (source->Read((char *)&fHeader, sizeof(fHeader), 0) == 0)
  {
    source->SetBlockStatus(kEOF);
    fHeader.BYTE = 0;
    source->Wait();
    return;
  }
  fBlockSize = fHeader.BIT.fSize * sizeof(short) - sizeof(fHeader);
  if (source->Read((char *)fBlockBuffer, fBlockSize, 0) == 0)
  {
    source->SetBlockStatus(kEOF);
    fHeader.BYTE = 0;
    source->Wait();
    return;
  }
  source->Wait();
  fEOB = false;

  return;
}

bool TArtParserRIDF::GetNextEvent(TArtRawEventObject *raweve)
{

  fEventStatus = kERROREVENT;
  // TArtCore::Debug("TArtParserRIDF", "GetNextEvent");

  while (!fEOB)
  { // scanning comments given at beginning

    fHeader.BYTE = *(unsigned long long int *)&fBlockBuffer[fNext];
    // fHeader.BIT.Print(); // print out only in the case of debugging mode
    fOffset = fNext;
    fPrev = fNext;
    fNext = fOffset + fHeader.BIT.fSize * sizeof(short);
    if (fBlockSize <= fNext)
      fEOB = true;

    if (fHeader.BIT.fClassID != 0 && // Event Fragment
        fHeader.BIT.fClassID != 3 && // Event
        fHeader.BIT.fClassID != 4 &&
        fHeader.BIT.fClassID != 5 &&
        fHeader.BIT.fClassID != 6 &&
        fHeader.BIT.fClassID != 8 &&
        fHeader.BIT.fClassID != 9 &&
        fHeader.BIT.fClassID != 11 &&
        fHeader.BIT.fClassID != 12 &&
        fHeader.BIT.fClassID != 13 &&
        fHeader.BIT.fClassID != 21)
    {
      std::cout << "TArtParserRIDF: Unsupported Block ClassID: " << fHeader.BIT.fClassID << std::endl;
    }

    if (fHeader.BIT.fClassID == 5)
    { // Comment header
      struct ridf_comment_runinfost info;
      int idx, cid;

      idx = fOffset + sizeof(int) * 3; // skip header
      std::memcpy((char *)&cid, fBlockBuffer + idx, sizeof(int));
      if (cid == 1)
      {                                  // Comment ID 1 = Header
        idx = fOffset + sizeof(int) * 4; // skip header
        std::memcpy((char *)&info, fBlockBuffer + idx, sizeof(info));
        raweve->SetRunNumber(strtol(info.runnumber, NULL, 10));
        std::cout << "TArtParserRIDF: Run number = " << raweve->GetRunNumber() << std::endl;
      }
      fEventStatus = kCOMMENT;
    }
    else if (fHeader.BIT.fClassID == 8)
    { // counter of the number of block
    }
    else if (fHeader.BIT.fClassID == 9)
    { // block ender
    }
    else if (fHeader.BIT.fClassID == 21)
    { // status data
      unsigned int size =
          fHeader.BIT.fSize * sizeof(short)
          //-sizeof(fHeader)-sizeof(int)*3;
          - sizeof(int) * 4;
      int idx, cid;
      idx = fOffset + sizeof(int) * 3; // skip header
      std::memcpy((char *)&cid, fBlockBuffer + idx, sizeof(int));
      // TArtCore::Info(__FILE__,"status ID: %d",cid);

      idx += sizeof(int);
      raweve->CopyStatusData(fBlockBuffer + idx, size);
      std::cout << "status data:\n"
                << raweve->GetStatusData() << std::endl;
    }
    else
    {
      break;
    }
  }

  if (fHeader.BIT.fClassID == 3 || // header for event w/o t.s.
      fHeader.BIT.fClassID == 6)
  { // header for event w/ t.s.
    int event_number;
    fOffset += sizeof(fHeader);
    std::memcpy((char *)&event_number, fBlockBuffer + fOffset, sizeof(int));
    fOffset += sizeof(int);
    raweve->SetEventNumber(event_number);

    if (fHeader.BIT.fClassID == 6)
    { // w/ timestamp header
      unsigned long long int timestamp;
      std::memcpy((char *)&timestamp, fBlockBuffer + fOffset, sizeof(timestamp));
      fOffset += sizeof(timestamp);
      raweve->SetTimeStamp(timestamp);
      // TArtCore::Debug("TArtParserRIDF", "Time Stamp = %lld", raweve->GetTimeStamp());
    }

    fEventStatus = kNORMALEVENT;
  }

  while (fOffset < fNext)
  {                                                   // scanning segment data
    fHeader = *(ArtHeader_t *)&fBlockBuffer[fOffset]; // segment size

    if (fHeader.BIT.fClassID == 4)
    { // header for segment
      if (isSkip)
      { // skip in the case of isSkip flag is on
        raweve->PutSegment(new TArtRawSegmentObject);
      }
      else
      {
        ArtSegIDRIDF_t segid =
            *(ArtSegIDRIDF_t *)(fBlockBuffer + fOffset + sizeof(fHeader));
        unsigned int size =
            fHeader.BIT.fSize * sizeof(short) - sizeof(fHeader) - sizeof(unsigned int);
        // TArtCore::Info("TArtParserRIDF","segid = %08x",segid);
        unsigned char *buf = fBlockBuffer + fOffset + sizeof(fHeader) + sizeof(segid);

        // TArtCore::Debug("TArtParserRIDF", "fp=%d,det=%d,mod=%d",
        //                 segid.BIT.fFP, segid.BIT.fDetector, segid.BIT.fModule);

        TArtRawSegmentObject *seg = new TArtRawSegmentObject(segid.BIT.fDevice, segid.BIT.fFP, segid.BIT.fDetector, segid.BIT.fModule);
        seg->SetAddress((int)fHeader.BIT.fAddress);
        seg->SetDataSize((int)size);

        TArtDecoder *decoder = TArtDecoderFactory::Instance()->GetDecoder(segid.BIT.fModule);
        // if(20 == segid.BIT.fModule || 15 == segid.BIT.fModule){
        // }
        // else if (!decoder) {
        if (!decoder)
        {
          std::cout << "TArtParserRIDF: No such decoder ID = " << segid.BIT.fModule << std::endl;
        }
        else
        {
          TArtDecoderFactory::Instance()->GetDecoder(segid.BIT.fModule)->Decode(buf, size, seg);
          raweve->PutSegment(seg);
        }
      }
    } // end of if (fHeader.BIT.fClassID == 4)
    else if (fHeader.BIT.fClassID == 11 || // header for non-clear 24bit scaler
             fHeader.BIT.fClassID == 12 || // header for clear 24bit scaler
             fHeader.BIT.fClassID == 13)
    { // header for non-clear 32bit scaler
      int scaler_date, scaler_id, num_scaler, idx;
      num_scaler = (fHeader.BIT.fSize * sizeof(short) - sizeof(fHeader) - 2 * sizeof(int)) / sizeof(int);
      idx = fOffset + sizeof(fHeader);
      std::memcpy((char *)&scaler_date, fBlockBuffer + idx, sizeof(int));
      idx += sizeof(int);
      std::memcpy((char *)&scaler_id, fBlockBuffer + idx, sizeof(int));
      idx += sizeof(int);
      TArtRawScalerObject *scl = new TArtRawScalerObject(num_scaler, scaler_id, scaler_date);
      std::memcpy((char *)scl->GetScaler(), fBlockBuffer + idx, num_scaler * sizeof(int));
      raweve->PutScaler(scl);
    } // end of if (fHeader.BIT.fClassID == 11,12,13)

    fOffset += fHeader.BIT.fSize * sizeof(short);

  } // end of while (fOffset < fNext)

  return true;
}

int TArtParserRIDF::CopyPrevEvent(char *dest)
{
  int size;

  size = fNext - fPrev;
  std::memcpy(dest, fBlockBuffer + fPrev, size);

  return size;
}
