#include "TestClientChannel.h"
#include <utils/Debug.h>

namespace timeline{

	TestClientChannel::TestClientChannel(double length) : timeline::IChannel(), length(length), time(0)
	{

	}

	TestClientChannel * TestClientChannel::clone() const
	{
		return new TestClientChannel(*this);
	}

	double TestClientChannel::getLength() const
	{
		return length;
	}

	void TestClientChannel::setTime(double time)
	{
		UTILS_ASSERT(( time >= 0 && time <= length), "Zly czas kanalu - poza jego dlugoscia");
		this->time = time;
	}

	TestClientChannel::TestClientChannel(const TestClientChannel & channel) : length(channel.length), time(channel.time)
	{

	}

}
