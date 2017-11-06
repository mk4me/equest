/********************************************************************
	created:  2014/10/26	16:59:40
	filename: JobManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__JOBMANAGER_H__
#define __HEADER_GUARD_CORELIB__JOBMANAGER_H__

#include <corelib/IJobManager.h>
#include <loglib/ILog.h>

namespace core
{
	class JobManager : public IJobManager
	{
	public:

		//! Domyœlny konstruktor
		JobManager(InnerJobManager * jm);
		//! Desturktor
		virtual ~JobManager();		

	private:

		virtual void logError(const std::string & message) override;

	private:

		//! Logger
		loglib::ILogPtr logger_;
	};
}

#endif	// __HEADER_GUARD_CORELIB__JOBMANAGER_H__
