#ifndef HEADER_GUARD_BVHPARSERTEST_H__
#define HEADER_GUARD_BVHPARSERTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class BvhParserTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(BvhParserTest);
    CPPUNIT_TEST(testLoad);
    CPPUNIT_TEST(fullTest);
    CPPUNIT_TEST_SUITE_END();
public:
    BvhParserTest(void);
    virtual ~BvhParserTest(void);

public:
    void testLoad();
    void fullTest();

private:
    std::vector<std::string> fill(const std::string& rootPath);
};
#endif
