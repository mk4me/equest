#include "SaveSceneTest.h"
#include "../src/plugins/newVdf/src/SceneModel.h"
#include "plugins/dfElements/DFProcessors.h"
#include "utils/CommandStack.h"
#include "../src/plugins/newVdf/src/TypesWindow.h"
#include "../src/core/src/ThreadPool.h"
#include "../src/corelib/thread/qthread.h"
#include <QtWidgets/QApplication>
#include "../src/plugins/newVdf/src/DataProcessorManager.h"
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( SaveSceneTest);

using namespace vdf;

void SaveSceneTest::setUp()
{
    char *argv[2];
    int argc = 1;
    argv[0]="foo.exe";
    argv[1]=nullptr;
    application = new QApplication(argc, argv);
}

struct testInitializer
{
    testInitializer() :
        editor(utils::make_shared<CanvasStyleEditor>()),
        sb(editor),
        dataProcessorManager(utils::make_shared<DataProcessorManager>())
    {
        const core::ThreadPool::size_type threadsCount = QThread::idealThreadCount();
		model = utils::make_shared<SceneModel>(editor, nullptr);
        tm = utils::make_shared<TypesModel>(utils::make_shared<utils::CommandStack>(), editor, model);
        dataProcessorManager->attach(tm.get());
    }
    
    CanvasStyleEditorPtr editor;
    SceneModelPtr model;
    vdf::SceneBuilder sb;

    DataProcessorManagerPtr dataProcessorManager;
    utils::shared_ptr<TypesModel> tm;
};

void SaveSceneTest::testSaveScene()
{
    testInitializer test;    

    auto processor =  vdf::IDataProcessorPtr(new vdf::DataProcessor(
        new ScalarAdd, core::UID::GenerateUniqueID("{11111111-1111-1111-1111-111111111111}"), "ScalarAdd",
        [&](const df::IProcessingNode* p) { return new ScalarAdd(); }));
    test.dataProcessorManager->registerDataProcessor(processor);
    
    auto itm1 = test.tm->createItemByEntry("ScalarAdd");//, QPointF(3.0f, 4.0f));
    auto itm2 = test.tm->createItemByEntry("ScalarAdd");//, QPointF(303.0f, 404.0f));
    itm1.get<0>()->visualItem()->setPos(3.0f, 4.0f);
    itm2.get<0>()->visualItem()->setPos(333.0f, 444.0f);
    test.model->addItem(itm1.get<0>());
    test.model->addItem(itm2.get<0>());
    test.model->addConnection(utils::dynamic_pointer_cast<vdf::IVisualOutputPin>(itm1.get<2>()[0]), 
        utils::dynamic_pointer_cast<vdf::IVisualInputPin>(itm2.get<1>()[0]));

    SceneModel::Serializer serializer(test.model, test.tm);
    utils::Filesystem::Path p("test.txt");
    std::ofstream ofs(p.c_str());
    assert(ofs.good());
    boost::archive::xml_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(serializer);
    ofs.close();

    {
        testInitializer test2;
        test2.dataProcessorManager->registerDataProcessor(processor);
            //for read
        std::ifstream ifs(p.c_str());
        boost::archive::xml_iarchive xmlIn(ifs);
        SceneModel::Serializer s(test2.model, test2.tm);
        xmlIn >> BOOST_SERIALIZATION_NVP(s);

        auto nodes = test.model->getVisualItems<IVisualNodePtr>();
        auto nodes2 = test2.model->getVisualItems<IVisualNodePtr>();
        CPPUNIT_ASSERT_EQUAL(nodes.size(), nodes2.size());

        auto connections = test.model->getVisualItems<IVisualConnectionPtr>();
        auto connections2 = test2.model->getVisualItems<IVisualConnectionPtr>();
        CPPUNIT_ASSERT_EQUAL(connections.size(), connections2.size());
        
        IVisualSourceNodePtr onode1 = utils::dynamic_pointer_cast<IVisualSourceNode>(nodes[0]);
        IVisualSinkNodePtr inode1 = utils::dynamic_pointer_cast<IVisualSinkNode>(nodes[1]);


        IVisualSourceNodePtr onode2 = utils::dynamic_pointer_cast<IVisualSourceNode>(nodes2[0]);
        IVisualSinkNodePtr inode2 = utils::dynamic_pointer_cast<IVisualSinkNode>(nodes2[1]);

        CPPUNIT_ASSERT(onode1 && onode2);
        CPPUNIT_ASSERT(inode1 && inode2);

        CPPUNIT_ASSERT_EQUAL(onode1->getOutputPin(0)->getNumConnections(), 1);
        CPPUNIT_ASSERT_EQUAL(onode2->getOutputPin(0)->getNumConnections(), 1);

        CPPUNIT_ASSERT(onode1->getOutputPin(0)->getConnection(0).lock()->getInputPin()->getParent().lock() == inode1);
        CPPUNIT_ASSERT(onode2->getOutputPin(0)->getConnection(0).lock()->getInputPin()->getParent().lock() == inode2);
    }
    
}

void SaveSceneTest::tearDown()
{
	delete application;
}
