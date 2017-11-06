/********************************************************************
	created:	2013/06/28
	created:	28:6:2013   14:16
	filename: 	AnalisisModelTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST_MDE_VIEW__ANALISISMODELTEST_H__
#define HEADER_GUARD_TEST_MDE_VIEW__ANALISISMODELTEST_H__


#include <cppunit/extensions/HelperMacros.h>
#include "../src/mde_view/src/AnalisisModel.h"

class AnalisisModelTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AnalisisModelTest);
        CPPUNIT_TEST(testTree);
    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

public:
    void testTree();

private:
    AnalisisModelPtr model;
};

#endif
