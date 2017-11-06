#include "UniversalSourceTest.h"
#include <plugins/newVdf/UniversalSource.h>
#include <plugins/newVdf/UniversalInputPin.h>
#include <plugins/newVdf/UniversalOutputPin.h>
//#include "plugins/c3d/C3DChannels.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( UniversalSourceTest );


void UniversalSourceTest::setUp()
{
    char** argv = nullptr; int arc = 0;
    app = new QApplication(arc, argv);
}

void UniversalSourceTest::testCreate()
{
    vdf::UniversalSource<int> us;
    CPPUNIT_ASSERT(!us.isNodeValid());
    us.produce();
}

void UniversalSourceTest::tearDown()
{
    delete app;
}
