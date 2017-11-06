#include "ThreadTest.h"
#include <threading/QtThreadFactory.h>
#include <QtCore/QMutex>
#include <exception>
#include <algorithm>
#include <vector>

using namespace utils;

CPPUNIT_TEST_SUITE_REGISTRATION( ThreadTest );

class TestExceptionRunnable : public IRunnable
{
public:
	virtual void run()
	{
		throw std::runtime_error("Runnable exception");
	}
};

class TestRunnable : public IRunnable
{
public:

	TestRunnable(int val) : val(val), m1_(QMutex::NonRecursive), m2_(QMutex::NonRecursive)
	{
		m1_.lock();
	}

	virtual void run()
	{
		m1_.unlock();
		m2_.lock();
		++val;
	}

	const int value() const
	{
		return val;
	}

	QMutex & m1()
	{
		return m1_;
	}

	QMutex & m2()
	{
		return m2_;
	}

private:
	volatile int val;
	QMutex m1_;
	QMutex m2_;
};

void ThreadTest::testJoin()
{
	IThreadFactoryPtr tf(new QtThreadFactory);
	IThreadPtr t(tf->createThread());
	int val = 2;
	boost::shared_ptr<TestRunnable> runnable(new TestRunnable(val));
	t->start(runnable, IThread::Inheritate);
	t->join();
	CPPUNIT_ASSERT_EQUAL(val + 1, runnable->value());
}

void ThreadTest::testCancel()
{
	IThreadFactoryPtr tf(new QtThreadFactory);
	IThreadPtr t(tf->createThread());
	int val = 2;
	boost::shared_ptr<TestRunnable> runnable(new TestRunnable(val));
	runnable->m2().lock();
	t->start(runnable, IThread::Inheritate);
	runnable->m1().lock();
	t->cancel();
	t->join();
	CPPUNIT_ASSERT_EQUAL(val, runnable->value());
}