#ifndef TARTDECODERMQDC32_H
#define TARTDECODERMQDC32_H
#include "TArtDecoder.hh"
class TArtDecoderMQDC32  : public TArtDecoder {
private:
   TArtDecoderMQDC32();

public:
   static const int kID = 33;
   static TArtDecoder* Instance();
   ~TArtDecoderMQDC32();
   int Decode(unsigned char* &buf, const unsigned int& size,
	      TArtRawSegmentObject *rawseg);
};
#endif // end of #ifdef TARTDECODERMQDC32_H
