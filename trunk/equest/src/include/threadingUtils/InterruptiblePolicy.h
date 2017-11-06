/********************************************************************
	created:  2014/10/07	10:52:53
	filename: InterruptiblePolicy.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPOLICY_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPOLICY_H__

#include <mutex>
#include <condition_variable>
#include <exception>
#include <chrono>
#include <threadingUtils/Future.h>
#include <threadingUtils/InterruptFlag.h>
#include <utils/SmartPtr.h>

namespace threadingUtils
{
	//! Domyslny timeout
	static const std::chrono::milliseconds DefaultTimeout = std::chrono::milliseconds(1);

	//! Wyjątek rzucany przy przerywaniu wątku
	class ThreadInterruptedException : public std::runtime_error
	{
	public:
		ThreadInterruptedException() : std::runtime_error(std::string()) {}
		explicit ThreadInterruptedException(const char * const & e) : std::runtime_error(e) {}
		//ThreadInterruptedException(const char * const & e, const int s) : std::runtime_error(e, s) {}
		ThreadInterruptedException(const ThreadInterruptedException& e) : std::runtime_error(e) {}
		ThreadInterruptedException& operator=(const ThreadInterruptedException& e) { std::exception::operator=(e); return *this; }
		virtual ~ThreadInterruptedException() {}
	};

	//! \tparam InterruptiblePolicy Polityka przerywalności wątku
	//! RAII używane do inicjalizacji wątku wspierającego przerywanie
	template<typename InterruptiblePolicy>
	struct InterruptibleThreadGuard
	{
	public:
		InterruptibleThreadGuard(InterruptiblePolicy & interrupt) : interrupt(interrupt)
		{
			interrupt.initializeThread();
		}

		~InterruptibleThreadGuard()
		{
			interrupt.deinitializeThread();
		}

	private:
		InterruptiblePolicy & interrupt;
	};

	//! Polityka bez przerywalności
	class NoInterruptiblePolicy
	{
	public:

		NoInterruptiblePolicy() = default;
		~NoInterruptiblePolicy() = default;

		void interrupt() {}
		static void interrupt(void*& privateData) {}
		bool interuptible() const { return false; }

		static void interruptionPoint() {}
		static void resetInterruption() {}

		void initializeThread() {}
		void deinitializeThread() {}

		static void initializeContext(void*& privateData) {}
		static void deinitializeContext(void*& privateData) {}
		
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock)
		{
			wait(cv, lock, DefaultTimeout);
		}

		template<class Rep, class Period>
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			cv.wait(lock);
		}

		template<typename Predicate>
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			Predicate pred)
		{
			wait(cv, lock, pred, DefaultTimeout);
		}

		template<typename Predicate, class Rep,	class Period>
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			Predicate pred,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			cv.wait(lock, pred);
		}

		template<typename Lockable>
		static void wait(std::condition_variable_any & cva,
			Lockable & lock)
		{
			wait(cva, lock, DefaultTimeout);
		}

		template<typename Lockable, class Rep, class Period>
		static void wait(std::condition_variable_any & cva,
			Lockable & lock,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			std::unique_lock<Lockable> l(lock);
			cva.wait(lock);
		}

		template<typename Lockable, typename Predicate>
		static void wait(std::condition_variable_any & cva,
			Lockable & lock,
			Predicate pred)
		{
			wait(cva, lock, pred, DefaultTimeout);
		}

		template<typename Lockable, typename Predicate, class Rep, class Period>
		static void wait(std::condition_variable_any & cva,
			Lockable & lock,
			Predicate pred,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			std::unique_lock<Lockable> l(lock);
			cva.wait(lock, pred);
		}

		template<typename T>
		static void wait(std::future<T> & f)
		{
			wait(f, DefaultTimeout);
		}

		template<typename T, class Rep,	class Period>
		static void wait(std::future<T> & f, 
			const std::chrono::duration<Rep, Period> & timeout)
		{
			f.wait();
		}
	};

	class InterrupltiblePolicy
	{
	private:
		utils::shared_ptr<bool> interuptible_;
		InterruptFlag flag;	

	public:

		InterrupltiblePolicy()
		{
		
		}

		~InterrupltiblePolicy()
		{

		}

		InterrupltiblePolicy(const InterrupltiblePolicy & Other) = delete;

		void interrupt()
		{
			flag.set();
		}

		static void interrupt(void*& privateData)
		{
			if (privateData != nullptr){
				InterruptFlag * iFlag = static_cast<InterruptFlag*>(privateData);
				iFlag->set();
			}
			else{
				throw std::runtime_error("Uninitialized private data for interruption");
			}
		}

		static void resetInterruption()
		{
			InterruptFlag::threadReset();
		}

		static void interruptionPoint()
		{
			if (InterruptFlag::threadIsSet() == true){
				throw ThreadInterruptedException();
			}
		}

		void initializeThread() { InterruptFlag::threadInterruptFlag() = &flag; interuptible_ = utils::make_shared<bool>(true); }
		void deinitializeThread() { interuptible_.reset(); InterruptFlag::threadInterruptFlag() = nullptr; }
		bool interruptible() const { return interuptible_ != nullptr && *interuptible_ == true; }

		static void initializeContext(void*& privateData) { privateData = InterruptFlag::threadInterruptFlag(); }
		static void deinitializeContext(void*& privateData) { privateData = nullptr; }
		static bool interruptible(void*& privateData)
		{
			bool ret = true;
			if (privateData != nullptr){
				InterruptFlag * iFlag = static_cast<InterruptFlag*>(privateData);
				ret = iFlag->isSet();
			}
			
			return !ret;
		}
		
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock)
		{
			wait(cv, lock, DefaultTimeout);			
		}

		template<class Rep,	class Period>
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			interruptionPoint();
			InterruptFlag::threadInterruptFlag()->setConditionVariable(cv);
			InterruptFlag::ClearConditionVariableOnDestruct guard;
			do{
				interruptionPoint();
			} while (cv.wait_for(lock, timeout) == std::cv_status::timeout);
		}

		template<class Rep, class Period, class Rep2, class Period2>
		static void wait_for(std::condition_variable & cv,
						 std::unique_lock<std::mutex> & lock,
						 const std::chrono::duration<Rep, Period> & timeout,
						 const std::chrono::duration<Rep2, Period2> & checkTime = DefaultTimeout)
		{
			auto start = std::chrono::system_clock::now();
			interruptionPoint();
			InterruptFlag::threadInterruptFlag()->setConditionVariable(cv);
			InterruptFlag::ClearConditionVariableOnDestruct guard;
			do {
				interruptionPoint();
				auto end = std::chrono::system_clock::now();
				std::chrono::duration<double> diff = end - start;
				if (diff >= timeout) {
					return;
				}
			} while (cv.wait_for(lock, checkTime) == std::cv_status::timeout);
		}



		template<typename Predicate>
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			Predicate pred)
		{
			wait(cv, lock, pred, DefaultTimeout);
		}

		template<typename Predicate, class Rep, class Period>
		static void wait(std::condition_variable & cv,
			std::unique_lock<std::mutex> & lock,
			Predicate pred,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			interruptionPoint();
			InterruptFlag::threadInterruptFlag()->setConditionVariable(cv);
			InterruptFlag::ClearConditionVariableOnDestruct guard;
			do{
				interruptionPoint();
			} while (cv.wait_for(lock, timeout, pred) == false);
		}

		//! Czekanie przerywalne na wďż˝asnym obiekcie synchronizujďż˝cym		
		template<typename Lockable>
		static void wait(std::condition_variable_any& cv, Lockable& lk)
		{
			wait(cv, lk, DefaultTimeout);
		}

		template<typename Lockable, class Rep, class Period>
		static void wait(std::condition_variable_any& cv, Lockable& lk,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			interruptionPoint();
			InterruptFlag::CustomLock<Lockable> cl(InterruptFlag::threadInterruptFlag(), cv, lk);
			do{
				interruptionPoint();
			} while (cv.wait_for(cl, timeout) == std::cv_status::timeout);
		}

		//! Czekanie przerywalne z predykatem na wďż˝asnym obiekcie synchornizujďż˝cym
		template<typename Lockable, typename Predicate>
		static void wait(std::condition_variable_any& cv, Lockable& lk, Predicate pred)
		{
			wait(cv, lk, pred, DefaultTimeout);
		}

		template<typename Lockable, typename Predicate, class Rep, class Period>
		static void wait(std::condition_variable_any & cv,
			Lockable & lk,
			Predicate pred,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			interruptionPoint();
			InterruptFlag::CustomLock<Lockable> cl(InterruptFlag::threadInterruptFlag(), cv, lk);
			do{
				interruptionPoint();
			} while (cv.wait_for(cl, timeout, pred) == false);
		}

		template<typename T>
		static void wait(std::future<T> & f)
		{
			wait(f, DefaultTimeout);
		}

		template<typename T, class Rep, class Period>
		static void wait(std::future<T> & f,
			const std::chrono::duration<Rep, Period> & timeout)
		{
			do{
				interruptionPoint();
			} while (f.wait_for(timeout) == std::future_status::timeout);
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPOLICY_H__
