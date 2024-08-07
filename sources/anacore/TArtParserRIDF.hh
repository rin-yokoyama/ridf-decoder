/*
 * @file TArtParserRIDF.h
 * @date  Created : 2008/10/17 18:27:33 JST<BR>
 *  Last Modified : 2008/10/30 21:47:17 JST
 *--------------------------------------------------------
 *    Comment :
 *
 *--------------------------------------------------------
 *    Copyright (C)2008 by Shinsuke OTA <ota@ribf.riken.jp>
 */
#ifndef TARTPARSERRIDF_H
#define TARTPARSERRIDF_H
#include "TArtParser.hh"
#include "TArtEventStore.hh"
#include "TArtRawEventObject.hh"
#include <cstring>
class TArtParserRIDF : public TArtParser
{
public:
   typedef union
   {
      unsigned long long int BYTE;
      struct
      {
         unsigned long int fSize : 22;
         unsigned long int fClassID : 6;
         unsigned long int fLayer : 2;
         unsigned long int fReserved : 2;
         unsigned long int fAddress : 32;
         void Print()
         {
            std::cout << "TArtParserRIDF: Header Information" << std::endl;
            std::cout << "TArtParserRIDF: Size  = " << fSize << std::endl;
            std::cout << "TArtParserRIDF: Class = " << fClassID << std::endl;
            std::cout << "TArtParserRIDF: Layer = " << fLayer << std::endl;
            std::cout << "TArtParserRIDF: Addr  = " << fAddress << std::endl;
         }
      } BIT;
   } ArtHeader_t;

   typedef union
   {
      unsigned int BYTE;
      struct
      {
         unsigned int fModule : 8;
         unsigned int fDetector : 6;
         unsigned int fFP : 6;
         unsigned int fDevice : 6;
         unsigned int fVersion : 6;
      } BIT;
   } ArtSegIDRIDF_t;

private:
   ArtHeader_t fHeader;

public:
   virtual void GetNextBlock(TArtDataSource *source);
   virtual bool GetNextEvent(TArtRawEventObject *rawdata);
   virtual int CopyPrevEvent(char *dest);
   TArtParserRIDF();
   ~TArtParserRIDF();
};
#endif // end of #ifdef TARTPARSERRIDF_H
