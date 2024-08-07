
#ifndef TARTDECODESIS3820_H
#define TARTDECODESIS3820_H
#include "TArtDecoder.hh"
class TArtDecoderSIS3820 : public TArtDecoder
{
private:
    TArtDecoderSIS3820();
    TArtDecoderSIS3820(const TArtDecoderSIS3820 &rhs) { ; }

public:
    static const int kID = 36;
    static TArtDecoder *Instance();
    ~TArtDecoderSIS3820();
    int Decode(unsigned char *&buf, const unsigned int &size,
               TArtRawSegmentObject *rawseg);
};
#endif // end of #ifdef TArtDecoderSIS3820_H
