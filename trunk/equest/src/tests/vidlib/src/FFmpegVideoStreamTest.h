/********************************************************************
	created:  2011/01/27
	created:  27:1:2011   16:23
	filename: FFmpegVideoStreamTest.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__FFMPEGVIDEOSTREAMTEST_H__
#define HEADER_GUARD__FFMPEGVIDEOSTREAMTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <vidlib/FFmpegVideoStream.h>

class FFmpegVideoStreamTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(FFmpegVideoStreamTest);
    CPPUNIT_TEST(testOpen);
    CPPUNIT_TEST_EXCEPTION(testOpenFail, vidlib::VideoError);
    CPPUNIT_TEST_EXCEPTION(testNegativeTimeFail, vidlib::VideoError);
    CPPUNIT_TEST_EXCEPTION(testOverflowTimeFail, vidlib::VideoError);
    CPPUNIT_TEST_EXCEPTION(testUndefinedTimeFail, vidlib::VideoError);
    CPPUNIT_TEST(testDecode);
    CPPUNIT_TEST(testConvert);
    CPPUNIT_TEST(testClose);
    CPPUNIT_TEST(testBoundaries);
    CPPUNIT_TEST(testClone);
    CPPUNIT_TEST_SUITE_END();

public:
    
    void testBoundaries();
    void testOpen();
    void testClose();
    void testDecode();
    void testConvert();
    void testClone();

    void testOpenFail();
    void testNegativeTimeFail();
    void testOverflowTimeFail();
    void testUndefinedTimeFail();
};


#endif  // HEADER_GUARD__FFMPEGVIDEOSTREAMTEST_H__
