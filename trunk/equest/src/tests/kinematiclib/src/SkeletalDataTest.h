/********************************************************************
	created:	2015/01/22
	created:	10:43
	filename: 	SkeletalDataTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST__SkeletalDataTest_H__
#define HEADER_GUARD_TEST__SkeletalDataTest_H__


#include <limits>
#include <cppunit/extensions/HelperMacros.h>
#include <kinematiclib/SkeletalData.h>

class SkeletalDataTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE(SkeletalDataTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();
public:
	SkeletalDataTest(void);
	virtual ~SkeletalDataTest(void);

public:
	void test1();
};
#endif
