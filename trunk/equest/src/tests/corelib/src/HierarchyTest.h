/********************************************************************
	created:	2013/06/28
	created:	28:6:2013   13:23
	filename: 	HierarchyTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST_CORELIB__HIERARCHYTEST_H__
#define HEADER_GUARD_TEST_CORELIB__HIERARCHYTEST_H__

#include <corelib/HierarchyHelper.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class HierarchyTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(HierarchyTest);
        CPPUNIT_TEST(testHierarchyHelper);
        CPPUNIT_TEST(testShallowCopy);
    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    void testHierarchyHelper();
    void testShallowCopy();

private:
    core::IHierarchyItemPtr createSimpleTree();
};

#endif
