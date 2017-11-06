/********************************************************************
	created:  2014/10/09	14:18:19
	filename: ExceptionHandlePolicy.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__EXCEPTIONHANDLEPOLICY_H__
#define __HEADER_GUARD_THREADINGUTILS__EXCEPTIONHANDLEPOLICY_H__

#include <exception>
#include <type_traits>

namespace threadingUtils
{
	//! Polityka surowego wywo³ania
	struct RethrowExceptionHandlePolicy
	{
		//! \tparam F Typ metody jak¹ wo³amy
		//! \tparam Args Typy argumentów z jakimi wo³amy metodê
		//! \param f Wo³ana metoda
		//! \param arguments Argumenty metody
		template<typename T>
		static inline void handle(const T & e)
		{
			throw e;
		}

		static inline void handle(std::exception_ptr eptr)
		{
			std::rethrow_exception(eptr);
		}
	};

	//! Polityka surowego wywo³ania
	struct ConsumeExceptionHandlePolicy
	{
		//! \tparam F Typ metody jak¹ wo³amy
		//! \tparam Args Typy argumentów z jakimi wo³amy metodê
		//! \param f Wo³ana metoda
		//! \param arguments Argumenty metody
		template<typename T>
		static inline void handle(const T & e)
		{
			
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__EXCEPTIONHANDLEPOLICY_H__