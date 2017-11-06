/********************************************************************
    created:  2011/03/25
    created:  25:3:2011   12:10
    filename: TimelineTimeManagmentTest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINELIB__TIMELINETIMEMANAGMENTTEST_H__
#define HEADER_GUARD_TIMELINELIB__TIMELINETIMEMANAGMENTTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <timelinelib/Model.h>

class TestClientChannel : public timeline::IChannel
{
public:
    TestClientChannel(double length) : timeline::IChannel(), length(length), time(0)
    {

    }

    virtual timeline::IChannel * clone() const
    {
        return new TestClientChannel(*this);
    }

    virtual double getLength() const
    {
        return length;
    }

    virtual void setTime(double time)
    {
        UTILS_ASSERT(( time >= 0 && time <= length), "Zly czas kanalu - poza jego dlugoscia");
        this->time = time;
    }

private:
    TestClientChannel(const TestClientChannel & channel) : length(channel.length), time(channel.time)
    {

    }

private:
    double length;
    double time;

};

class TimelineTimeManagmentTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TimelineTimeManagmentTest);

    CPPUNIT_TEST(testRootScale);
    CPPUNIT_TEST(testRootOffset);

    CPPUNIT_TEST(testOneLevelScale);
    CPPUNIT_TEST(testOneLevelOffset);

    CPPUNIT_TEST(testMultilevelLevelScale);
    CPPUNIT_TEST(testMultiLevelOffset);

    CPPUNIT_TEST(testTimeSet);

    CPPUNIT_TEST_SUITE_END();

public:

    virtual void setUp();

    void testRootScale();

    void testRootOffset();
    void testOneLevelScale();
    void testOneLevelOffset();
    void testMultilevelLevelScale();

    void testMultiLevelOffset();
    void testTimeSet();

private:
    timeline::ModelPtr model;
};

#endif
