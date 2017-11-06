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

class NetworkUtilsTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(NetworkUtilsTest);
  CPPUNIT_TEST(testOpen);
  CPPUNIT_TEST(testCrypto);
  CPPUNIT_TEST_SUITE_END();

public:
    void testOpen();
    void testCrypto();
};


#endif  // HEADER_GUARD__MACROSTEST_H__
