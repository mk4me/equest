#ifndef HEADER_GUARD__DFMTEST_MODEL_H__
#define HEADER_GUARD__DFMTEST_MODEL_H__

#include <testnodefactory/TestDFNodeProcessing.h>
#include <testnodefactory/TestDFNodeSource.h>

#include <cppunit/extensions/HelperMacros.h>
#include <string>


class ModelOperationsTest : public CppUnit::TestFixture
{
public:
	ModelOperationsTest(void);
	virtual ~ModelOperationsTest(void);

    virtual void setUp();

	void testBasicNodeAddition();
    void testSameNodeAddition();
    void testOtherModelNodeAddition();
    void testUnsupportedNodeAddition();

    void testBasicNodeRemove();
    void testOtherModelNodeRemove();

	void testDFSourceNodeAddition();
    void testDFSourceNodeRemove();

	void testBasicConnecting();
    void testDuplicatedConnecting();
    void testInvalidConnecting();
    void testConnectedInputPinConnecting();
    void testCycleConnecting();

	void testConnectionRemove();
    void testVirtualConnectionRemove();

    void testRequiredConnections();

	CPPUNIT_TEST_SUITE( ModelOperationsTest );
	CPPUNIT_TEST( testBasicNodeAddition );
    CPPUNIT_TEST_EXCEPTION( testSameNodeAddition, std::runtime_error );
    CPPUNIT_TEST_EXCEPTION( testOtherModelNodeAddition, std::runtime_error );
    CPPUNIT_TEST_EXCEPTION( testUnsupportedNodeAddition, std::runtime_error );

    CPPUNIT_TEST( testBasicNodeRemove );
    CPPUNIT_TEST_EXCEPTION( testOtherModelNodeRemove, std::runtime_error );

    CPPUNIT_TEST( testDFSourceNodeAddition );
    CPPUNIT_TEST( testDFSourceNodeRemove );

    CPPUNIT_TEST( testBasicConnecting );
    CPPUNIT_TEST_EXCEPTION( testDuplicatedConnecting, std::runtime_error );
    CPPUNIT_TEST_EXCEPTION( testInvalidConnecting, std::runtime_error );
    CPPUNIT_TEST_EXCEPTION( testConnectedInputPinConnecting, std::runtime_error );
    CPPUNIT_TEST_EXCEPTION( testCycleConnecting, std::runtime_error );

    CPPUNIT_TEST( testConnectionRemove );
    CPPUNIT_TEST_EXCEPTION( testVirtualConnectionRemove, std::runtime_error );

    CPPUNIT_TEST( testRequiredConnections );

	CPPUNIT_TEST_SUITE_END();

protected:

	dflm::DFMPtr DFModelA;
	dflm::DFMPtr DFModelB;
	dflm::DFMPtr DFModelC;
	dflm::DFMPtr DFModelD;
	dflm::DFMPtr DFModelE;
	dflm::DFMPtr DFModelF;
	dflm::DFMPtr DFModelG;

    dflm::DFNPtr DFNodeA;
    dflm::DFNPtr DFNodeB;
    dflm::DFNPtr DFNodeC;
    dflm::DFNPtr DFNodeD;
    dflm::DFNPtr DFNodeE;
    dflm::DFNPtr DFNodeF;
    dflm::DFNPtr DFNodeG;

    dflm::DFSNPtr DFSourceNodeA;
    dflm::DFSNPtr DFSourceNodeB;
    dflm::DFSNPtr DFSourceNodeC;
    dflm::DFSNPtr DFSourceNodeD;
    dflm::DFSNPtr DFSourceNodeE;
    dflm::DFSNPtr DFSourceNodeF;
    dflm::DFSNPtr DFSourceNodeG;

};

#endif
