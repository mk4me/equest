#include "JobManagerTest.h"
#include <threading/JobManager.h>
#include <threading/ThreadPool.h>
#include <vector>
#include <threading/QtThreadFactory.h>
#include <utils/SmartPtr.h>
#include <threading/SynchronizationPolicies.h>


using namespace utils;

class SimpleTestRunnable : public IRunnable
{
public:

	SimpleTestRunnable(int val) : val(val)
	{
		synch_.lock();
	}

	virtual void run()
	{
		++val;
		synch_.unlock();
	}

	const int value() const
	{
		return val;
	}

	void join()
	{
		ScopedLock<StrictSyncPolicy> lock(synch_);
	}

private:
	volatile int val;
	StrictSyncPolicy synch_;
};

void JobManagerTest::testWorking()
{
	{
		std::vector<shared_ptr<SimpleTestRunnable>> runnables;
		for(unsigned int i = 0; i < 1000; ++i){
			runnables.push_back(shared_ptr<SimpleTestRunnable>(new SimpleTestRunnable(i)));
		}
		
		{
			IThreadFactoryPtr tf(new QtThreadFactory);
			JobManager jm(tf->createThread());
			for(auto it = runnables.begin(); it != runnables.end(); ++it){
				jm.addJob(*it);
			}
		}

		bool ok = true;
		for(unsigned int i = 0; i < 1000; ++i){
			if(runnables[i]->value() != i){
				ok = false;
				break;
			}
		}

		CPPUNIT_ASSERT_EQUAL(true, ok);
	}

	{
		std::vector<shared_ptr<SimpleTestRunnable>> runnables;
		for(unsigned int i = 0; i < 1000; ++i){
			runnables.push_back(shared_ptr<SimpleTestRunnable>(new SimpleTestRunnable(i)));
		}

		{
			IThreadFactoryPtr tf(new QtThreadFactory);
			IThreadPoolPtr tp(new ThreadPool(tf));

			JobManager jm(tp->getThread());

			for(auto it = runnables.begin(); it != runnables.end(); ++it){
				jm.addJob(*it);
			}

			jm.addWorkerThread(tp->getThread());
			jm.addWorkerThread(tp->getThread());
			jm.addWorkerThread(tp->getThread());

			for(auto it = runnables.begin(); it != runnables.end(); ++it){
				(*it)->join();
			}
		}

		bool ok = true;
		for(unsigned int i = 0; i < 1000; ++i){
			if(runnables[i]->value() != i + 1){
				ok = false;
				break;
			}
		}

		CPPUNIT_ASSERT_EQUAL(true, ok);
	}

	{
		std::vector<shared_ptr<SimpleTestRunnable>> runnables;
		for(unsigned int i = 0; i < 1000; ++i){
			runnables.push_back(shared_ptr<SimpleTestRunnable>(new SimpleTestRunnable(i)));
		}

		{
			IThreadFactoryPtr tf(new QtThreadFactory);
			IThreadPoolPtr tp(new ThreadPool(tf));

			JobManager jm(tp->getThread());

			jm.addWorkerThread(tp->getThread());
			jm.addWorkerThread(tp->getThread());
			jm.addWorkerThread(tp->getThread());

			for(auto it = runnables.begin(); it != runnables.end(); ++it){
				jm.addJob(*it);
			}

			for(auto it = runnables.begin(); it != runnables.end(); ++it){
				(*it)->join();
			}
		}

		bool ok = true;
		for(unsigned int i = 0; i < 1000; ++i){
			if(runnables[i]->value() != i + 1){
				ok = false;
				break;
			}
		}

		CPPUNIT_ASSERT_EQUAL(true, ok);
	}
}

void JobManagerTest::testWorkerAddition()
{

}

void JobManagerTest::testWorkerRemoval()
{

}