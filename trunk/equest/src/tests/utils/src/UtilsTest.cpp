#include <string>
#include <utils/Macros.h>
#include <utils/Utils.h>
#include "UtilsTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( UtilsTest );

void UtilsTest::testComaStringize()
{
#ifdef __GNUC__
  // nie widzi spacji przed przecinkami
  std::string str = "Lorem ipsum dolor sit amet,consectetur adipisicing elit,sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
#else
  std::string str = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
#endif
  std::string stringized = COMA_AWARE_STRINGIZE(Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.);
  CPPUNIT_ASSERT_EQUAL(str, stringized);
}

void UtilsTest::testClamp()
{
  // typowe przypadki
  CPPUNIT_ASSERT_EQUAL( 0.5, utils::clamp(0.5, 0.0, 1.0 ) );
  CPPUNIT_ASSERT_EQUAL( 0.0, utils::clamp(-0.5, 0.0, 1.0 ) );
  CPPUNIT_ASSERT_EQUAL( 1.0, utils::clamp(1.5, 0.0, 1.0 ) );
  CPPUNIT_ASSERT_EQUAL( 0.0, utils::clamp(0.0, 0.0, 1.0 ) );
  CPPUNIT_ASSERT_EQUAL( 1.0, utils::clamp(1.0, 0.0, 1.0 ) );

  // na odwrót
  CPPUNIT_ASSERT_EQUAL( 1.0, utils::clamp(0.5, 1.0, 0.0 ) );
  CPPUNIT_ASSERT_EQUAL( 0.0, utils::clamp(1.5, 1.0, 0.0 ) );
}
