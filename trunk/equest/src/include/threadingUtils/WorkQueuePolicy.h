/********************************************************************
	created:  2014/10/06	18:51:19
	filename: WorkQueuePolicy.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__WORKQUEUEPOLICY_H__
#define __HEADER_GUARD_THREADINGUTILS__WORKQUEUEPOLICY_H__

#include <threadingUtils/Export.h>
#include <map>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <thread>
#include <threadingUtils/Macros.h>
#include <threadingUtils/Future.h>
#include <threadingUtils/FunctionWrapper.h>
#include <threadingUtils/Queue.h>
#include <threadingUtils/StealingQueue.h>
#include <utils/SmartPtr.h>

namespace threadingUtils
{

	struct THREADINGUTILS_EXPORT Task
	{
		std::thread::id id;
		FunctionWrapper functionWrapper;
	};

	class THREADINGUTILS_EXPORT StealingMultipleWorkQueuePolicy
	{
		using WorkStealingQueue = StealingQueue<FunctionWrapper>;
		using LocalQueues = std::map<std::thread::id, utils::shared_ptr<WorkStealingQueue>>;

		Queue<FunctionWrapper> commonQueue;
		LocalQueues localQueues;
		mutable std::mutex mutex;

	public:

		using size_type = Queue<FunctionWrapper>::size_type;

	public:

		StealingMultipleWorkQueuePolicy();

		~StealingMultipleWorkQueuePolicy();
		
		void initializeThread();

		void deinitializeThread();

		size_type size() const;		

		template<typename F>
		std::future<typename std::result_of<F()>::type> submit(F && f)
		{
			using result_type = typename std::result_of<F()>::type;
			std::packaged_task<result_type()> innerTask(std::forward<F>(f));
			return innerSubmit(std::move(innerTask));
		}

		bool tryGet(Task & task);

		void clearLocalQueue();

		bool empty() const;

	private:

		template<typename RT>
		std::future<RT> innerSubmit(std::packaged_task<RT()> && innerTask)
		{
			std::future<RT> ret(innerTask.get_future());
			FunctionWrapper fw(std::move(innerTask));
			if (localWorkQueue() != nullptr) {
				localWorkQueue()->push(std::move(fw));
			}
			else {
				commonQueue.push(std::move(fw));
			}

			return ret;
		}

		void rescheduleLocalQueue();
		
		bool localQueuesEmpty() const;

		bool tryPopWorkFromLocalQueue(Task & task);

		bool tryPopWorkFromCommonQueue(Task & task);

		bool tryPopWorkFromOtherLocalQueue(Task & task);

		static StealingQueue<FunctionWrapper> *& localWorkQueue();
	};	
}

#endif	// __HEADER_GUARD_THREADINGUTILS__WORKQUEUEPOLICY_H__