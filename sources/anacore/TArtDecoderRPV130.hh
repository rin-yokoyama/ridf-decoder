#ifndef TARTDECODERRPV130_H
#define TARTDECODERRPV130_H
#include "TArtDecoder.hh"
class TArtDecoderRPV130 : public TArtDecoder
{
private:
    TArtDecoderRPV130();
    TArtDecoderRPV130(const TArtDecoderRPV130 &rhs) { ; }

public:
    static const int kID = 8;
    static TArtDecoder *Instance();
    ~TArtDecoderRPV130();
    int Decode(unsigned char *&buf, const unsigned int &size,
               TArtRawSegmentObject *rawseg);
};
#endif // end of #ifdef TARTDECODERRPV130_H
