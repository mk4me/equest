/********************************************************************
	created:	2014/06/21
	created:	21:6:2014   9:53
	filename: 	ImuParsersTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_IMUTEST__IMUPARSERSTEST_H__
#define HEADER_GUARD_IMUTEST__IMUPARSERSTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class ImuParsersTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ImuParsersTest);
        CPPUNIT_TEST(testSplit);
    CPPUNIT_TEST_SUITE_END();

public:

    virtual void setUp();

    void testSplit();
};

#endif
