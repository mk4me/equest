#include "VisualizerContextTest.h"
#include "..\src\mde_view\src\ContextEventFilter.h"
#include <QtTest/qtest.h>
#include <QtTest/qtestmouse.h>
#include "../src/mde_view/src/MdeMainWindow.h"
#include "../src/mde_view/src/MdeContexts.h"
#include "QApplication"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( VisualizerContextTest );

void VisualizerContextTest::testEventFilter()
{
    MdeMainWindow mmw([](){}, "test");

    QWindow window;
	
	QWidget* w = QWidget::createWindowContainer(&window);
    coreUI::IAppUsageContextPtr context = utils::make_shared<MdeEmptyContext>();
    mmw.addContext(context);
	mmw.addWidgetToContext(context, w);
    ContextEventFilterPtr cef = utils::make_shared<ContextEventFilter>(&mmw);
	cef->registerClosableContextWidget(w);
    VisualizerEventFilter vef(cef);
    ConnectionTester ct;
    QObject::connect(&vef, SIGNAL(focusOn(QWidget*)), &ct, SLOT(onFocus(QWidget*)));
    w->installEventFilter(&vef);
	QTest::mouseClick(w, Qt::LeftButton);
	CPPUNIT_ASSERT_EQUAL(ct.getWidget(), w);
	CPPUNIT_ASSERT_EQUAL(mmw.getCurrentContextWidget(), w);
	CPPUNIT_ASSERT(mmw.isCurrentContextWidget(w));
	
}

void VisualizerContextTest::setUp()
{
    char** argv = nullptr; int arc = 0;
    app = new QApplication(arc, argv);
}

void VisualizerContextTest::tearDown()
{
    delete app;
}


