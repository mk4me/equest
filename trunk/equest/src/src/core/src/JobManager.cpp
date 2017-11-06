#include "CorePCH.h"
#include "JobManager.h"
#include "ApplicationCommon.h"

#include <loglib/Exceptions.h>

using namespace core;

JobManager::JobManager(InnerJobManager * jm) : IJobManager(jm),
logger_(getLogInterface()->subLog("jobs"))
{
	
}

JobManager::~JobManager()
{

}

void JobManager::logError(const std::string & message)
{
	LOG_ERROR(logger_, "Job " << JobBase::currentName() << " owned by " << JobBase::currentOwner() << " failed with message: " << message);
}