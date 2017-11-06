/********************************************************************
    created:  2013/03/29
    created:  29:3:2013   19:13
    filename: ThreadPoolTest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___THREADPOOLTEST_H__
#define HEADER_GUARD___THREADPOOLTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class ThreadPoolTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ThreadPoolTest);
	CPPUNIT_TEST(createAndDestroyDefault);
	CPPUNIT_TEST(createAndDestroyCustom);
	CPPUNIT_TEST(createAndDestroySingleThreadNoRun);
	CPPUNIT_TEST(createAndDestroyMultipleThreadsNoRun);
	CPPUNIT_TEST(toMuchExceptions);
	CPPUNIT_TEST(createAndDestroySingleThreadRun);
	CPPUNIT_TEST(createAndDestroySingleThreadReRun);
	CPPUNIT_TEST(createAndDestroyMultipleThreadsRun);
	CPPUNIT_TEST_SUITE_END();

public:
	void createAndDestroyDefault();
	void createAndDestroyCustom();
	void createAndDestroySingleThreadNoRun();
	void createAndDestroyMultipleThreadsNoRun();
	void toMuchExceptions();
	void createAndDestroySingleThreadRun();
	void createAndDestroySingleThreadReRun();
	void createAndDestroyMultipleThreadsRun();
};

#endif	//	HEADER_GUARD___THREADPOOLTEST_H__
