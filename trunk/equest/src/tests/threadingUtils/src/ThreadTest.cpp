#include "ThreadTest.h"
#include "TestObject.h"
#include <threadingUtils/RunnableThread.h>
#include <threadingUtils/InterruptHandlingPolicy.h>
#include <threadingUtils/InterruptiblePolicy.h>
#include <threadingUtils/InterruptibleThread.h>
#include <threadingUtils/CallPolicy.h>
#include <thread>
#include <atomic>

CPPUNIT_TEST_SUITE_REGISTRATION(ThreadTest);

using namespace threadingUtils;

typedef RunnableThread<std::thread, RawCallPolicy> TestRunnableThread;
typedef MultipleRunThread<std::thread> TestMultipleRunThread;
typedef InterrubtibleThread<TestRunnableThread> TestNoInterruptibleThread;
typedef InterrubtibleThread<TestRunnableThread, NoInterruptHandlingPolicy, InterrupltiblePolicy> TestInterruptibleThread;
//typedef InterrubtibleThread<TestRunnableThread, ResetInterruptHandlingPolicy, InterrupltiblePolicy> TestInterruptibleThread;

void ThreadTest::runnableThread()
{
	TestObject to;
	TestRunnableThread t;
	CPPUNIT_ASSERT(t.joinable() == false);
	CPPUNIT_ASSERT(t.get_id() == std::thread::id());
	//Unspecified
	//CPPUNIT_ASSERT(t.native_handle() == std::thread::native_handle_type());
	t.run(&TestObject::inc, &to);
	t.join();
	CPPUNIT_ASSERT(t.joinable() == false);
	CPPUNIT_ASSERT(to.value() == 1);
	CPPUNIT_ASSERT_THROW(t.run(&TestObject::inc, &to), std::logic_error);
}

void ThreadTest::multipleRunThread()
{
	TestObject to;
	TestMultipleRunThread t;
	CPPUNIT_ASSERT(t.joinable() == false);
	CPPUNIT_ASSERT(t.get_id() == std::thread::id());
	//CPPUNIT_ASSERT(t.native_handle() == std::thread::native_handle_type());
	auto f = t.run(&TestObject::inc, &to);
	f.wait();
	CPPUNIT_ASSERT(t.joinable() == true);
	CPPUNIT_ASSERT(to.value() == 1);

	f = t.run(&TestObject::inc, &to);
	f.wait();

	CPPUNIT_ASSERT(t.joinable() == true);
	CPPUNIT_ASSERT(to.value() == 2);
	t.join();
}

void ThreadTest::interruptibleThread()
{
	
}