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
#include <threading/IThreadPool.h>

class ThreadPoolTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ThreadPoolTest);
	CPPUNIT_TEST(testCreate);
	CPPUNIT_TEST(testCreateGroup);
	CPPUNIT_TEST_EXCEPTION(testToMuchException, utils::NoFreeThreadAvaiable);	
	CPPUNIT_TEST_SUITE_END();

public:
	void testCreate();
	void testCreateGroup();
	void testToMuchException();	
};

#endif	//	HEADER_GUARD___THREADPOOLTEST_H__
