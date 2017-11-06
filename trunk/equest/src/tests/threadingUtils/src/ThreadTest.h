/********************************************************************
	created:  2014/11/26	10:56:07
	filename: ThreadTest.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___THREADTEST_H__
#define __HEADER_GUARD___THREADTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class ThreadTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ThreadTest);
	CPPUNIT_TEST(runnableThread);
	CPPUNIT_TEST(multipleRunThread);
	CPPUNIT_TEST(interruptibleThread);
	CPPUNIT_TEST_SUITE_END();

public:
	void runnableThread();
	void multipleRunThread();
	void interruptibleThread();
};

#endif	// __HEADER_GUARD___THREADTEST_H__