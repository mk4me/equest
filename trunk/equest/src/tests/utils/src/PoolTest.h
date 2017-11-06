/********************************************************************
	created:  2010/07/16
	created:  16:7:2010   17:26
	filename: PoolTest.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__POOLTEST_H__
#define HEADER_GUARD__POOLTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class PoolTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(PoolTest);
  CPPUNIT_TEST(testPrimitives);
  CPPUNIT_TEST(testDestructors);
  CPPUNIT_TEST_SUITE_END();

public:
  void testPrimitives();
  void testDestructors();

private:

  template <class T1>
  void testPool(const T1& data1);
  template <class T1, class T2>
  void testPool(const T1& data1, const T2& data2);
  template <class T1, class T2, class T3>
  void testPool(const T1& data1, const T2& data2, const T3& data3);
  template <class T1, int Size>
  void testPool(const T1 (&data1)[Size] );
};



#endif  // HEADER_GUARD__POOLTEST_H__
