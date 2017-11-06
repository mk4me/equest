/********************************************************************
	created:	2013/07/01
	created:	1:7:2013   21:32
	filename: 	VisualizerContextTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_TEST__VISUALIZERCONTEXTTEST_H__
#define HEADER_GUARD_MDE_TEST__VISUALIZERCONTEXTTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include "../src/mde_view/src/VisualizerEventFilter.h"
#include <QtCore/QObject>

class ConnectionTester : public QObject
{
    Q_OBJECT
public:
    ConnectionTester() : widget(nullptr) {}
    QWidget* getWidget() const { return widget; }

public slots:
    void onFocus(QWidget* w) 
{ 
	widget = w; 
}

private:
    QWidget* widget;
};

class VisualizerContextTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(VisualizerContextTest);
        CPPUNIT_TEST(testEventFilter);
    CPPUNIT_TEST_SUITE_END();

public:
    void testEventFilter();
    virtual void setUp();
    virtual void tearDown();

private:
    QApplication* app;
};

#endif
