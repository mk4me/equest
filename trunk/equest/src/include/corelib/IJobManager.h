/********************************************************************
	created:  2015/08/12	13:20:02
	filename: IJobManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__IJOBMANAGER_H__
#define __HEADER_GUARD_CORELIB__IJOBMANAGER_H__

#include <corelib/JobManagerTypes.h>
#include <corelib/Job.h>

namespace core
{
	class IJobManager
	{
	public:

		//! Domy�lny konstruktor
		IJobManager(InnerJobManager * jm) : jm(jm) {}

		//! Desturktor
		virtual ~IJobManager() {}

		template<typename F, class ...Args>
		Job<typename std::result_of<F(Args...)>::type> create(const std::string & owner, const std::string & name, F&& f, Args&& ...arguments)
		{
			typedef typename std::result_of<F(Args...)>::type result_type;
			auto description = utils::make_shared<JobBase::Description>(JobBase::Description{ owner, name });

			std::function<result_type()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

			auto innerJob = jm->create(
				[=]() -> result_type
			{
				logError("Job " + name + " owned by " + owner + " started");
				JobBase::JobGuard guard(description);
				try{
					logError("Job " + name + " owned by " + owner + " executing");
					return (result_type)ff();
				}
				catch (std::exception & e){
					logError(e.what());
					throw;
				}
				catch (...){
					logError("UNKNOWN error");
					throw;
				}
			});

			return Job<result_type>(std::move(innerJob), description);
		}

		template<typename JobType>
		void waitForOtherJob(JobType & job)
		{
			logError("Job " << job.name() << " owned by " << job.owner() << " waiting for other job");
			jm->waitForOtherJob(job);
		}

	private:

		virtual void logError(const std::string & message) = 0;

	private:

		InnerJobManager * jm;
	};
}

#endif	// __HEADER_GUARD_CORELIB__IJOBMANAGER_H__
