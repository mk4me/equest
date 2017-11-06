/********************************************************************
	created:	2014/11/07
	created:	12:28
	filename: 	Exceptions.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_utils__Exceptions_H__
#define HEADER_GUARD_utils__Exceptions_H__

#include <loglib/ILog.h>
#include <loglib/Logger.h>
#include <type_traits>
#include <typeinfo>
#include <future>


namespace loglib {

	class Exception
	{
		
	private:

		template<typename MessageType>
		inline static void log(const char * typeName, const MessageType & message)
		{
			static auto el = Logger::getLog();// ::getApplication()->log()->subLog("exception");
			LOG_DEBUG(el, "First chance exception [type -> " << typeName << "]: " << message);
		}
	private:

		//! klasa dodaje logowanie do standardowych wyjątków z stl-a
		template <typename BaseException, bool>
		class WrapperImpl : public BaseException
		{
			static_assert((std::is_pod<BaseException>::value == false), "Base exception type should be compound type");
		protected:
			template<class... Args>
			WrapperImpl(Args &&... arguments)
				: BaseException(std::forward<Args>(arguments)...)
			{

			}

			//! destruktor wykonuje faktyczne logowanie
			virtual ~WrapperImpl()
			{
				log(typeid(BaseException).name(), BaseException::what());
			}
		};

		//! klasa dodaje logowanie do standardowych wyjątków z stl-a
		template <typename BaseException>
		class WrapperImpl<BaseException, true>
		{
			static_assert((std::is_pod<BaseException>::value == true), "Base exception type should be POD type");
		protected:
			WrapperImpl(BaseException value)
				: exceptionValue(value)
			{

			}

			explicit operator BaseException() const { return exceptionValue; }

			//! destruktor wykonuje faktyczne logowanie
			virtual ~WrapperImpl()
			{
				log(typeid(BaseException).name(), exceptionValue);				
			}

		private:

			BaseException exceptionValue;
		};

	public:

		//! klasa dodaje logowanie do standardowych wyjątków z stl-a
		template <typename BaseException>
		class Wrapper : public WrapperImpl < BaseException, std::is_pod<BaseException>::value >
		{
		public:
			template<class... Args>
			Wrapper(Args &&... arguments)
				: WrapperImpl < BaseException, std::is_pod<BaseException>::value >(std::forward<Args>(arguments)...)
			{

			}

			//! destruktor wykonuje faktyczne logowanie
			virtual ~Wrapper()
			{

			}
		};

	public:

		template<typename ExceptionType, class... Args>
		static inline void Throw(Args &&... arguments)
		{
			ExceptionType e(std::forward<Args>(arguments)...);
			log(typeid(ExceptionType).name(), e.what());
			throw e;
		}

		template<typename T>
		static inline void Throw(const T & val)
		{
			log(typeid(T).name(), val);			
			throw val;
		}
	};	

	// rozszerzone wyjątki z stl-a, użycie: throw core::runtime_error("whatever...");
	
	using bad_cast = Exception::Wrapper<std::bad_cast>;
	using bad_alloc = Exception::Wrapper<std::bad_alloc>;
	using bad_array_new_length = Exception::Wrapper<std::bad_array_new_length>;
	using bad_exception = Exception::Wrapper<std::bad_exception>;
	using bad_function_call = Exception::Wrapper<std::bad_function_call>;
	using bad_typeid = Exception::Wrapper<std::invalid_argument>;
	using bad_weak_ptr = Exception::Wrapper<std::invalid_argument>;

	using logic_error = Exception::Wrapper<std::logic_error>;
	using out_of_range = Exception::Wrapper<std::out_of_range>;
	using domain_error = Exception::Wrapper<std::domain_error>;
	using future_error = Exception::Wrapper<std::future_error>;
	using invalid_argument = Exception::Wrapper<std::invalid_argument>;
	using length_error = Exception::Wrapper<std::length_error>;

	using runtime_error = Exception::Wrapper<std::runtime_error>;
	using overflow_error = Exception::Wrapper<std::overflow_error>;
	using range_error = Exception::Wrapper<std::range_error>;
	using system_error = Exception::Wrapper<std::system_error>;
	using underflow_error = Exception::Wrapper<std::underflow_error>;

}

#endif // HEADER_GUARD 
