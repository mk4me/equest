/********************************************************************
	created:  2015/08/12	13:21:35
	filename: Job.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__JOB_H__
#define __HEADER_GUARD_CORELIB__JOB_H__

#include <string>
#include <corelib/Export.h>
#include <boost/thread/tss.hpp>
#include <utils/SmartPtr.h>
#include <corelib/JobManagerTypes.h>

namespace core
{
	class IJobManager;

	class CORELIB_EXPORT JobBase
	{
		friend class IJobManager;

	public:

		struct Description
		{
			std::string owner;
			std::string name;
		};

		struct CORELIB_EXPORT JobGuard
		{
			JobGuard(utils::shared_ptr<Description> description);

			~JobGuard();
		};

		friend struct JobGuard;

	private:		

		typedef boost::thread_specific_ptr<utils::shared_ptr<Description>> TLS;

	protected:

		JobBase();

		JobBase(const JobBase & Other) = delete;

		JobBase(utils::shared_ptr<Description> description);

		JobBase(JobBase && Other);

		virtual ~JobBase();

		//! \param Other W¹tek którego zasoby przejmujemy
		JobBase& operator=(JobBase&& Other) NOEXCEPT;

		JobBase& operator=(const JobBase&) = delete;

		void swap(JobBase & Other) NOEXCEPT;

	public:

		std::string owner() const;

		std::string name() const;

		static std::string currentOwner();

		static std::string currentName();

	private:

		utils::shared_ptr<Description> description;
		static TLS tlsJD;
	};

	template<typename T>
	class Job : public JobBase, public InnerJobManager::InterruptibleJob<T>
	{
		friend class IJobManager;
	public:

		typedef InnerJobManager::InterruptibleJob<T> InnerJob;

	private:

		Job(InnerJob && Other, utils::shared_ptr<Description> description)
			: InnerJob(std::move(Other)), JobBase(description)
		{

		}

	public:

		Job() = default;

		Job(Job && Other) : JobBase(std::move(static_cast<JobBase&>(Other))),
			InnerJob(std::move(static_cast<InnerJob&>(Other)))
		{

		}

		Job(const Job & Other) = delete;

		virtual ~Job()
		{

		}

		//! \param Other W¹tek którego zasoby przejmujemy
		Job& operator=(Job&& Other) NOEXCEPT
		{
			this->JobBase::operator =(std::move(static_cast<JobBase&>(Other)));
			this->InnerJob::operator =(std::move(static_cast<InnerJob&>(Other)));
			return *this;
		}

		Job& operator=(const Job&) = delete;

		void swap(Job & Other) NOEXCEPT
		{
			std::swap<JobBase>(*this, Other);
			std::swap<InnerJob>(*this, Other);
		}
	};
}

#endif	// __HEADER_GUARD_CORELIB__JOB_H__