/********************************************************************
	created:	2015/01/22
	created:	10:41
	filename: 	SkeletonTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST__SkeletonTest_H__
#define HEADER_GUARD_TEST__SkeletonTest_H__

#include <limits>
#include <cppunit/extensions/HelperMacros.h>
#include <kinematiclib/Skeleton.h>

class SkeletonTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE(SkeletonTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();
public:
	SkeletonTest(void);
	virtual ~SkeletonTest(void);

public:
	void test1();
};
#endif
