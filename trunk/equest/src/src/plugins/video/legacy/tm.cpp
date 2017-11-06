#include "VideoPCH.h"
#include <stdexcept>
#include <utils/Utils.h>
#include "tm.h"

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

TimelineManager * TimelineManager::instance = NULL;

TimelineManager * TimelineManager::createInstance()
{
  if ( instance == NULL ) {
    instance = new TimelineManager();
    // TUTAJ INICJUJEMY ZALEŻNE SINGLETONY
    return instance;
  } else {
    throw new std::runtime_error("Instance already exists.");
  }
}

TimelineManager * TimelineManager::getInstance()
{
  if (instance == NULL) {
    instance = createInstance();
  }
  return instance;
}

void TimelineManager::destroyInstance()
{
  UTIL_DELETEPTR(instance);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

