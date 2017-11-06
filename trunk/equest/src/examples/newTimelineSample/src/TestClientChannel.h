/********************************************************************
    created:  2011/04/06
    created:  6:4:2011   10:10
    filename: TestClientChannel.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TESTCLIENTCHANNEL_H__
#define HEADER_GUARD_TIMELINE__TESTCLIENTCHANNEL_H__

#include <timelinelib/IChannel.h>

namespace timeline{

	class TestClientChannel : public IChannel
	{
		
	private:
		double length;
		double time;
		
	public:
		TestClientChannel(double length);

		virtual TestClientChannel * clone() const;

		virtual double getLength() const;

		virtual void setTime(double time);

	private:
		TestClientChannel(const TestClientChannel & channel);
	};

}

#endif	//HEADER_GUARD_TIMELINE__TESTCLIENTCHANNEL_H__
