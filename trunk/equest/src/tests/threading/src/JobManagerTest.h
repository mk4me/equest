/********************************************************************
    created:  2013/07/02
    created:  2:7:2013   10:33
    filename: JobManagerTest.h
    author:   Mateusz Janiak
    
    purpose:  Test dla funkcjonalnoœci JobManagera
*********************************************************************/
#ifndef HEADER_GUARD___JOBMANAGERTEST_H__
#define HEADER_GUARD___JOBMANAGERTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <threading/IThreadPool.h>

class JobManagerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(JobManagerTest);
	CPPUNIT_TEST(testWorking);
	CPPUNIT_TEST(testWorkerAddition);
	CPPUNIT_TEST(testWorkerRemoval);
	CPPUNIT_TEST_SUITE_END();

public:
	void testWorking();
	void testWorkerAddition();
	void testWorkerRemoval();	
};

#endif	//	HEADER_GUARD___JOBMANAGERTEST_H__
