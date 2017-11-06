/********************************************************************
	created:  2014/10/10	12:08:00
	filename: InterruptibleFuture.h
	author:	  Mateusz Janiak

	purpose:  Klasa realizuj¹ca przerywalny future. Poza klasycznym czekaniem
	i blokowaniem wykonania do czasu otrzymania wyniku mo¿na równie¿ przerwaæ
	w¹tek realizuj¹cy zadanie na któego wynik czekamy
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEFUTURE_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEFUTURE_H__

#include <system_error>
#include <threadingUtils/Future.h>

namespace threadingUtils
{
	template<class InterruptiblePolicy>
	class InterruptibleSharedFutureHelper;

	template<class InterruptiblePolicy>
	class InterruptibleFutureHelper
	{
		template<class>
		friend class InterruptibleSharedFutureHelper;

	protected:

		InterruptibleFutureHelper() 
		{

		}

		InterruptibleFutureHelper(std::future<void*> && interruptPrivateData,
			utils::shared_ptr<bool> interruptFlag)
			: interruptPrivateData(std::move(interruptPrivateData)),
			interruptFlag(interruptFlag)
		{

		}

		InterruptibleFutureHelper(InterruptibleFutureHelper && Other)
			: interruptPrivateData(std::move(Other.interruptPrivateData)),
			interruptFlag(std::move(Other.interruptFlag))
		{

		}

		InterruptibleFutureHelper(const InterruptibleFutureHelper & Other) = delete;

		~InterruptibleFutureHelper()
		{

		}

	public:

		bool interruptible() const 
		{
			return interruptFlag != nullptr && *interruptFlag == false;
		}

		void interrupt()
		{
			if (interruptible() == true){
				*interruptFlag = true;
				if (interruptPrivateData.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready){
					void * ipv = interruptPrivateData.get();
					InterruptiblePolicy::interrupt(ipv);
				}
			}
			else{
				throw std::system_error(std::make_error_code(std::future_errc::broken_promise));
			}
		}

		InterruptibleFutureHelper& operator=(InterruptibleFutureHelper&& Right) 
		{	// assign from rvalue future object
			interruptPrivateData = std::move(Right.interruptPrivateData);
			interruptFlag = std::move(Right.interruptFlag);

			return (*this);
		}

	private:
		//! Future dostarczajacy przywatnych danych polityki przerywaj¹cej zadanie
		std::future<void*> interruptPrivateData;
		//! Flaga informuj¹ca czy nast¹pi³o przerwanie czy nie
		utils::shared_ptr<bool> interruptFlag;
	};

	template<class InterruptiblePolicy>
	class InterruptibleSharedFutureHelper
	{
	protected:

		InterruptibleSharedFutureHelper()
		{

		}

		InterruptibleSharedFutureHelper(InterruptibleFutureHelper<InterruptiblePolicy> && futureHelper)
			: interruptPrivateData(std::move(futureHelper.interruptPrivateData)),
			interruptFlag(std::move(futureHelper.interruptFlag))
		{

		}

		InterruptibleSharedFutureHelper(InterruptibleSharedFutureHelper && Other)
			: interruptPrivateData(std::move(Other.interruptPrivateData)),
			interruptFlag(std::move(Other.interruptFlag))
		{

		}

		InterruptibleSharedFutureHelper(const InterruptibleFutureHelper<InterruptiblePolicy> & Other)
			: interruptPrivateData(Other.interruptPrivateData),
			interruptFlag(Other.interruptFlag)
		{

		}

		InterruptibleSharedFutureHelper(const InterruptibleSharedFutureHelper & Other) = delete;

		~InterruptibleSharedFutureHelper()
		{

		}

	public:

		bool interruptible() const 
		{
			return interruptFlag != nullptr && *interruptFlag == false;
		}

		void interrupt()
		{
			if (interruptible() == true){
				*interruptFlag = true;
				if (interruptPrivateData.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready){
					void * ipv = interruptPrivateData.get();
					InterruptiblePolicy::interrupt(ipv);
				}
			}
			else{
				throw std::logic_error(std::make_error_code(std::future_errc::broken_promise));
			}
		}

		InterruptibleSharedFutureHelper& operator=(InterruptibleSharedFutureHelper&& Right) 
		{	// assign from rvalue future object
			interruptPrivateData = std::move(Right.interruptPrivateData);
			interruptFlag = std::move(Right.interruptFlag);
			return (*this);
		}

	private:
		std::shared_future<void*> interruptPrivateData;
		utils::shared_ptr<bool> interruptFlag;
	};


	//! Forward declare
	//! \tparam T Typ obiektu zwracanego przez future
	//! \tparam InterruptiblePolicy Typ polityki przerywania w¹tku
	template<typename T, class InterruptiblePolicy>
	class InterruptibleSharedFuture;

	//! \tparam T Typ obiektu zwracanego przez future
	//! \tparam InterruptiblePolicy Typ polityki przerywania w¹tku
	//! Wzorzec reprezentuj¹cy przerywalny future
	template<typename T, class InterruptiblePolicy>
	class InterruptibleFuture : public InterruptibleFutureHelper<InterruptiblePolicy>
	{
	private:

		using MyFuture = std::future<T>;
		using MySharedFuture = InterruptibleSharedFuture<T, InterruptiblePolicy>;

	public:

		InterruptibleFuture()
		{

		}

		InterruptibleFuture(MyFuture && future, std::future<void*> && interruptPrivateData,
		utils::shared_ptr<bool> interruptFlag) : future(std::move(future)),
		InterruptibleFutureHelper<InterruptiblePolicy>(std::move(interruptPrivateData), interruptFlag)
		{

		}	

		InterruptibleFuture(InterruptibleFuture&& Other) 
			: future(std::move(Other.future)),
			InterruptibleFutureHelper<InterruptiblePolicy>(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Other)))
		{	// construct from rvalue future object

		}

		InterruptibleFuture& operator=(InterruptibleFuture&& Right) 
		{	// assign from rvalue future object
			future = std::move(Right.future);
			InterruptibleFutureHelper<InterruptiblePolicy>::operator=(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Right)));
			return (*this);
		}

		~InterruptibleFuture() 
		{	// destroy
		}

		InterruptibleFuture(const InterruptibleFuture&) = delete;
		InterruptibleFuture& operator=(const InterruptibleFuture&) = delete;

		bool valid() const
		{
			return future.valid();
		}

		void wait() const
		{	// wait for signal
			future.wait();
		}

		template<class Rep, class Per>
		std::future_status wait_for(const std::chrono::duration<Rep, Per> & relTime) const
		{	// wait for duration
			return future.wait_for(relTime);
		}

		template<class Clock, class Dur>
		std::future_status wait_until(const std::chrono::time_point<Clock, Dur>& absTime) const
		{	// wait until time point
			return future.wait_until(absTime);
		}

		T get()
		{	// block until ready then return the stored result or
			// throw the stored exception
			return future.get();
		}

		MySharedFuture share()
		{	// return state as shared_future
			return MySharedFuture(std::move(future), std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(*this)));
		}

	private:
		MyFuture future;
	};

	template<typename T, class InterruptiblePolicy>
	class InterruptibleFuture<T&, InterruptiblePolicy> : public InterruptibleFutureHelper<InterruptiblePolicy>
	{

	private:
		
		using MyFuture = std::future<T&>;
		using MySharedFuture = InterruptibleSharedFuture<T&, InterruptiblePolicy>;

	public:

		InterruptibleFuture()
		{

		}

		InterruptibleFuture(MyFuture && future, std::future<void*> && interruptPrivateData,
		utils::shared_ptr<bool> interruptFlag) : future(std::move(future)),
		InterruptibleFutureHelper<InterruptiblePolicy>(std::move(interruptPrivateData), interruptFlag)
		{

		}

		InterruptibleFuture(InterruptibleFuture&& Other) 
			: future(std::move(Other.future)),
			InterruptibleFutureHelper<InterruptiblePolicy>(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Other)))
			{	// construct from rvalue future object

			}

			InterruptibleFuture& operator=(InterruptibleFuture&& Right) 
			{	// assign from rvalue future object
				future = std::move(Right.future);
				using Base = InterruptibleFutureHelper<InterruptiblePolicy>;
				this->Base.operator=(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Right)));
				return (*this);
			}

			~InterruptibleFuture() 
			{	// destroy
			}

			InterruptibleFuture(const InterruptibleFuture&) = delete;
			InterruptibleFuture& operator=(const InterruptibleFuture&) = delete;

			bool valid() const
			{
				return future.valid();
			}

			void wait() const
			{	// wait for signal
				future.wait();
			}

			template<class Rep, class Per>
			std::future_status wait_for(const std::chrono::duration<Rep, Per>& relTime) const
			{	// wait for duration
				return future.wait_for(relTime);
			}

			template<class Clock, class Dur>
			std::future_status wait_until(const std::chrono::time_point<Clock, Dur>& absTime) const
			{	// wait until time point
				return future.wait_until(absTime);
			}

			T& get()
			{	// block until ready then return the stored result or
				// throw the stored exception
				return future.get();
			}

			MySharedFuture share()
			{	// return state as shared_future
				return MySharedFuture(std::move(future), std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(*this)));
			}

	private:
		MyFuture future;
	};

	template<class InterruptiblePolicy>
	class InterruptibleFuture<void, InterruptiblePolicy> : public InterruptibleFutureHelper<InterruptiblePolicy>
	{
	private:

		using MyFuture = std::future<void>;
		using MySharedFuture = InterruptibleSharedFuture<void, InterruptiblePolicy>;

	public:

		InterruptibleFuture() 
		{

		}

		InterruptibleFuture(MyFuture && future, std::future<void*> && interruptPrivateData,
		utils::shared_ptr<bool> interruptFlag) : future(std::move(future)),
		InterruptibleFutureHelper<InterruptiblePolicy>(std::move(interruptPrivateData), interruptFlag)
		{

		}

		InterruptibleFuture(InterruptibleFuture&& Other) 
			: future(std::move(Other.future)),
			InterruptibleFutureHelper<InterruptiblePolicy>(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Other)))
			{	// construct from rvalue future object

			}

			InterruptibleFuture& operator=(InterruptibleFuture&& Right) 
			{	// assign from rvalue future object
				future = std::move(Right.future);
				InterruptibleFutureHelper<InterruptiblePolicy>::operator=(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Right)));
				return (*this);
			}

			~InterruptibleFuture() 
			{	// destroy
			}

			InterruptibleFuture(const InterruptibleFuture&) = delete;
			InterruptibleFuture& operator=(const InterruptibleFuture&) = delete;

			bool valid() const
			{
				return future.valid();
			}

			void wait() const
			{	// wait for signal
				future.wait();
			}

			template<class Rep, class Per>
			std::future_status wait_for(const std::chrono::duration<Rep, Per>& relTime) const
			{	// wait for duration
				return future.wait_for(relTime);
			}

			template<class Clock, class Dur>
			std::future_status wait_until(const std::chrono::time_point<Clock, Dur>& absTime) const
			{	// wait until time point
				return future.wait_until(absTime);
			}

			void get()
			{	// block until ready then return the stored result or
				// throw the stored exception
				future.get();
			}

			MySharedFuture share()
			{	// return state as shared_future
				return MySharedFuture(std::move(future), std::move(*this));
			}

	private:
		MyFuture future;
	};

	template<typename T, class InterruptiblePolicy>
	class InterruptibleSharedFuture : public InterruptibleSharedFutureHelper<InterruptiblePolicy>
	{	// class that defines a copyable asynchronous return object
		// that holds a value
	private:

		using MyInterruptibleFuture = InterruptibleFuture<T, InterruptiblePolicy>;
		using MySharedFuture = std::shared_future<T>;
		using MyType = InterruptibleSharedFuture<T, InterruptiblePolicy>;

	public:
		InterruptibleSharedFuture() 
		{	// construct
		}

		InterruptibleSharedFuture(const InterruptibleSharedFuture& Other)
		: InterruptibleSharedFutureHelper<InterruptiblePolicy>(Other),
		sharedFuture(Other.sharedFuture)
		{	// construct from shared_future object
		}

		InterruptibleSharedFuture& operator=(const InterruptibleSharedFuture& Right)
		{	// assign from shared_future object
			sharedFuture = Right.sharedFuture;
			InterruptibleSharedFutureHelper<InterruptiblePolicy>::operator=(Right);
			return (*this);
		}

		InterruptibleSharedFuture(MyInterruptibleFuture && Other) 
			: sharedFuture(std::move(Other.future)),
			InterruptibleSharedFutureHelper<InterruptiblePolicy>(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Other)))
		{	// construct from rvalue future object
		}

		InterruptibleSharedFuture(InterruptibleSharedFuture&& Other) 
			: sharedFuture(std::move(Other.sharedFuture)),
			InterruptibleSharedFutureHelper<InterruptiblePolicy>(static_cast<InterruptibleSharedFutureHelper<InterruptiblePolicy>&>(Other))
		{	// construct from rvalue shared_future object
		}

		~InterruptibleSharedFuture() 
		{	// destroy
		}

		const T& get() const
		{	// block until ready then return the stored result or
			// throw the stored exception
			return sharedFuture.get();
		}

		void wait() const
		{	// wait for signal
			sharedFuture.wait();
		}

		template<class Rep, class Per>
		std::future_status wait_for(const std::chrono::duration<Rep, Per>& relTime) const
		{	// wait for duration
			return sharedFuture.wait_for(relTime);
		}

		template<class Clock, class Dur>
		std::future_status wait_until(const std::chrono::time_point<Clock, Dur>& absTime) const
		{	// wait until time point
			return sharedFuture.wait_until(absTime);
		}

	private:
		MySharedFuture sharedFuture;
	};

	template<typename T, class InterruptiblePolicy>
	class InterruptibleSharedFuture<T&, InterruptiblePolicy> : public InterruptibleSharedFutureHelper<InterruptiblePolicy>
	{	// class that defines a copyable asynchronous return object
		// that holds a value
	private:

		using MyInterruptibleFuture = InterruptibleFuture<T&, InterruptiblePolicy>;
		using MySharedFuture = std::shared_future<T&>;
		using MyType = InterruptibleSharedFuture<T&, InterruptiblePolicy>;

	public:
		InterruptibleSharedFuture() 
		{	// construct
		}

		InterruptibleSharedFuture(const InterruptibleSharedFuture& Other)
		: InterruptibleSharedFutureHelper<InterruptiblePolicy>(Other),
		sharedFuture(Other.sharedFuture)
		{	// construct from shared_future object
		}

		InterruptibleSharedFuture& operator=(const InterruptibleSharedFuture& Right)
		{	// assign from shared_future object
			sharedFuture = Right.sharedFuture;
			InterruptibleSharedFutureHelper<InterruptiblePolicy>::operator=(Right);
			return (*this);
		}

		InterruptibleSharedFuture(MyInterruptibleFuture && Other) 
			: sharedFuture(std::move(Other.future)),
			InterruptibleSharedFutureHelper<InterruptiblePolicy>(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Other)))
		{	// construct from rvalue future object
		}

		InterruptibleSharedFuture(InterruptibleSharedFuture&& Other) 
			: sharedFuture(std::move(Other.sharedFuture)),
			InterruptibleSharedFutureHelper<InterruptiblePolicy>(static_cast<InterruptibleSharedFutureHelper<InterruptiblePolicy>&>(Other))
		{	// construct from rvalue shared_future object
		}

		~InterruptibleSharedFuture() 
		{	// destroy
		}

		const T& get() const
		{	// block until ready then return the stored result or
			// throw the stored exception
			return sharedFuture.get();
		}

		void wait() const
		{	// wait for signal
			sharedFuture.wait();
		}

		template<class Rep, class Per>
		std::future_status wait_for(const std::chrono::duration<Rep, Per>& relTime) const
		{	// wait for duration
			return sharedFuture.wait_for(relTime);
		}

		template<class Clock, class Dur>
		std::future_status wait_until(const std::chrono::time_point<Clock, Dur>& absTime) const
		{	// wait until time point
			return sharedFuture.wait_until(absTime);
		}

	private:
		MySharedFuture sharedFuture;
	};

	template<class InterruptiblePolicy>
	class InterruptibleSharedFuture<void, InterruptiblePolicy> : public InterruptibleSharedFutureHelper<InterruptiblePolicy>
	{	// class that defines a copyable asynchronous return object
		// that holds a value
	private:

		using MyInterruptibleFuture = InterruptibleFuture<void, InterruptiblePolicy>;
		using MySharedFuture = std::shared_future<void>;
		using MyType = InterruptibleSharedFuture<void, InterruptiblePolicy>;

	public:
		InterruptibleSharedFuture() 
		{	// construct
		}

		InterruptibleSharedFuture(const InterruptibleSharedFuture& Other)
		: InterruptibleSharedFutureHelper<InterruptiblePolicy>(Other),
		sharedFuture(Other.sharedFuture)
		{	// construct from shared_future object
		}

		InterruptibleSharedFuture& operator=(const InterruptibleSharedFuture& Right)
		{	// assign from shared_future object
			sharedFuture = Right.sharedFuture;
			InterruptibleSharedFutureHelper<InterruptiblePolicy>::operator=(Right);
			return (*this);
		}

		InterruptibleSharedFuture(MyInterruptibleFuture && Other) 
			: sharedFuture(std::move(Other.future)),
			InterruptibleSharedFutureHelper<InterruptiblePolicy>(std::move(static_cast<InterruptibleFutureHelper<InterruptiblePolicy>&>(Other)))
		{	// construct from rvalue future object
		}

		InterruptibleSharedFuture(InterruptibleSharedFuture&& Other) 
			: sharedFuture(std::move(Other.sharedFuture)),
			InterruptibleSharedFutureHelper<InterruptiblePolicy>(static_cast<InterruptibleSharedFutureHelper<InterruptiblePolicy>&>(Other))
		{	// construct from rvalue shared_future object
		}

		~InterruptibleSharedFuture() 
		{	// destroy
		}

		void get() const
		{	// block until ready then return the stored result or
			// throw the stored exception
			sharedFuture.get();
		}

		void wait() const
		{	// wait for signal
			sharedFuture.wait();
		}

		template<class Rep, class Per>
		std::future_status wait_for(const std::chrono::duration<Rep, Per>& relTime) const
		{	// wait for duration
			return sharedFuture.wait_for(relTime);
		}

		template<class Clock, class Dur>
		std::future_status wait_until(const std::chrono::time_point<Clock, Dur>& absTime) const
		{	// wait until time point
			return sharedFuture.wait_until(absTime);
		}

	private:
		MySharedFuture sharedFuture;
	};

	/*
	// DEFINITION OF future<void>::share()
	inline shared_future<void> future<void>::share()
	{	// return state as shared_future
		return (shared_future<void>(std::move(*this)));
	}
	*/
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEFUTURE_H__
