/********************************************************************
	created:  2010/07/01
	created:  1:7:2010   12:49
	filename: EnumTest.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__ENUMTEST_H__
#define HEADER_GUARD__ENUMTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>


class EnumTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(EnumTest);
  CPPUNIT_TEST(testLength);
  CPPUNIT_TEST(testNames);
  CPPUNIT_TEST(testParsing);
  CPPUNIT_TEST_EXCEPTION(testParsingFail, std::invalid_argument);
  CPPUNIT_TEST_EXCEPTION(testNameFail, std::invalid_argument);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void testLength();
  void testNames();
  void testParsing();

  void testParsingFail();
  void testNameFail();
};


#endif  // HEADER_GUARD__ENUMTEST_H__
