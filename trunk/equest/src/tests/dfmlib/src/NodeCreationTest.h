#ifndef HEADER_GUARD__DFMTEST_NODE_H__
#define HEADER_GUARD__DFMTEST_NODE_H__

#include <testnodefactory/TestDFNodeProcessing.h>
#include <testnodefactory/TestDFNodeSource.h>

#include <cppunit/extensions/HelperMacros.h>
#include <string>


class NodeCreationTest : public CppUnit::TestFixture
{
public:
	NodeCreationTest(void);
	~NodeCreationTest(void);

	void setUp();

	void testProcessingNodeCreation();
	void testSourceNodeCreation();

	CPPUNIT_TEST_SUITE( NodeCreationTest );
	CPPUNIT_TEST( testProcessingNodeCreation );
	CPPUNIT_TEST( testSourceNodeCreation );
	CPPUNIT_TEST_SUITE_END();

private:
	dflm::DFNPtr ProcNodeA;
	dflm::DFNPtr ProcNodeB;
	dflm::DFNPtr ProcNodeC;
	dflm::DFNPtr ProcNodeD;
	dflm::DFNPtr ProcNodeE;
	dflm::DFNPtr ProcNodeF;
	dflm::DFNPtr ProcNodeG;
	dflm::DFNPtr ProcNodeH;

	dflm::DFSNPtr SrcNodeA;
	dflm::DFSNPtr SrcNodeB;
	dflm::DFSNPtr SrcNodeC;
	dflm::DFSNPtr SrcNodeD;
	dflm::DFSNPtr SrcNodeE;
};

#endif
