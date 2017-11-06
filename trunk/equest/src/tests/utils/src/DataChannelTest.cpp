#include <utils/DataChannel.h>
#include "DataChannelTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DataChannelTest );

typedef utils::RawUniformDataChannel<float, float> FDChannel;
typedef utils::RawGeneralDataChannel<float, float> FCChannel;

class DataChannelChangesCounter : public FCChannel::_MyObserverType
{
public:
    DataChannelChangesCounter() : counter(0) {}
    ~DataChannelChangesCounter() {}

    int getCounter() const { return counter; }

    virtual void update(const FCChannel::_MyRawChannelReaderType * channel) { counter++; }

private:
    int counter;
};

void DataChannelTest::testZeroTimeSample()
{
    FCChannel channel;

    channel.addPoint(0, 5);

    CPPUNIT_ASSERT(channel.value(0) == 5);
    CPPUNIT_ASSERT(channel[0u].second == 5);
    CPPUNIT_ASSERT(channel[0.0f].second == 5);

    FCChannel channelNext;
    channelNext.addPoint(5,5);
}

void DataChannelTest::testChannelStats()
{
    /*boost::shared_ptr<FUTDCChannel> channel(new FUTDCChannel());

    channel->addPoint(-3);
    channel->addPoint(-2);
    channel->addPoint(-1);
    channel->addPoint(0);
    channel->addPoint(1);
    channel->addPoint(2);
    channel->addPoint(3);

    utils::ChannelStats<FUTDCChannel::point_type, FUTDCChannel::time_type> stats(channel);

    CPPUNIT_ASSERT(stats.meanValue() == 0);
    CPPUNIT_ASSERT(stats.minValue() == -3);
    CPPUNIT_ASSERT(stats.maxValue() == 3);*/
}

void DataChannelTest::testTimeOverflow()
{
    FCChannel channel;

    channel.addPoint(0,0);
    channel.addPoint(1,12);

    channel.value(2);
}

void DataChannelTest::testTimeUnderflow()
{
    FCChannel channel;

    channel.addPoint(0,0);
    channel.addPoint(1,12);

    channel.value(-2);
}

void DataChannelTest::testContiniousChannel()
{
    FCChannel channel;

    channel.addPoint(0,0);
    channel.addPoint(1,12);

    CPPUNIT_ASSERT(channel.value(0.5) == 6);
}

void DataChannelTest::testDiscreteChannel()
{
    FDChannel channel;

    channel.addPoint(0, 5);

    CPPUNIT_ASSERT(channel.value(0.0f) == 5);
    CPPUNIT_ASSERT(channel[0u].second == 5);
    CPPUNIT_ASSERT(channel[0.0f].second == 5);

    channel.addPoint(3, 6);

    CPPUNIT_ASSERT(channel.value(3.0f) == 6);
    CPPUNIT_ASSERT(channel[1u].second == 6);
    CPPUNIT_ASSERT(channel[3.0f] == 6);

    channel.setTimeEpsilon(3);
    channel.setSmallerIfEqualPolicy(true);

    CPPUNIT_ASSERT(channel.getValue(1.5f) == 5);

    channel.setSmallerIfEqualPolicy(false);

    CPPUNIT_ASSERT(channel.getValue(1.5f) == 6);

    channel.setTimeEpsilon(1);

    channel.getValue(1.5f);
}

void DataChannelTest::testUniformChannel()
{
    FUTDCChannel channel;

    channel.addPoint(0, 1.0);

    channel.addPoint(2);
    channel.addPoint(7, 6.0);
}

void DataChannelTest::testObservableChannelTest()
{
    FCChannel channel;
    DataChannelChangesCounter counter;
    channel.attach(&counter);

    int i = 0;

    for( ; i < 10; i++){
        channel.addPoint(i,i*i);
    }

    CPPUNIT_ASSERT(counter.getCounter() == i);
}

void DataChannelTest::testTimeChannel()
{
    boost::shared_ptr<FUTDCChannel> channel(new FUTDCChannel());

    channel->addPoint(0);
    channel->addPoint(1);
    channel->addPoint(2);
    channel->addPoint(3);
    channel->addPoint(4);
    channel->addPoint(5);

    utils::ChannelTimer<FUTDCChannel::point_type, FUTDCChannel::time_type> timer(channel);

    timer.setTime(3 * channel->getSampleDuration());

    CPPUNIT_ASSERT(timer.getCurrentValue() == 3);

}

void DataChannelTest::testNamedChannel()
{

}

