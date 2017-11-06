/********************************************************************
	created:  2015/12/09
	filename: LogPolicy.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__LOGPOLICY_H__
#define __HEADER_GUARD_THREADINGUTILS__LOGPOLICY_H__

#include <string>
#include <iostream>

namespace threadingUtils
{
	class EmptyLogPolicy
	{
	public:

		EmptyLogPolicy() {}
		EmptyLogPolicy(const EmptyLogPolicy &) {}
		EmptyLogPolicy(EmptyLogPolicy &&) {}
		~EmptyLogPolicy() {}

		inline static void log(const std::string &) {}
	};

	class CoutLogPolicy
	{
	public:

		CoutLogPolicy() : idx(0) {}
		CoutLogPolicy(const CoutLogPolicy & Other) : idx(Other.idx) {}
		CoutLogPolicy(CoutLogPolicy && Other) : idx(Other.idx) {}
		~CoutLogPolicy() {}

		inline void log(const std::string & message)
		{
			std::cout << idx++ << ": " << message << std::endl;
		}

	private:

		std::size_t idx;
	};
}

#endif  // __HEADER_GUARD_THREADINGUTILS__LOGPOLICY_H__