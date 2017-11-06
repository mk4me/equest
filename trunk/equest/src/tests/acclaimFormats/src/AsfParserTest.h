#ifndef HEADER_GUARD_ASFPARSERTEST_H__
#define HEADER_GUARD_ASFPARSERTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class AsfParserTest : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(AsfParserTest);
	CPPUNIT_TEST(testLoadUnits);
	CPPUNIT_TEST(testLoadMetadata);
	CPPUNIT_TEST(testRoot);
	CPPUNIT_TEST(testSingleBone);
	CPPUNIT_TEST(testBones);
	CPPUNIT_TEST(testHierarchy);
    //CPPUNIT_TEST_EXCEPTION(testLoadNotExisting, std::runtime_error);
    CPPUNIT_TEST_SUITE_END();
public:
    AsfParserTest(void);
    virtual ~AsfParserTest(void);

public:
	virtual void setUp();
	virtual void tearDown();

	void testLoadMetadata();
	void testLoadUnits();
    //void testLoadNotExisting();
	void testHierarchy();
	void testBones();
	void testSingleBone();
	void testRoot();
private:
	std::unique_ptr<acclaim::Skeleton> skeleton;
};
#endif
