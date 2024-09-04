
#ifndef TARTDECODERTC842_H
#define TARTDECODERTC842_H
#include "TArtDecoder.hh"
class TArtDecoderTC842 : public TArtDecoder
{
private:
   TArtDecoderTC842();

public:
   static const int kID = 30;
   static TArtDecoder *Instance();
   virtual ~TArtDecoderTC842();
   virtual int Decode(unsigned char *&buf, const unsigned int &size,
                      TArtRawSegmentObject *rawseg);
};
#endif // end of #ifdef TARTDECODERTC842_H
