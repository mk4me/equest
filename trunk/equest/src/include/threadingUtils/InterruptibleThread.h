/********************************************************************
	created:  2014/10/07	12:55:02
	filename: InterruptibleThread.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREAD_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREAD_H__

#include <utils/Utils.h>
#include <threadingUtils/FunctionWrapper.h>
#include <threadingUtils/InterruptiblePolicy.h>
#include <threadingUtils/InterruptHandlingPolicy.h>
#include <threadingUtils/InterruptibleFuture.h>
#include <threadingUtils/InterruptiblePackagedTask.h>


namespace threadingUtils
{
	//! \tparam RunnableThread Polityka wątku z metodą uruchamiania zadań run
	//! \tparam InterruptHandlingPolicy Polityka obsługi przerwania w wątku
	//! \tparam InteruptiblePolicy Polityka realizująca przerywanie wątku
	template<typename RunnableThread, typename ExceptionHandlePolicy = ConsumeExceptionHandlePolicy, typename InterruptHandlingPolicy = NoInterruptHandlingPolicy, typename InterruptiblePolicyT = NoInterruptiblePolicy>
	class InterrubtibleThread : private ExceptionHandlePolicy, private InterruptHandlingPolicy
	{
	public:
		//! Typ polityki przerywania wątku
		using InterruptiblePolicy = InterruptiblePolicyT;

	private:
		//! Struktura zawierająca prywatny, współdzielony stan wątku
		struct SharedState
		{
			//! Obiekt polityki przerywania
			InterruptiblePolicy interruptible;
		};

	public:

		//! Konstruktor
		InterrubtibleThread(RunnableThread && innerThread = RunnableThread(),
			const ExceptionHandlePolicy & ehp = ExceptionHandlePolicy(),
			const InterruptHandlingPolicy & ihp = InterruptHandlingPolicy())
			: ExceptionHandlePolicy(ehp), InterruptHandlingPolicy(ihp),
			thread(std::move(innerThread)) {}

		//! Konstruktor
		/*template<class... Args>
		InterrubtibleThread(const ExceptionHandlePolicy & ehp = ExceptionHandlePolicy(),
			const InterruptHandlingPolicy & ihp = InterruptHandlingPolicy(),
			Args &&... arguments)
			: ExceptionHandlePolicy(ehp), InterruptHandlingPolicy(ihp), thread(std::forward<Args>(arguments)...) {}
			*/

		//! \param Other Przenoszony wątek
		InterrubtibleThread(InterrubtibleThread&& Other)
			: ExceptionHandlePolicy(std::move(Other)),
			InterruptHandlingPolicy(std::move(Other)),
			sharedState(std::move(Other.sharedState)),
			thread(std::move(Other.thread)) {}

		//! Konstruktor kopiujący
		InterrubtibleThread(const InterrubtibleThread&) = delete;
		//! Destruktor
		~InterrubtibleThread() {}

		//! Operator przypisania
		//! \param Other przenoszony wątek
		InterrubtibleThread& operator=(InterrubtibleThread&& Other)
		{
			ExceptionHandlePolicy::operator=(std::move(Other));
			InterruptHandlingPolicy::operator=(std::move(Other));				
			sharedState = std::move(Other.sharedState);
			thread = std::move(Other.thread);
			return *this;
		}
		//! Operator przypisania		
		InterrubtibleThread& operator=(const InterrubtibleThread&) = delete;

		//! \tparam F Typ funkcji jaką wołamy w wątku
		//! \tparam Args Typy argumentów przekazywanych do funkcji
		template<class F, class... Args>
		//! \param f Funkcja którą wołamy w wątku
		//! \param arguments Argumenty przekazywane do funkcji
		void run(F&& f, Args &&... arguments)
		{
			std::function<void()> ff(std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...));
			sharedState = utils::make_shared<SharedState>();
			thread.run([=](utils::shared_ptr<SharedState> sharedState)
			{
				InterruptibleThreadGuard<InterrupltiblePolicy> guard(sharedState->interruptible);
				try{
					ff();
				}
				catch (ThreadInterruptedException & e)
				{
					InterruptHandlingPolicy::handle(e);
				}
				catch (...){
					ExceptionHandlePolicy::handle(std::current_exception());
				}
			}, sharedState);
		}

		//! \param Other Zamieniany wątek
		void swap(InterrubtibleThread& Other)
		{
			std::swap((ExceptionHandlePolicy&)*this, (ExceptionHandlePolicy&)Other);
			std::swap((InterruptHandlingPolicy&)*this, (InterruptHandlingPolicy&)Other);
			std::swap(sharedState, Other.sharedState);
			std::swap(thread, Other.thread);
		}
		//! \return Czy można dołączyć do wątku
		bool joinable() const { return thread.joinable(); }
		//! Dołancza się do wątku, czeka na jego zakończenie
		void join() { thread.join(); }
		//! Zwalnia wątek - żyje samodzielnie, nie mamy już nad nim kontroli z poziomu tej klasy
		void detach() { thread.detach(); }
		//! \return Identyfikator wątku
		std::thread::id get_id() const { return thread.get_id(); }
		//! \return Uchwyt wątku specyficzny dla platformy
		std::thread::native_handle_type native_handle() { return thread.native_handle(); }
		//! Przerwania wątku
		void interrupt() { sharedState->interruptible.interrupt(); }
		//! \return Czy wątek jest przerywalny
		bool interruptible() const { return sharedState->interruptible.interruptible(); }
		//! Sprawdzenie przerwania wątku
		static void interruptionPoint() { InterruptiblePolicy::interruptionPoint(); }
		//! Reset przerwania wątku
		static void resetInterruption() { InterruptiblePolicy::resetInterruption(); }

	private:
		//! Faktyczny wątek
		RunnableThread thread;
		//! Prywatny stan wątku
		utils::shared_ptr<SharedState> sharedState;
	};

	template<typename RunnableThread, typename ExceptionHandlePolicy = ConsumeExceptionHandlePolicy, typename InterruptHandlingPolicy = NoInterruptHandlingPolicy, typename InterruptiblePolicyT = NoInterruptiblePolicy>
	class InterruptibleMultipleRunThread : private ExceptionHandlePolicy, private InterruptHandlingPolicy
	{
	public:

		using InterruptiblePolicy = InterruptiblePolicyT;

	private:

		using MyThreadType = InterruptibleMultipleRunThread<RunnableThread, ExceptionHandlePolicy, InterruptHandlingPolicy, InterruptiblePolicy>;

		struct SharedState
		{
			volatile bool finalize;
			std::mutex functionMutex;
			std::condition_variable functionCondition;
			utils::shared_ptr<FunctionWrapper> functionWrapper;
			InterruptiblePolicy interruptible;
		};

	public:

		InterruptibleMultipleRunThread(RunnableThread && innerThread = RunnableThread(),
			const ExceptionHandlePolicy & ehp = ExceptionHandlePolicy(),
			const InterruptHandlingPolicy & ihp = InterruptHandlingPolicy())
			: ExceptionHandlePolicy(ehp), InterruptHandlingPolicy(ihp),
			thread(std::move(innerThread)) {}

		/*template<class... Args>
		InterruptibleMultipleRunThread(const ExceptionHandlePolicy & ehp = ExceptionHandlePolicy(),
			const InterruptHandlingPolicy & ihp = InterruptHandlingPolicy(), Args&&... arguments)
			: ExceptionHandlePolicy(ehp), InterruptHandlingPolicy(ihp), thread(std::forward(arguments)...) {}
			*/

		InterruptibleMultipleRunThread(InterruptibleMultipleRunThread&& Other)
			: ExceptionHandlePolicy(std::move(Other)), InterruptHandlingPolicy(std::move(Other)),
			sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread)) {}

		InterruptibleMultipleRunThread(const InterruptibleMultipleRunThread&) = delete;
		~InterruptibleMultipleRunThread() { }


		InterruptibleMultipleRunThread& operator=(InterruptibleMultipleRunThread&& Other)
		{
			ExceptionHandlePolicy::operator=(std::move(Other));
			InterruptHandlingPolicy::operator=(std::move(Other));
			sharedState = std::move(Other.sharedState);
			thread = std::move(Other.thread);
			return *this;
		}
		InterruptibleMultipleRunThread& operator=(const InterruptibleMultipleRunThread&) = delete;

		template<typename F, class ...Args>
		InterruptibleFuture<typename std::result_of<F(Args...)>::type, InterruptiblePolicy> run(F&& f, Args&& ...arguments)
		{
			using result_type = typename std::result_of<F(Args...)>::type;

			if (sharedState != nullptr && sharedState->finalize == true){
				throw std::logic_error("Operation not permitted");
			}
			else if (sharedState == nullptr){

				sharedState = utils::make_shared<SharedState>();
				sharedState->finalize = false;

				thread.run([](utils::shared_ptr<SharedState> sharedState){

					InterruptibleThreadGuard<InterruptiblePolicy> guard(sharedState->interruptible);

					while (true){
						std::unique_lock<std::mutex> lock(sharedState->functionMutex);

						while (sharedState->functionCondition.wait_for(lock,
							std::chrono::milliseconds(100),
							[=]() { return (sharedState->functionWrapper != nullptr) || (sharedState->finalize == true); }) == false) {
						}

						if (sharedState->finalize == true){
							break;
						}

						if (sharedState->functionWrapper != nullptr){

							auto fw = std::move(sharedState->functionWrapper);
							(*fw)();

							InterruptiblePolicy::resetInterruption();
						}
					}
				}, sharedState);
			}

			InterruptibleFuture<result_type, InterruptiblePolicy> ret;

			{
				std::lock_guard<std::mutex> lock(sharedState->functionMutex);

				if (sharedState->functionWrapper != nullptr){
					if (sharedState->functionWrapper->valid() == true){
						throw std::logic_error("Operation not permitted");
					}
				}

				std::function<result_type()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

				auto l = [=]()->result_type
				{
					try{
						return ff();
					}
					catch (ThreadInterruptedException & e){
						InterruptHandlingPolicy::handle(e);
						throw;
					}
					catch (...){
						auto e = std::current_exception();
						ExceptionHandlePolicy::handle(e);
						std::rethrow_exception(e);
					}					
				};
				
				InterruptiblePackagedTask<InterruptiblePolicy, result_type()> innerTask(l);
				ret = innerTask.get_future();
				sharedState->functionWrapper.reset(new FunctionWrapper(std::move(innerTask)));
			}

			sharedState->functionCondition.notify_one();
			return ret;
		}

		void swap(InterruptibleMultipleRunThread& Other)
		{
			std::swap((ExceptionHandlePolicy&)*this, (ExceptionHandlePolicy&)Other);
			std::swap((InterruptHandlingPolicy&)*this, (InterruptHandlingPolicy&)Other);
			std::swap(sharedState,Other.sharedState);
			std::swap(thread, Other.thread);
		}

		bool joinable() const  {

			if (sharedState != nullptr && sharedState->finalize == true){
				throw std::logic_error("Operation not permitted");
			}

			return thread.joinable();
		}

		void join() {
			
			tryFinalize();			
			thread.join();
		}

		void detach() {

			tryFinalize();			
			thread.detach();
		}

		std::thread::id get_id() const { return thread.get_id(); }
		std::thread::native_handle_type native_handle() { return thread.native_handle(); }
		void interrupt() { sharedState->interruptible.interrupt(); }
		bool interruptible() const { return sharedState != nullptr && sharedState->interruptible.interruptible(); }
		static void interruptionPoint() { InterruptiblePolicy::interruptionPoint(); }
		static void resetInterruption() { InterruptiblePolicy::resetInterruption(); }

	private:

		void tryFinalize()
		{
			if (sharedState != nullptr){
				sharedState->finalize = true;
				sharedState->functionCondition.notify_one();
			}
		}

	private:
		RunnableThread thread;
		utils::shared_ptr<SharedState> sharedState;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREAD_H__
