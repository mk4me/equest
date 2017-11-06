/********************************************************************
	created:  2014/10/02	09:33:26
	filename: Macros.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__MACROS_H__
#define __HEADER_GUARD_THREADINGUTILS__MACROS_H__

// Makro aliasuj¹ce thread_local (TLS - thread local storage)
#if defined __GNUC__
#define thread_local __thread
#elif defined _MSC_VER
#define thread_local __declspec(thread)
#define FIX_VOID_RET_PACKAGED_TASK 1
#else
#error Provide alias for TLS (thread local storage) for current compiler!
#endif

#endif	// __HEADER_GUARD_THREADINGUTILS__MACROS_H__