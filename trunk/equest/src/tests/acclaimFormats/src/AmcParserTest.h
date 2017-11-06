#ifndef HEADER_GUARD_AMCPARSERTEST_H__
#define HEADER_GUARD_AMCPARSERTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class AmcParserTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(AmcParserTest);
    CPPUNIT_TEST(testLoad);
	//CPPUNIT_TEST_EXCEPTION(testLoadNotExisting, std::runtime_error);
    CPPUNIT_TEST_SUITE_END();
public:
    AmcParserTest(void);
    virtual ~AmcParserTest(void);

public:
    void testLoad();
    //void testLoadNotExisting();
};
#endif
