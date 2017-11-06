/********************************************************************
	created:  2010/07/01
	created:  1:7:2010   13:59
	filename: UtilsTest.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__MACROSTEST_H__
#define HEADER_GUARD__MACROSTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class UtilsTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(UtilsTest);
  CPPUNIT_TEST(testComaStringize);
  CPPUNIT_TEST(testClamp);
  CPPUNIT_TEST_SUITE_END();

public:
  void testComaStringize();
  void testClamp();
};


#endif  // HEADER_GUARD__MACROSTEST_H__
