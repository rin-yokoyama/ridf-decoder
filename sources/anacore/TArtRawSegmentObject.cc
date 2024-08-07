#include "TArtRawSegmentObject.hh"
#include "TArtRawDataObject.hh"

TArtRawSegmentObject::TArtRawSegmentObject() : device(-1),
                                               fp(-1),
                                               detector(-1),
                                               module(-1)
{
  nmodule = 0;
  ngoodmodule = 0;
  size = 0;
}

void TArtRawSegmentObject::Clear()
{
  for (int i = 0; i < GetNumData(); i++)
    delete data_array[i];
  data_array.clear();

  nmodule = 0;
  ngoodmodule = 0;
  size = 0;
  return;
}
