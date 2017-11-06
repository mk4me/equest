/********************************************************************
	created:  2014/11/26	11:02:16
	filename: TestObject.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___TESTOBJECT_H__
#define __HEADER_GUARD___TESTOBJECT_H__

#include <atomic>

class TestObject
{
public:

	TestObject() : val_(0) {}
	~TestObject() {}

	int value() const { return val_; }
	void inc() { ++val_; }
	void reset() { val_ = 0; }

private:

	std::atomic<int> val_;

};

#endif	// __HEADER_GUARD___TESTOBJECT_H__