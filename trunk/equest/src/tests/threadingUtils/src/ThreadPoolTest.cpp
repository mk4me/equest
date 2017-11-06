#include "ThreadPoolTest.h"
#include <threadingUtils/InterruptibleWorkManager.h>
#include <threadingUtils/InterruptibleThreadPool.h>
#include <threadingUtils/RunnableThread.h>
#include <threadingUtils/InterruptHandlingPolicy.h>
#include <threadingUtils/CallPolicy.h>
#include <atomic>

CPPUNIT_TEST_SUITE_REGISTRATION( ThreadPoolTest );

using namespace threadingUtils;

//! Przerywalny w¹tek z funkcj¹ runnable
typedef RunnableThread<std::thread, RawCallPolicy> TestRunnableThread;
//! Wewnêtrza realizacja puli w¹tków
typedef InterruptibleThreadPool<TestRunnableThread, RawCallPolicy, InterrupltiblePolicy, ResetInterruptHandlingPolicy> TestThreadPool;

static std::atomic<int> testVal = 0;

void testFunction()
{
	++testVal;
}

void ThreadPoolTest::createAndDestroyDefault()
{
	TestThreadPool tp;
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(tp.minThreads() > 0);
	CPPUNIT_ASSERT(tp.maxThreads() > 0);
	CPPUNIT_ASSERT(tp.maxThreads() >= tp.minThreads());
}

void ThreadPoolTest::createAndDestroyCustom()
{
	TestThreadPool tp(2, 10);
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(tp.minThreads() == 2);
	CPPUNIT_ASSERT(tp.maxThreads() == 10);
}

void ThreadPoolTest::createAndDestroySingleThreadNoRun()
{
	TestThreadPool tp(2, 10);
	auto t = tp.get();
	CPPUNIT_ASSERT(tp.threadsCount() == 1);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	t = TestThreadPool::Thread();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	TestThreadPool::Threads ts;
	CPPUNIT_ASSERT(tp.get(1, ts, true) == 1);
	CPPUNIT_ASSERT(tp.threadsCount() == 1);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts.size() == 1);
	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	CPPUNIT_ASSERT(tp.get(1, ts, false) == 1);
	CPPUNIT_ASSERT(tp.threadsCount() == 1);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts.size() == 1);
	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
}

void ThreadPoolTest::createAndDestroyMultipleThreadsNoRun()
{
	TestThreadPool tp(2, 10);
	TestThreadPool::Threads ts;

	CPPUNIT_ASSERT(tp.get(5, ts, false) == 5);
	CPPUNIT_ASSERT(tp.threadsCount() == 5);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts.size() == 5);
	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	CPPUNIT_ASSERT(tp.get(5, ts, true) == 5);
	CPPUNIT_ASSERT(tp.threadsCount() == 5);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts.size() == 5);	
	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	CPPUNIT_ASSERT(tp.get(tp.maxThreads(), ts, true) == tp.maxThreads());
	CPPUNIT_ASSERT(tp.threadsCount() == tp.maxThreads());
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts.size() == tp.maxThreads());
	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	CPPUNIT_ASSERT(tp.get(tp.maxThreads() + 2, ts, false) == tp.maxThreads());
	CPPUNIT_ASSERT(tp.threadsCount() == tp.maxThreads());
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts.size() == tp.maxThreads());
	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	TestThreadPool::Threads ts2;
	CPPUNIT_ASSERT(tp.get(tp.maxThreads(), ts, true) == tp.maxThreads());
	CPPUNIT_ASSERT(tp.get(1, ts2, false) == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts2.size() == 0);
	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
}

void ThreadPoolTest::toMuchExceptions()
{
	TestThreadPool tp(2, 10);
	TestThreadPool::Threads ts;
	CPPUNIT_ASSERT(tp.get(tp.maxThreads(), ts, true) == tp.maxThreads());
	TestThreadPool::Threads ts2;
	CPPUNIT_ASSERT_THROW(tp.get(1, ts2, true), std::runtime_error);
	CPPUNIT_ASSERT(tp.threadsCount() == tp.maxThreads());
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);	
	TestThreadPool::Thread t;
	CPPUNIT_ASSERT_THROW(t = tp.get(), std::runtime_error);
	CPPUNIT_ASSERT(tp.threadsCount() == tp.maxThreads());
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT_THROW(tp.get(tp.maxThreads() + 2, ts, true), std::runtime_error);
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
}

void ThreadPoolTest::createAndDestroySingleThreadRun()
{
	TestThreadPool tp(2, 10);
	auto t = tp.get();
	CPPUNIT_ASSERT(tp.threadsCount() == 1);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	t.run(&testFunction);
	t = TestThreadPool::Thread();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 1);
	CPPUNIT_ASSERT(testVal == 1);
	testVal = 0;
}

void ThreadPoolTest::createAndDestroySingleThreadReRun()
{
	TestThreadPool tp(2, 10);
	auto t = tp.get();
	t.run(&testFunction);
	t = TestThreadPool::Thread();
	t = tp.get();
	CPPUNIT_ASSERT(tp.threadsCount() == 1);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);

	t.run(&testFunction);
	t = TestThreadPool::Thread();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 1);
	CPPUNIT_ASSERT(testVal == 2);
	testVal = 0;
}

void ThreadPoolTest::createAndDestroyMultipleThreadsRun()
{
	TestThreadPool tp(2, 10);
	TestThreadPool::Threads ts;

	const auto requestedThreadsCount = std::min(tp.minThreads() + 2, tp.maxThreads());

	CPPUNIT_ASSERT(tp.get(requestedThreadsCount, ts, false) == requestedThreadsCount);
	CPPUNIT_ASSERT(tp.threadsCount() == requestedThreadsCount);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts.size() == requestedThreadsCount);
	
	for (auto & t : ts){
		t.run(&testFunction);
	}	
	
	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == tp.minThreads());
	CPPUNIT_ASSERT(testVal == requestedThreadsCount);

	CPPUNIT_ASSERT(tp.get(requestedThreadsCount, ts, false) == requestedThreadsCount);
	CPPUNIT_ASSERT(tp.threadsCount() == requestedThreadsCount);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == 0);
	CPPUNIT_ASSERT(ts.size() == requestedThreadsCount);

	for (auto & t : ts){
		t.run(&testFunction);
	}

	ts = TestThreadPool::Threads();
	CPPUNIT_ASSERT(tp.threadsCount() == 0);
	CPPUNIT_ASSERT(tp.cachedThreadsCount() == tp.minThreads());
	CPPUNIT_ASSERT(testVal == 2 * requestedThreadsCount);
	testVal = 0;
}