#include "NodeCreationTest.h"
#include <testnodefactory/TestNodeFactory.h>
#include <dfmlib/DFPin.h>

//TODO
//testy dla fabryki do poprawki - węzeł jest inicjalizowany dopiero po podłączneiu go do modelu!!
//CPPUNIT_TEST_SUITE_REGISTRATION( NodeCreationTest );

NodeCreationTest::NodeCreationTest(void) : TestFixture()
{
}

int getNodeRequiredInputPins(const dflm::DFNPtr node){
    int ret = 0;

    for(auto it = node->beginIn(); it != node->endIn(); it++){
        if((*it)->isRequired() == true){
            ret++;
        }
    }

    return ret;
}


NodeCreationTest::~NodeCreationTest(void)
{
}

void NodeCreationTest::setUp(){
	ProcNodeA = TestNodeFactory::createProcessingNode("A", 0, 5);
	ProcNodeB = TestNodeFactory::createProcessingNode("B", 1, 2, 0);
	ProcNodeC = TestNodeFactory::createProcessingNode("C", 1, 2, 1);
	ProcNodeD = TestNodeFactory::createProcessingNode("D", 2, 1, 0);
	ProcNodeE = TestNodeFactory::createProcessingNode("E", 2, 1, 2, true);
	ProcNodeF = TestNodeFactory::createProcessingNode("F", 100, 500, 50);
	ProcNodeG = TestNodeFactory::createProcessingNode("G", 1000, 500, 2000);

	SrcNodeA = TestNodeFactory::createSourceNode("A", 0);
	SrcNodeB = TestNodeFactory::createSourceNode("B", 1);
	SrcNodeC = TestNodeFactory::createSourceNode("C", 2);
	SrcNodeD = TestNodeFactory::createSourceNode("D", 100);
	SrcNodeE = TestNodeFactory::createSourceNode("E", 1000);
}

void NodeCreationTest::testProcessingNodeCreation(){
	CPPUNIT_ASSERT( ProcNodeA == nullptr );
	CPPUNIT_ASSERT( ProcNodeB != nullptr );
	CPPUNIT_ASSERT( ProcNodeB->sizeIn() == 1 );
	CPPUNIT_ASSERT( ProcNodeB->sizeOut() == 2 );
    CPPUNIT_ASSERT( getNodeRequiredInputPins(ProcNodeB) ==  0);
	
	CPPUNIT_ASSERT( ProcNodeC != nullptr );
	CPPUNIT_ASSERT( ProcNodeC->sizeIn() == 1 );
	CPPUNIT_ASSERT( ProcNodeC->sizeOut() == 2 );
    CPPUNIT_ASSERT( getNodeRequiredInputPins(ProcNodeC) ==  1);

	CPPUNIT_ASSERT( ProcNodeD != nullptr );
	CPPUNIT_ASSERT( ProcNodeD->sizeIn() == 2 );
	CPPUNIT_ASSERT( ProcNodeD->sizeOut() == 1 );
	CPPUNIT_ASSERT( getNodeRequiredInputPins(ProcNodeD) ==  0);

	CPPUNIT_ASSERT( ProcNodeE != nullptr );
	CPPUNIT_ASSERT( ProcNodeE->sizeIn() == 2 );
	CPPUNIT_ASSERT( ProcNodeE->sizeOut() == 1 );
    CPPUNIT_ASSERT( getNodeRequiredInputPins(ProcNodeE) ==  2);

	CPPUNIT_ASSERT( ProcNodeF != nullptr );
	CPPUNIT_ASSERT( ProcNodeF->sizeIn() == 100 );
	CPPUNIT_ASSERT( ProcNodeF->sizeOut() == 500 );
	CPPUNIT_ASSERT( getNodeRequiredInputPins(ProcNodeF) ==  50);

	CPPUNIT_ASSERT( ProcNodeG != nullptr );
	CPPUNIT_ASSERT( ProcNodeG->sizeIn() == 1000 );
	CPPUNIT_ASSERT( ProcNodeG->sizeOut() == 500 );
	CPPUNIT_ASSERT( getNodeRequiredInputPins(ProcNodeG) ==  2000);
}

void NodeCreationTest::testSourceNodeCreation(){
	CPPUNIT_ASSERT( SrcNodeA == nullptr );
	CPPUNIT_ASSERT( SrcNodeB != nullptr );
	CPPUNIT_ASSERT( SrcNodeB->sizeIn() == 0 );
	CPPUNIT_ASSERT( SrcNodeB->sizeOut() == 1 );
	CPPUNIT_ASSERT( SrcNodeC != nullptr );
	CPPUNIT_ASSERT( SrcNodeC->sizeIn() == 0 );
	CPPUNIT_ASSERT( SrcNodeC->sizeOut() == 2 );
	CPPUNIT_ASSERT( SrcNodeD != nullptr );
	CPPUNIT_ASSERT( SrcNodeD->sizeIn() == 0 );
	CPPUNIT_ASSERT( SrcNodeD->sizeOut() == 100 );
	CPPUNIT_ASSERT( SrcNodeE != nullptr );
	CPPUNIT_ASSERT( SrcNodeE->sizeIn() == 0 );
	CPPUNIT_ASSERT( SrcNodeE->sizeOut() == 1000 );
}
