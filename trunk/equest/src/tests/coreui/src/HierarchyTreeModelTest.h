/********************************************************************
	created:	2014/04/25
	created:	25:4:2014   13:48
	filename: 	HierarchyTreeModelTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUITEST__HIERARCHYTREEMODELTEST_H__
#define HEADER_GUARD_COREUITEST__HIERARCHYTREEMODELTEST_H__

#include <corelib/HierarchyHelper.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class HierarchyTreeModelTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(HierarchyTreeModelTest);
        CPPUNIT_TEST(testModel);
        CPPUNIT_TEST(testModelView);
    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    void testModel();
    void testModelView();
    virtual void tearDown();

private:
    core::IHierarchyItemPtr createSimpleTree();
};

#endif
