/********************************************************************
	created:	2013/07/04
	created:	4:7:2013   19:49
	filename: 	UniversalSourceTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DFELEMENTS_TEST__UNIVERSALSOURCETEST_H__
#define HEADER_GUARD_DFELEMENTS_TEST__UNIVERSALSOURCETEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <QtWidgets/QApplication>

class UniversalSourceTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(UniversalSourceTest);
        CPPUNIT_TEST(testCreate);
    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
    
    void testCreate();

private:
    QApplication* app;
};

#endif
