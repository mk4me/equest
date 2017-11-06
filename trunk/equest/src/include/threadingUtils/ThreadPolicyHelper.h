/********************************************************************
	created:  2014/10/07	16:16:00
	filename: ThreadPolicyHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__THREADPOLICYHELPER_H__
#define __HEADER_GUARD_THREADINGUTILS__THREADPOLICYHELPER_H__

#include <type_traits>

namespace threadingUtils
{
	template <class T> struct is_thread_policy : public std::false_type {};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__THREADPOLICYHELPER_H__