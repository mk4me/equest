#include <utils/Align.h>
#include "AlignTest.h"

using namespace utils;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( AlignTest );

void AlignTest::testAlign()
{
  CPPUNIT_ASSERT_EQUAL( 1, static_cast<int>(Align<1>::value) );
  CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(Align<2>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(Align<3>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(Align<4>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(Align<5>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(Align<6>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(Align<7>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(Align<8>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(Align<666>::value) );
}

void AlignTest::testAlignTo()
{
  CPPUNIT_ASSERT_EQUAL( 0, static_cast<int>(AlignTo<0,1>::value) );
  CPPUNIT_ASSERT_EQUAL( 1, static_cast<int>(AlignTo<1,1>::value) );
  CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(AlignTo<2,1>::value) );
  CPPUNIT_ASSERT_EQUAL( 3, static_cast<int>(AlignTo<3,1>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(AlignTo<4,1>::value) );

  CPPUNIT_ASSERT_EQUAL( 0, static_cast<int>(AlignTo<0,2>::value) );
  CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(AlignTo<1,2>::value) );
  CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>(AlignTo<2,2>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(AlignTo<3,2>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(AlignTo<4,2>::value) );

  CPPUNIT_ASSERT_EQUAL( 0, static_cast<int>(AlignTo<0,4>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(AlignTo<1,4>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(AlignTo<2,4>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(AlignTo<3,4>::value) );
  CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>(AlignTo<4,4>::value) );
  CPPUNIT_ASSERT_EQUAL( 8, static_cast<int>(AlignTo<5,4>::value) );

  CPPUNIT_ASSERT_EQUAL( 0, static_cast<int>(AlignTo<0,8>::value) );
  CPPUNIT_ASSERT_EQUAL( 8, static_cast<int>(AlignTo<1,8>::value) );
  CPPUNIT_ASSERT_EQUAL( 8, static_cast<int>(AlignTo<2,8>::value) );
  CPPUNIT_ASSERT_EQUAL( 8, static_cast<int>(AlignTo<3,8>::value) );
  CPPUNIT_ASSERT_EQUAL( 8, static_cast<int>(AlignTo<4,8>::value) );
  CPPUNIT_ASSERT_EQUAL( 8, static_cast<int>(AlignTo<5,8>::value) );
  CPPUNIT_ASSERT_EQUAL( 8, static_cast<int>(AlignTo<8,8>::value) );
  CPPUNIT_ASSERT_EQUAL(16, static_cast<int>(AlignTo<9,8>::value) );
}
