#include "TArtDecoderRPV130.hh"
#include "TArtDecoderFactory.hh"
#include <stdio.h>
TArtDecoderRPV130::TArtDecoderRPV130()
    : TArtDecoder(kID)
{
}

TArtDecoderRPV130::~TArtDecoderRPV130()
{
}

TArtDecoder *TArtDecoderRPV130::Instance()
{
    static TArtDecoderRPV130 instance;
    return &instance;
}

int TArtDecoderRPV130::Decode(unsigned char *&buf, const unsigned int &size,
                              TArtRawSegmentObject *rawseg)
{
    const int igeo = 0;
    const int ich = 0;

    if (size < 1)
    {
        auto *rdata = new TArtRawDataObject(igeo, ich, 0xff);
        rawseg->PutData(rdata);
    }
    else
    {
        auto *rdata = new TArtRawDataObject(igeo, ich, buf[0] & 0x00ff);
        rawseg->PutData(rdata);
    }

    return 0;
}
