/********************************************************************
	created:	2015/01/22
	created:	10:24
	filename: 	RotationConverterTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_kinematicUtilsTest__RotationConverterTest_H__
#define HEADER_GUARD_kinematicUtilsTest__RotationConverterTest_H__


#include <cppunit/extensions/HelperMacros.h>

class RotationConverterTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE(RotationConverterTest);
	CPPUNIT_TEST(testNormalizeAngle);
	CPPUNIT_TEST(testCompareAngle);
	CPPUNIT_TEST(testConversion);
	CPPUNIT_TEST(testIdentity);
	CPPUNIT_TEST(testDeg2Rad);
	CPPUNIT_TEST(testEulerQuatEuler);
	CPPUNIT_TEST(testSameDirection);
	CPPUNIT_TEST_SUITE_END();

public:
	void testNormalizeAngle();
	void testCompareAngle();
	void testConversion();
	void testIdentity();
	void testDeg2Rad();
	void testEulerQuatEuler();
	void testSameDirection();
};
#endif
