/********************************************************************
	created:  2010/07/02
	created:  2:7:2010   10:10
	filename: ObserverPatternTest.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__OBSERVERPATTERNTEST_H__
#define HEADER_GUARD__OBSERVERPATTERNTEST_H__

#include <stdexcept>
#include <cppunit/extensions/HelperMacros.h>

class ObserverPatternTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(ObserverPatternTest);
  CPPUNIT_TEST_EXCEPTION(testAttach, std::invalid_argument);
  CPPUNIT_TEST_EXCEPTION(testDetach, std::invalid_argument);
  CPPUNIT_TEST(testNotify);
  CPPUNIT_TEST_SUITE_END();

public:
  void testAttach();
  void testDetach();
  void testNotify();
};


#endif  // HEADER_GUARD__OBSERVERPATTERNTEST_H__
