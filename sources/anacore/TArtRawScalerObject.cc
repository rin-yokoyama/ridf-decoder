#include "TArtRawScalerObject.hh"

TArtRawScalerObject::TArtRawScalerObject(const int nch, const int sid, const int dd)
{
  if (nch > SCALER_MAX_CHANNEL)
    std::cout << "num of scaler is more than SCALER_MAX_CHANNEL: " << nch << std::endl;
  num_channel = nch;
  scaler_id = sid;
  scaler_date = dd;
}

TArtRawScalerObject::TArtRawScalerObject() : num_channel(0),
                                             scaler_id(-1)
{
}

void TArtRawScalerObject::SetNumChannel(const int nch)
{
  if (nch > SCALER_MAX_CHANNEL)
    std::cout << "num of scaler is more than SCALER_MAX_CHANNEL: " << nch << std::endl;
  if (num_channel != 0)
    Clear();
  num_channel = nch;
  return;
}

void TArtRawScalerObject::Clear()
{
  num_channel = 0;
  return;
}
