#include <utils/Enum.h>
#include "EnumTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( EnumTest );

// definicja enuma
#define COLOR_ENUM_DEF \
  ColorRed = 0x01,\
  ColorGreen = 0x02,\
  ColorBlue = 0x04,\
  ColorBlack = 0x00,\
  ColorWhite = ColorBlue | ColorRed | ColorGreen,\
  ColorYellow = ColorRed | ColorGreen,\
  ColorPink = ColorRed | ColorBlue,\
  ColorTeal = ColorBlue | ColorGreen

// definiujemy enum
enum Color {
  COLOR_ENUM_DEF
};

// tworzymy dodatkowe informacje
DECLARE_ENUM_ENCHANCEMENT(Color, COLOR_ENUM_DEF);

// definicja typu
typedef utils::Enum<Color> EnumColor;

void EnumTest::testLength()
{
  CPPUNIT_ASSERT_EQUAL(EnumColor::size(), 8u);
}

void EnumTest::testNames()
{
  CPPUNIT_ASSERT_EQUAL(EnumColor::getName(ColorRed), std::string("ColorRed"));
  CPPUNIT_ASSERT_EQUAL(EnumColor::getName(ColorGreen), std::string("ColorGreen"));
  CPPUNIT_ASSERT_EQUAL(EnumColor::getName(ColorBlue), std::string("ColorBlue"));
  CPPUNIT_ASSERT_EQUAL(EnumColor::getName(ColorBlack), std::string("ColorBlack"));
  CPPUNIT_ASSERT_EQUAL(EnumColor::getName(ColorWhite), std::string("ColorWhite"));
  CPPUNIT_ASSERT_EQUAL(EnumColor::getName(ColorYellow), std::string("ColorYellow"));
  CPPUNIT_ASSERT_EQUAL(EnumColor::getName(ColorPink), std::string("ColorPink"));
  CPPUNIT_ASSERT_EQUAL(EnumColor::getName(ColorTeal), std::string("ColorTeal"));
}

void EnumTest::testParsing()
{
  CPPUNIT_ASSERT_EQUAL(EnumColor::parse("ColorRed"), ColorRed);
  CPPUNIT_ASSERT_EQUAL(EnumColor::parse("ColorGreen"), ColorGreen);
  CPPUNIT_ASSERT_EQUAL(EnumColor::parse("ColorBlue"), ColorBlue);
  CPPUNIT_ASSERT_EQUAL(EnumColor::parse("ColorBlack"), ColorBlack);
  CPPUNIT_ASSERT_EQUAL(EnumColor::parse("ColorWhite"), ColorWhite);
  CPPUNIT_ASSERT_EQUAL(EnumColor::parse("ColorYellow"), ColorYellow);
  CPPUNIT_ASSERT_EQUAL(EnumColor::parse("ColorPink"), ColorPink);
  CPPUNIT_ASSERT_EQUAL(EnumColor::parse("ColorTeal"), ColorTeal);
}

void EnumTest::testParsingFail()
{
  Color value = EnumColor::parse("ColorFAIL");
}

void EnumTest::testNameFail()
{
  std::string name = EnumColor::getName(static_cast<Color>(0x1000));
}
