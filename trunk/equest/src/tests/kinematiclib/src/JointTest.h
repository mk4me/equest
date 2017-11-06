#ifndef HEADER_GUARD_MISCTEST_H__
#define HEADER_GUARD_MISCTEST_H__

#include <limits>
#include <cppunit/extensions/HelperMacros.h>

class JointTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE(JointTest);
    //CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();
public:
	JointTest(void);
	virtual ~JointTest(void);

public:
    void test1();
};
#endif
