/********************************************************************
	created:  2014/10/13	08:13:40
	filename: InterruptiblePackagedTask.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPACKAGEDTASK_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPACKAGEDTASK_H__

#include <threadingUtils/InterruptibleFuture.h>

namespace threadingUtils
{
	//! Struktura pomocnicza przy inicjonawniu przerywalnoœci zadania
	//! RAII
	//! \tparam InterruptiblePolicy Polityka przerywania w¹tku
	template<typename InterruptiblePolicy>
	struct InterruptibleTaskGuard
	{
		//! \param interruptPrivateData Obiekt przechowuj¹cy prywatne dane polityki przerywania
		InterruptibleTaskGuard(std::promise<void*> & interruptPrivateData) : ipd(nullptr)
		{
			try{
				InterruptiblePolicy::initializeContext(ipd);
				interruptPrivateData.set_value(ipd);
			}
			catch (...)
			{
				interruptPrivateData.set_exception(std::current_exception());
			}		
		}

		//! Destruktor
		~InterruptibleTaskGuard()
		{
			InterruptiblePolicy::deinitializeContext(ipd);
		}

		void * ipd;
	};

	//! Wzorzec klasy realizujacej obs³ugê zadania przerywalnego
	template<class, class>
	class InterruptiblePackagedTask;

	template<class InterruptiblePolicy,	class Ret, class... Args>
	class InterruptiblePackagedTask<InterruptiblePolicy, Ret(Args...)>
	{	// class that defines an asynchronous provider that returns the
		// result of a call to a function object

	private:

		using MyPackagedTask = std::packaged_task<Ret(Args...)>;

	private:

		MyPackagedTask packagedTask;
		std::promise<void*> interruptPrivateData;
		utils::shared_ptr<bool> interrupt;

	public:
		InterruptiblePackagedTask() : interrupt(utils::make_shared<bool>(false))
		{	// construct
		}

		template<class Fty>
		explicit InterruptiblePackagedTask(Fty&& Fnarg)
			: packagedTask(std::forward<Fty>(Fnarg)), interrupt(utils::make_shared<bool>(false))
		{	// construct from rvalue function object
		}

		InterruptiblePackagedTask(InterruptiblePackagedTask&& other) 
			: packagedTask(std::move(other.packagedTask)),
			interruptPrivateData(std::move(other.interruptPrivateData)),
			interrupt(std::move(other.interrupt))
		{	// construct from rvalue packaged_task object
		}

		InterruptiblePackagedTask& operator=(InterruptiblePackagedTask&& other) 
		{	// assign from rvalue packaged_task object
			packagedTask = std::move(other.packagedTask);
			interruptPrivateData = std::move(other.interruptPrivateData);
			interrupt = std::move(other.interrupt);
			return (*this);
		}

		template<class Fty,	class Alloc>
		explicit InterruptiblePackagedTask(std::allocator_arg_t, const Alloc& Al,
			Fty&& Fnarg) : packagedTask(Al, std::forward<Fty>(Fnarg)), interrupt(utils::make_shared<bool>(false))
		{	// construct from rvalue function object and allocator
		}

		~InterruptiblePackagedTask() 
		{	// destroy
		}

		void swap(InterruptiblePackagedTask& other) 
		{	// exchange with _Other
			std::swap(packagedTask, other.packagedTask);
			std::swap(interruptPrivateData, other.interruptPrivateData);
			std::swap(interrupt, other.interrupt);			
		}

		explicit operator bool() const 	// retained
		{	// return status
			return packagedTask();
		}

		bool valid() const
		{	// return status
			return packagedTask.valid();
		}

		InterruptibleFuture<Ret, InterruptiblePolicy> get_future()
		{	// return a future object that shares the associated
			// asynchronous state
			return InterruptibleFuture<Ret, InterruptiblePolicy>(packagedTask.get_future(), interruptPrivateData.get_future(), interrupt);
		}

		void operator()(Args... arguments)
		{	// call the function object
			InterruptibleTaskGuard<InterruptiblePolicy> taskGuard(interruptPrivateData);

			if (*interrupt == true){
				throw ThreadInterruptedException();
			}

			packagedTask(std::forward<Args>(arguments)...);
		}

		void make_ready_at_thread_exit(Args... arguments)
		{	// call the function object and block until thread exit
			packagedTask.make_ready_at_thread_exit(std::forward<Args>(arguments)...);
		}

		void reset()
		{	// reset to newly constructed state
			packagedTask.reset();
		}

	public:
		InterruptiblePackagedTask(const InterruptiblePackagedTask&) = delete;
		InterruptiblePackagedTask& operator=(const InterruptiblePackagedTask&) = delete;
	};
}

/*
template<class InterruptiblePolicy, class T>
inline void std::swap(threadingUtils::InterruptiblePackagedTask<InterruptiblePolicy, T>& _Left,
threadingUtils::InterruptiblePackagedTask<InterruptiblePolicy, T>& _Right) 
{	// exchange _Left and _Right
	_Left.swap(_Right);
}
*/

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEPACKAGEDTASK_H__