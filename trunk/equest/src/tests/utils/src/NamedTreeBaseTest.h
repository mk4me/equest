/********************************************************************
    created:  2011/03/21
    created:  21:3:2011   9:45
    filename: NamedTreeBaseTest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__NAMEDTREEBASETEST_H__
#define HEADER_GUARD_UTILS__NAMEDTREEBASETEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <utils/NamedTree.h>

class NamedTreeBaseTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(NamedTreeBaseTest);
    
    CPPUNIT_TEST(testRootStructure);

    CPPUNIT_TEST(testAddChild);
    CPPUNIT_TEST(testAddChildPose);
    CPPUNIT_TEST_EXCEPTION(testAddChildExist, std::runtime_error);
    CPPUNIT_TEST_EXCEPTION(testAddChildWithParent, std::runtime_error);

    CPPUNIT_TEST(testAddChildName);
    CPPUNIT_TEST(testAddChildNamePose);
    CPPUNIT_TEST_EXCEPTION(testAddChildNameExist, std::runtime_error);

    CPPUNIT_TEST(testAddChildPath);
    CPPUNIT_TEST(testAddChildPathPose);
    CPPUNIT_TEST_EXCEPTION(testAddChildPathExist, std::runtime_error);
    CPPUNIT_TEST_EXCEPTION(testAddChildPathBeyondRoot, std::runtime_error);

    CPPUNIT_TEST(testRemoveChild);
    CPPUNIT_TEST(testRemoveChildPose);
    CPPUNIT_TEST_EXCEPTION(testRemoveChildNotExist, std::runtime_error);

    CPPUNIT_TEST(testRemoveChildName);
    CPPUNIT_TEST_EXCEPTION(testRemoveChildNameNotExist, std::runtime_error);

    CPPUNIT_TEST(testRemoveChildPath);
    CPPUNIT_TEST_EXCEPTION(testGetChildPathNotExist, std::runtime_error);
    CPPUNIT_TEST_EXCEPTION(testRemoveChildPathBeyondRoot, std::runtime_error);

    CPPUNIT_TEST_SUITE_END();

public:

    virtual void setUp();

    void testRootStructure();

    void testAddChild();
    void testAddChildPose();
    void testAddChildExist();
    void testAddChildWithParent();

    void testAddChildName();
    void testAddChildNamePose();
    void testAddChildNameExist();

    void testAddChildPath();
    void testAddChildPathPose();
    void testAddChildPathExist();
    void testAddChildPathBeyondRoot();

    void testRemoveChild();
    void testRemoveChildPose();
    void testRemoveChildNotExist();

    void testRemoveChildName();
    void testRemoveChildNameNotExist();

    void testRemoveChildPath();
    void testGetChildPathNotExist();
    void testRemoveChildPathBeyondRoot();

private:
    NamedTreeBasePtr rootA;
    NamedTreeBasePtr rootB;
};

#endif
