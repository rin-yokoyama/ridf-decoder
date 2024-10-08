/*
 * @file TArtDecoderLUPO.cc
 * modified by T. Isobe
 */
#include "TArtDecoderLUPO.hh"
#include "TArtDecoderFactory.hh"
#include <stdio.h>
#include <cstring>
TArtDecoderLUPO::TArtDecoderLUPO()
    : TArtDecoder(kID)
{
}

TArtDecoderLUPO::~TArtDecoderLUPO()
{
}

TArtDecoder *TArtDecoderLUPO::Instance()
{
   static TArtDecoderLUPO instance;
   return &instance;
}

int TArtDecoderLUPO::Decode(unsigned char *&buf, const unsigned int &size,
                            TArtRawSegmentObject *rawseg)
{

   // do not put timestamp data in the meantime, because timestamp
   // information is in header
   unsigned int ts0, ts1 = size;

   unsigned long long int timestamp = 0;
   std::memcpy((char *)&timestamp, buf, sizeof(timestamp));
   timestamp = timestamp << 16;
   timestamp = timestamp >> 16;
   ts0 = timestamp >> 32;
   ts1 = timestamp & 0xffffffff;

   // first 16bit is in ts0 and last 32bit is in ts1
   // original timestamp can be driven from
   // unsigned long long int ts = ts0;
   // TArtCore::Info("TArtDecoderLUPO","ts: %lld",(ts<<32)+ts1);

   // 10nsec step timestamp can be driven from
   // TArtCore::Info("TArtDecoderLUPO","ts: %lld",((ts<<32)+ts1)>>4);

   TArtRawDataObject *rdata0 = new TArtRawDataObject(0, 0, ts0);
   rawseg->PutData(rdata0);
   TArtRawDataObject *rdata1 = new TArtRawDataObject(0, 1, ts1);
   rawseg->PutData(rdata1);
   return 0;
}
