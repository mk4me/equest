#include "MergeTest.h"
#include "..\src\plugins\newVdf\src\BackgroundsFactory.h"
#include "..\src\plugins\newVdf\src\DarkBackgroundStrategy.h"
#include <plugins/dfElements/DFProcessors.h>
#include "../src/plugins/newVdf/src/SceneModel.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MergeTest );

using namespace vdf;

void MergeTest::setUp()
{
}

void MergeTest::testMerge()
{
    /*auto backgroundFactory = IBackgroundsFactoryPtr(new BackgroundsFactory<DarkBackgroundStrategy>("Dark", "Dark desc", QUuid()));
    IBackgroundStrategyPtr strategy = backgroundFactory->createBackgroundStrategy();
    CPPUNIT_ASSERT(strategy);
    auto darkStrategy = utils::dynamic_pointer_cast<DarkBackgroundStrategy>(strategy);
    CPPUNIT_ASSERT(darkStrategy);*/

    //CanvasStyleEditorPtr editor = utils::make_shared<CanvasStyleEditor>();
    //
    //SceneModel model(editor);
    //QList<QGraphicsItem*> itms;
    ////itms.push_back(vdf::SceneBuilder::createProcessing("test1", QIcon(), new ScalarAdd(), 2, 1 ));
    //vdf::SceneBuilder::VisualNodeWithPins merged = model.merge(itms);
}
