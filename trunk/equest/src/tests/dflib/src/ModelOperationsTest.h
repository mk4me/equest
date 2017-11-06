#ifndef HEADER_GUARD__DFMTEST_MODEL_H__
#define HEADER_GUARD__DFMTEST_MODEL_H__

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <dflib/ModelImpl.h>

class ModelOperationsTest : public CppUnit::TestFixture
{
public:
	ModelOperationsTest(void);
	virtual ~ModelOperationsTest(void);

	void testBasicNodeAddition();
    void testSameNodeAddition();
    void testOtherModelNodeAddition();

    void testBasicNodeRemove();
    void testOtherModelNodeRemove();

	void testBasicConnecting();
    void testDuplicatedConnecting();
    void testDifferentModelsConnecting();

	void testConnectionRemove();
    void testDifferentModelConnectionRemove();

	//void testModelRunning();
	void testSimplestModel();
	void testSingleThreadModelRunning();

	CPPUNIT_TEST_SUITE( ModelOperationsTest );
	CPPUNIT_TEST( testBasicNodeAddition );
    CPPUNIT_TEST_EXCEPTION( testSameNodeAddition, df::NodeInModelEditionException );
    CPPUNIT_TEST_EXCEPTION( testOtherModelNodeAddition, df::NodeInModelEditionException );

    CPPUNIT_TEST( testBasicNodeRemove );
    CPPUNIT_TEST_EXCEPTION( testOtherModelNodeRemove, df::NodeInModelEditionException );

    CPPUNIT_TEST( testBasicConnecting );
    CPPUNIT_TEST_EXCEPTION( testDuplicatedConnecting, df::PinsConnectionException );
    CPPUNIT_TEST_EXCEPTION( testDifferentModelsConnecting, df::PinsConnectionException );    

    CPPUNIT_TEST( testConnectionRemove );
    CPPUNIT_TEST_EXCEPTION( testDifferentModelConnectionRemove, df::PinsConnectionException );

	//CPPUNIT_TEST( testModelRunning );
	CPPUNIT_TEST(testSimplestModel);
	CPPUNIT_TEST(testSingleThreadModelRunning);

	CPPUNIT_TEST_SUITE_END();

};

#endif
