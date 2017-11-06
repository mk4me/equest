#include "VisualizerContextTest.h"
#include "../src/medusa_view/src/ContextEventFilter.h"
#include <QtTest/qtest.h>
#include <QtTest/qtestmouse.h>
#include "../src/medusa_view/src/MdeMainWindow.h"
#include "../src/medusa_view/src/MdeContexts.h"
#include "QApplication"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( VisualizerContextTest );

void VisualizerContextTest::testEventFilter()
{
	MdeMainWindow mmw([](){}, "test");
	//QWindow window;
	//std::unique_ptr<QWidget> w(QWidget::createWindowContainer(&window));
	auto w = std::make_unique<QWidget>();
	coreUI::IAppUsageContextPtr context = utils::make_shared<MdeEmptyContext>();
	mmw.addContext(context);
	mmw.addWidgetToContext(context, w.get());
	ContextEventFilterPtr cef = utils::make_shared<ContextEventFilter>(&mmw);
	cef->registerClosableContextWidget(w.get());
	VisualizerEventFilter vef(cef);
	ConnectionTester ct;
	QObject::connect(&vef, SIGNAL(focusOn(QWidget*)), &ct, SLOT(onFocus(QWidget*)));
	w->installEventFilter(&vef);
	QTest::mouseClick(w.get(), Qt::MouseButton::LeftButton);

	CPPUNIT_ASSERT(ct.getWidget() == w.get());
	CPPUNIT_ASSERT(mmw.isCurrentContextWidget(w.get()));
	CPPUNIT_ASSERT(mmw.getCurrentContextWidget() == w.get());
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


