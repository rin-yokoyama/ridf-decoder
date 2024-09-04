
#include "TArtDecoderTC842.hh"
#include <cstring>

TArtDecoderTC842::TArtDecoderTC842()
    : TArtDecoder(kID)
{
}
TArtDecoderTC842::~TArtDecoderTC842()
{
}

TArtDecoder *TArtDecoderTC842::Instance()
{
      static TArtDecoderTC842 instance;
      return &instance;
}

int TArtDecoderTC842::Decode(unsigned char *&buf, const unsigned int &size,
                             TArtRawSegmentObject *rawseg)
{
      unsigned int *evtdata = (unsigned int *)buf;
      int igeo = 0;
      int edge = 0;

      double x[48], y = 1.e+12, t;
      unsigned int evtsize = size / sizeof(unsigned int);

      std::memcpy((char *)x, buf, size);

      for (unsigned int i = 0; i < evtsize; ++i)
      {
            t = x[i] * y;
            auto rdata = new TArtRawDataObject(igeo, i, (int)t);
            rdata->SetEdge(edge);
            rawseg->PutData(rdata);
      }

      return 0;
}
