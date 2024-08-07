#include "TArtRawEventObject.hh"
#include <cstring>

TArtRawEventObject::TArtRawEventObject() : run_number(-2), event_number(-1), time_stamp(0), nblock(0)
{
}

void TArtRawEventObject::Clear()
{
  event_number = -1;
  for (int i = 0; i < GetNumSeg(); i++)
    delete segment_array[i];
  segment_array.clear();
  for (int i = 0; i < GetNumScaler(); i++)
    delete scaler_array[i];
  scaler_array.clear();
  return;
}

void TArtRawEventObject::Print()
{

  std::cout << "TArtRawEventObject: print out info in RawEventObject" << std::endl;
  std::cout << "TArtRawEventObject: Run Number: " << run_number << std::endl;
  std::cout << "TArtRawEventObject: Event Number: " << event_number << std::endl;
  std::cout << "TArtRawEventObject: Time Stamp: " << time_stamp << std::endl;
  std::cout << "TArtRawEventObject: Num Segments: " << segment_array.size() << std::endl;
  std::cout << "TArtRawEventObject: Num Scalers: " << scaler_array.size() << std::endl;
  std::cout << "TArtRawEventObject: Status Data: " << status_data << std::endl;

  return;
}

void TArtRawEventObject::CopyStatusData(unsigned char *org, size_t size)
{
  char *data = new char[size];
  std::memcpy(data, org, size);
  status_data = data;
}
