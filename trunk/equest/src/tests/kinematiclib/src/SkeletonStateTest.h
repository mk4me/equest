/********************************************************************
	created:	2015/01/22
	created:	10:44
	filename: 	SkeletonStateTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST__SkeletonStateTest_H__
#define HEADER_GUARD_TEST__SkeletonStateTest_H__


#include <limits>
#include <cppunit/extensions/HelperMacros.h>
#include <kinematiclib/SkeletonState.h>

class SkeletonStateTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE(SkeletonStateTest);
	CPPUNIT_TEST(testMapping);
	CPPUNIT_TEST(testFrameConvert);
	CPPUNIT_TEST(testConvertStateChange);
	CPPUNIT_TEST_SUITE_END();
public:
	SkeletonStateTest(void);
	virtual ~SkeletonStateTest(void);
	
public:
	void testMapping();
	void testFrameConvert();
	void testConvertStateChange();
	virtual void setUp();
	virtual void tearDown();

private:
	std::unique_ptr<acclaim::Skeleton> acclaimSkeleton;
	std::unique_ptr<acclaim::MotionData> acclaimData;
	std::unique_ptr<acclaim::Skeleton::HelperMotionData> acclaimhelperMotionData;
	std::size_t acclaimActiveBones;
};
#endif
