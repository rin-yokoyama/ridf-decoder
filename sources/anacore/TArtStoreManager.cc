/*
 *    TArtStoreManager.cc
 *
 *    by T. Isobe
 */

#include "TArtRawEventObject.hh"
#include "TArtEventInfo.hh"
#include "TArtStoreManager.hh"

TArtStoreManager *TArtStoreManager::sStoreManager = 0;
//////////////////////////////////////////////////

TArtStoreManager::TArtStoreManager()
{
  std::cout << "TArtStoreManager: Constructor" << std::endl;
}

TArtStoreManager::~TArtStoreManager()
{
  sStoreManager = NULL;
}

TArtStoreManager *TArtStoreManager::Instance()
{
  if (!sStoreManager)
    sStoreManager = new TArtStoreManager();
  return sStoreManager;
}
