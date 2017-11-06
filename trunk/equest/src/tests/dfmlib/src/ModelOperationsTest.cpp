#include "ModelOperationsTest.h"
#include <testnodefactory/TestNode.h>
#include <testnodefactory/TestNodeFactory.h>

#include <dfmlib/DFModel.h>

CPPUNIT_TEST_SUITE_REGISTRATION( ModelOperationsTest );

ModelOperationsTest::ModelOperationsTest(void) : TestFixture()
{
}


ModelOperationsTest::~ModelOperationsTest(void)
{
}

void ModelOperationsTest::setUp()
{
    DFModelA.reset(new dflm::DFModel());
    DFModelB.reset(new dflm::DFModel());
    DFModelC.reset(new dflm::DFModel());
    DFModelD.reset(new dflm::DFModel());
    DFModelE.reset(new dflm::DFModel());
    DFModelF.reset(new dflm::DFModel());
    DFModelG.reset(new dflm::DFModel());

    DFNodeA = TestNodeFactory::createProcessingNode("A", 2, 4, 1, false);
    DFNodeB = TestNodeFactory::createProcessingNode("B", 4, 4, 2, false);
    DFNodeC = TestNodeFactory::createProcessingNode("C", 3, 1, 2, false);
    DFNodeD = TestNodeFactory::createProcessingNode("D", 1, 1, 0, false);
    DFNodeE = TestNodeFactory::createProcessingNode("E", 2, 2, 1, false);
    DFNodeF = TestNodeFactory::createProcessingNode("F", 2, 3, 2, false);
    DFNodeG = TestNodeFactory::createProcessingNode("G", 1, 4, 0, false);

    DFSourceNodeA = TestNodeFactory::createSourceNode("SA", 2);
    DFSourceNodeB = TestNodeFactory::createSourceNode("SB", 1);
    DFSourceNodeC = TestNodeFactory::createSourceNode("SC", 3);
    DFSourceNodeD = TestNodeFactory::createSourceNode("SD", 2);
    DFSourceNodeE = TestNodeFactory::createSourceNode("SE", 4);
    DFSourceNodeF = TestNodeFactory::createSourceNode("SF", 2);
    DFSourceNodeG = TestNodeFactory::createSourceNode("SG", 3);
}

void ModelOperationsTest::testBasicNodeAddition()
{
    DFModelA->addNode(DFNodeA);
    
    CPPUNIT_ASSERT( DFNodeA->getModel() == DFModelA);
    CPPUNIT_ASSERT( DFModelA->getNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getSourceNodes().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().begin()->second.size() == 1 );

    DFModelA->addNode(DFNodeC);

    CPPUNIT_ASSERT( DFNodeC->getModel() == DFModelA);
    CPPUNIT_ASSERT( DFModelA->getNodes().size() == 2 );
    CPPUNIT_ASSERT( DFModelA->getSourceNodes().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 2 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().size() == 2 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().find(DFNodeA)->second.size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().find(DFNodeC)->second.size() == 2 );
}

void ModelOperationsTest::testSameNodeAddition()
{
    DFModelA->addNode(DFNodeA);
    DFModelA->addNode(DFNodeA);
}

void ModelOperationsTest::testOtherModelNodeAddition()
{
    DFModelB->addNode(DFNodeB);
    DFModelA->addNode(DFNodeB);
}

void ModelOperationsTest::testUnsupportedNodeAddition()
{
    DFModelA->addNode(dflm::NPtr(new TestNode("TestNode")));
}

void ModelOperationsTest::testBasicNodeRemove()
{
    DFModelA->addNode(DFNodeA);
    DFModelA->addNode(DFNodeC);

    DFModelA->removeNode(DFNodeA);
    CPPUNIT_ASSERT( DFNodeA->getModel() == nullptr);
    CPPUNIT_ASSERT( DFModelA->getNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getSourceNodes().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().find(DFNodeC)->second.size() == 2 );
}

void ModelOperationsTest::testOtherModelNodeRemove()
{
    DFModelB->addNode(DFNodeB);
    DFModelA->removeNode(DFNodeB);
}

void ModelOperationsTest::testDFSourceNodeAddition()
{
    DFModelA->addNode(DFNodeC);
    DFModelA->addNode(DFSourceNodeA);
    CPPUNIT_ASSERT( DFSourceNodeA->getModel() == DFModelA);
    CPPUNIT_ASSERT( DFModelA->getNodes().size() == 2 );
    CPPUNIT_ASSERT( DFModelA->getSourceNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().find(DFNodeC)->second.size() == 2 );
}

void ModelOperationsTest::testDFSourceNodeRemove()
{
    DFModelA->addNode(DFNodeC);
    DFModelA->addNode(DFSourceNodeA);
    DFModelA->removeNode(DFSourceNodeA);
    CPPUNIT_ASSERT( DFSourceNodeA->getModel() == nullptr);
    CPPUNIT_ASSERT( DFModelA->getNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getSourceNodes().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 0 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getRequiringConnections().find(DFNodeC)->second.size() == 2 );
}

void ModelOperationsTest::testBasicConnecting()
{
    DFModelA->addNode(DFSourceNodeA);
    DFModelA->addNode(DFNodeA);
    DFModelA->addNode(DFNodeC);

    CPPUNIT_ASSERT( DFModelA->getNodes().size() == 3 );
    CPPUNIT_ASSERT( DFModelA->getSourceNodes().size() == 1 );

    DFModelA->connect(DFSourceNodeA->getOutPin(0), DFNodeA->getInPin(0));

    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 2 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 1 );

    DFModelA->connect(DFNodeA->getOutPin(0), DFNodeC->getInPin(0));

    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 2 );
}

void ModelOperationsTest::testDuplicatedConnecting()
{
    DFModelA->addNode(DFSourceNodeA);
    DFModelA->addNode(DFNodeA);

    DFModelA->connect(DFSourceNodeA->getOutPin(0), DFNodeA->getInPin(0));
    DFModelA->connect(DFSourceNodeA->getOutPin(0), DFNodeA->getInPin(0));
}

void ModelOperationsTest::testInvalidConnecting()
{
    DFModelB->addNode(DFNodeB);
    DFModelA->addNode(DFSourceNodeA);
    DFModelA->connect(DFSourceNodeA->getOutPin(0), DFNodeB->getInPin(0));
}

void ModelOperationsTest::testConnectedInputPinConnecting()
{
    DFModelA->addNode(DFSourceNodeA);
    DFModelA->addNode(DFNodeA);

    DFModelA->connect(DFSourceNodeA->getOutPin(0), DFNodeA->getInPin(0));
    DFModelA->connect(DFSourceNodeA->getOutPin(1), DFNodeA->getInPin(0));
}

void ModelOperationsTest::testCycleConnecting()
{
    DFModelA->addNode(DFSourceNodeA);
    DFModelA->addNode(DFNodeA);
    DFModelA->addNode(DFNodeC);

    DFModelA->connect(DFSourceNodeA->getOutPin(0), DFNodeA->getInPin(0));
    DFModelA->connect(DFNodeA->getOutPin(0), DFNodeC->getInPin(0));
    DFModelA->connect(DFNodeC->getOutPin(0), DFNodeA->getInPin(1));
}

void ModelOperationsTest::testConnectionRemove()
{
    DFModelA->addNode(DFSourceNodeA);
    DFModelA->addNode(DFNodeA);
    DFModelA->addNode(DFNodeC);

    DFModelA->connect(DFSourceNodeA->getOutPin(0), DFNodeA->getInPin(0));
    DFModelA->connect(DFNodeA->getOutPin(0), DFNodeC->getInPin(0));

    DFModelA->removeConnection(*(DFNodeA->getInPin(0)->begin()));

    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 1 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 1 );

    DFModelA->removeConnection(*(DFNodeA->getOutPin(0)->begin()));

    CPPUNIT_ASSERT( DFModelA->getLeafNodes().size() == 2 );
    CPPUNIT_ASSERT( DFModelA->getConnections().size() == 0 );
}

void ModelOperationsTest::testVirtualConnectionRemove()
{
    DFModelB->addNode(DFNodeB);
    DFModelB->addNode(DFNodeD);
    dflm::ConnPtr conn(DFModelB->connect(DFNodeB->getOutPin(0), DFNodeD->getInPin(0)));

    DFModelA->removeConnection(conn);
}

void ModelOperationsTest::testRequiredConnections()
{
    CPPUNIT_ASSERT( true );
}
