/********************************************************************
	created:	2013/06/28
	created:	28:6:2013   14:30
	filename: 	MergeTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST_NEW_VDF__SAVESCENETEST_H__
#define HEADER_GUARD_TEST_NEW_VDF__SAVESCENETEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <utils/SmartPtr.h>
#include <QtWidgets/QApplication>


class SaveSceneTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SaveSceneTest);
        CPPUNIT_TEST(testSaveScene);
    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    void testSaveScene();

	virtual void tearDown() override;

private:
    QApplication* application;
};

#endif
