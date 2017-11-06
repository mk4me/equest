/********************************************************************
	created:  2015/08/12	15:34:52
	filename: JobManagerTypes.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__JOBMANAGERTYPES_H__
#define __HEADER_GUARD_CORELIB__JOBMANAGERTYPES_H__

#include <loglib/ILog.h>
#include <corelib/Thread.h>
#include <threadingUtils/InterruptibleWorkManager.h>
#include <threadingUtils/InterruptibleJobManager.h>

namespace core
{

	class LogWrapper
	{
	public:
		LogWrapper(loglib::ILogPtr log) : log_(log) {}
		LogWrapper(const LogWrapper & Other) : log_(Other.log_) {}
		LogWrapper(LogWrapper && Other) : log_(std::move(Other.log_)) {}
		~LogWrapper() {}

		inline void log(const std::string & message)
		{
			log_->log(loglib::ILog::Debug, message);
		}

	private:

		loglib::ILogPtr log_;
	};

	//typedef threadingUtils::InterruptibleWorkManager<threadingUtils::StealingMultipleWorkQueuePolicy, Thread, threadingUtils::ConsumeExceptionHandlePolicy, threadingUtils::NoInterruptHandlingPolicy, LogWrapper> InnerWorkManager;
	typedef threadingUtils::InterruptibleWorkManager<threadingUtils::StealingMultipleWorkQueuePolicy, Thread, threadingUtils::ConsumeExceptionHandlePolicy, threadingUtils::NoInterruptHandlingPolicy> InnerWorkManager;
	typedef threadingUtils::InterruptibleJobManager<InnerWorkManager> InnerJobManager;
}

#endif	// __HEADER_GUARD_CORELIB__JOBMANAGERTYPES_H__