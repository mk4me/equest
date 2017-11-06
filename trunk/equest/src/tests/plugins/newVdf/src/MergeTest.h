/********************************************************************
	created:	2013/06/28
	created:	28:6:2013   14:30
	filename: 	MergeTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST_NEW_VDF__MERGETEST_H__
#define HEADER_GUARD_TEST_NEW_VDF__MERGETEST_H__

#include <cppunit/extensions/HelperMacros.h>

class MergeTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MergeTest);
        CPPUNIT_TEST(testMerge);
    CPPUNIT_TEST_SUITE_END();

public:

    virtual void setUp();

    void testMerge();
};

#endif
