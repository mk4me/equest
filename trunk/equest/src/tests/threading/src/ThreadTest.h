/********************************************************************
    created:  2013/03/29
    created:  29:3:2013   16:52
    filename: ThreadTest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___THREADTEST_H__
#define HEADER_GUARD___THREADTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class ThreadTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ThreadTest);
	CPPUNIT_TEST(testJoin);
	CPPUNIT_TEST(testCancel);
	CPPUNIT_TEST_SUITE_END();

public:
	void testJoin();
	void testCancel();
};

#endif	//	HEADER_GUARD___THREADTEST_H__
