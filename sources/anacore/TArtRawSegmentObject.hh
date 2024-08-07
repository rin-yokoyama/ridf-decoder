#ifndef TARTRAWSEGMENTOBJECT_H
#define TARTRAWSEGMENTOBJECT_H

#include <iostream>
#include <vector>
#include "TArtRawDataObject.hh"

class TArtRawSegmentObject
{
public:
  TArtRawSegmentObject(const unsigned int indev, const unsigned int infp,
                       const unsigned int indet, const unsigned int inmod)
  {
    device = indev;
    fp = infp;
    detector = indet;
    module = inmod;
    nmodule = 0;
    ngoodmodule = 0;
    size = 0;
  }
  TArtRawSegmentObject();
  virtual ~TArtRawSegmentObject() { Clear(); }

  virtual void SetSegmentID(const unsigned int indev,
                            const unsigned int infp,
                            const unsigned int indet,
                            const unsigned int inmod)
  {
    device = indev;
    fp = infp;
    detector = indet;
    module = inmod;
  }
  virtual int GetDevice() { return device; }
  virtual int GetFP() { return fp; }
  virtual int GetDetector() { return detector; }
  virtual int GetModule() { return module; }
  virtual void Clear();

  // for DAQ analysis / debugging
  virtual int GetAddress() { return address; }
  virtual void SetAddress(int i) { address = i; }
  virtual int GetDataSize() { return size; }
  virtual void SetDataSize(int i) { size = i; }
  virtual int GetNumModule() { return nmodule; }
  virtual void SetNumModule(int i) { nmodule = i; }
  virtual int GetNumGoodModule() { return ngoodmodule; }
  virtual void SetNumGoodModule(int i) { ngoodmodule = i; }

  virtual void PutData(TArtRawDataObject *indata)
  {
    data_array.push_back(indata);
    return;
  }
  virtual TArtRawDataObject *GetData(int nn)
  {
    return data_array[nn];
  }
  virtual int GetNumData() { return data_array.size(); }

protected:
  // device id, BigRIPS, ZDC, Sharaq
  int device;
  // focal plane
  int fp;
  // detector id, such as DALI, PPAC ...
  int detector;
  // module id, such as CAEN-V490
  int module;

  int address;     // supposed to be same as EFN in DAQ
  int nmodule;     // number of modules
  int ngoodmodule; // number of good modules
  int size;        // data size

  // array of data
  std::vector<TArtRawDataObject *> data_array;
};

#endif
