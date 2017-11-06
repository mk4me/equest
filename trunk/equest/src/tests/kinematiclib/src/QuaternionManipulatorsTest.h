/********************************************************************
	created:	2015/01/22
	created:	10:40
	filename: 	QuaternionManipulatorsTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST__QuaternionManipulatorsTest_H__
#define HEADER_GUARD_TEST__QuaternionManipulatorsTest_H__

#include <limits>
#include <cppunit/extensions/HelperMacros.h>
#include <kinematiclib/QuaternionManipulators.h>

class QuaternionManipulatorsTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE(QuaternionManipulatorsTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();
public:
	QuaternionManipulatorsTest(void);
	virtual ~QuaternionManipulatorsTest(void);

public:
	void test1();
};
#endif
