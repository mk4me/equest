/********************************************************************
    created:  2011/07/26
    created:  26:7:2011   14:00
    filename: DataChannelTest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TEST__DATACHANNELTEST_H__
#define HEADER_GUARD_TEST__DATACHANNELTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class DataChannelTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DataChannelTest);
    CPPUNIT_TEST_EXCEPTION(testZeroTimeSample, std::runtime_error);
    CPPUNIT_TEST(testChannelStats);
    CPPUNIT_TEST_EXCEPTION(testTimeUnderflow, std::runtime_error);
    CPPUNIT_TEST_EXCEPTION(testTimeOverflow, std::runtime_error);
    CPPUNIT_TEST(testContiniousChannel);
    CPPUNIT_TEST_EXCEPTION(testDiscreteChannel, std::runtime_error);
    CPPUNIT_TEST_EXCEPTION(testUniformChannel, std::runtime_error);
    CPPUNIT_TEST(testObservableChannelTest);
    CPPUNIT_TEST(testTimeChannel);
    CPPUNIT_TEST(testNamedChannel);
    CPPUNIT_TEST_SUITE_END();

public:
    void testZeroTimeSample();
    void testChannelStats();
    void testTimeUnderflow();
    void testTimeOverflow();
    void testContiniousChannel();
    void testDiscreteChannel();
    void testUniformChannel();
    void testObservableChannelTest();
    void testTimeChannel();
    void testNamedChannel();
};

#endif  //  HEADER_GUARD_TEST__DATACHANNELTEST_H__
