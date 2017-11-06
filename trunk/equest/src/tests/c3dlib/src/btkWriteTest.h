#ifndef HEADER_GUARD__DFMTEST_MODEL_H__
#define HEADER_GUARD__DFMTEST_MODEL_H__

#include <cppunit/extensions/HelperMacros.h>
#include <string>

class btkWriteTest : public CppUnit::TestFixture
{
public:
	btkWriteTest(void) {}
	virtual ~btkWriteTest(void) {}

	void testBasicWrite();
	void testScalarChannelWrite();
    
	CPPUNIT_TEST_SUITE(btkWriteTest);
	CPPUNIT_TEST( testBasicWrite);
	CPPUNIT_TEST(testScalarChannelWrite);
    
	CPPUNIT_TEST_SUITE_END();

};

#endif
