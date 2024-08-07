#include "TArtDecoderSIS3820.hh"
#include "TArtDecoderFactory.hh"
#include <stdio.h>
TArtDecoderSIS3820::TArtDecoderSIS3820()
    : TArtDecoder(kID)
{
}

TArtDecoderSIS3820::~TArtDecoderSIS3820()
{
}

TArtDecoder *TArtDecoderSIS3820::Instance()
{
    static TArtDecoderSIS3820 instance;
    return &instance;
}

int TArtDecoderSIS3820::Decode(unsigned char *&buf, const unsigned int &size,
                               TArtRawSegmentObject *rawseg)
{
    int igeo = 0;
    int ich = 0;
    const unsigned int *evtdata = (unsigned int *)buf;
    const unsigned int evtsize = size / sizeof(unsigned int);

    if (evtsize != 32)
        return 0;

    for (int i = 0; i < evtsize; ++i)
    {
        ich = i;
        auto *rdata = new TArtRawDataObject(igeo, ich, evtdata[i]);
        rawseg->PutData(rdata);
    }

    return 0;
}
