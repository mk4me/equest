/********************************************************************
	created:  2014/10/02	10:04:35
	filename: RunnableThread.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__RUNNABLETHREAD_H__
#define __HEADER_GUARD_THREADINGUTILS__RUNNABLETHREAD_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <threadingUtils/Future.h>
#include <threadingUtils/ExceptionHandlePolicy.h>
#include <threadingUtils/FunctionWrapper.h>
#include <utils/Utils.h>
namespace threadingUtils
{
	//! \tparam Thread Typ wątku jaki wrapujemy do postaci runnable
	//! \tparam CallPolicy Sposób wołania metod przez wątek
	template<typename Thread, typename ExceptionHandlePolicy = RethrowExceptionHandlePolicy>
	class RunnableThread : private ExceptionHandlePolicy
	{
	public:

		//! Konstruktor domyślny
		RunnableThread(Thread && innerThread, const ExceptionHandlePolicy & ehp = ExceptionHandlePolicy())
			: ExceptionHandlePolicy(ehp), thread(std::move(innerThread)), launched(false) {}

		template<class... Args>
		RunnableThread(const ExceptionHandlePolicy & ehp = ExceptionHandlePolicy(), Args&&... arguments)
			: ExceptionHandlePolicy(ehp), thread(std::forward<Args>(arguments)...), launched(false) {}

		//! \param Other wątek którego zasoby przejmujemy
		RunnableThread(RunnableThread&& Other)
			: ExceptionHandlePolicy(std::move(Other)), launched(std::move(Other.launched)), thread(std::move(Other.thread)) {}
		RunnableThread(const RunnableThread&) = delete;
		//! Destruktor
		~RunnableThread() {}
		
		//! \param Other wątek którego zasoby przejmujemy
		RunnableThread& operator=(RunnableThread&& Other)
		{
			ExceptionHandlePolicy::operator=(std::move(Other));
			launched = std::move(Other.launched);
			thread = std::move(Other.thread);
			return *this;
		}

		RunnableThread& operator=(const RunnableThread&) = delete;

		template<class F, class... Args>
		void run(F&& f, Args&&... arguments)
		{
			if (launched == true){
				throw std::logic_error("Operation not permitted");
			}
			else{				

				std::function<void()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

				thread = Thread([=]{

					try{
						ff();
					}
					catch (...){
						ExceptionHandlePolicy::handle(std::current_exception());
					}
					});
				launched = true;
			}
		}

		void swap(RunnableThread & Other) { std::swap(launched, Other.launched); std::swap(thread, Other.thread); }
		bool joinable() const  { return thread.joinable(); }
		void join() { thread.join(); }
		void detach() { thread.detach(); }
		std::thread::id get_id() const  { return thread.get_id(); }
		std::thread::native_handle_type native_handle() { return thread.native_handle(); }

	private:
		bool launched;
		Thread thread;
	};

	template<typename Thread, typename ExceptionHandlePolicy = ConsumeExceptionHandlePolicy>
	class MultipleRunThread : private ExceptionHandlePolicy
	{
	private:

		using MyMultipleRunThread = MultipleRunThread<Thread, ExceptionHandlePolicy>;

		struct SharedState
		{
			volatile bool finalize;
			std::mutex functionMutex;
			std::condition_variable functionCondition;
			utils::shared_ptr<FunctionWrapper> functionWrapper;			
		};

	public:

		MultipleRunThread(Thread && innerThread, const ExceptionHandlePolicy & ehp = ExceptionHandlePolicy())
			: ExceptionHandlePolicy(ehp), thread(std::move(innerThread)) {}

		template<class... Args>
		MultipleRunThread(const ExceptionHandlePolicy & ehp = ExceptionHandlePolicy(), Args&&... arguments) : ExceptionHandlePolicy(ehp), thread(std::forward(arguments)...) {}

		MultipleRunThread(MultipleRunThread&& Other) : ExceptionHandlePolicy(std::move(Other)), sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread)) {}
		MultipleRunThread(const MultipleRunThread&) = delete;
		~MultipleRunThread() { }


		MultipleRunThread& operator=(MultipleRunThread&& Other)
		{
			ExceptionHandlePolicy::operator=(std::move(Other));
			sharedState = std::move(Other.sharedState);
			thread = std::move(Other.thread);
			return *this;
		}

		MultipleRunThread& operator=(const MultipleRunThread&) = delete;

		template<typename F, class ...Args>
		std::future<typename std::result_of<F(Args...)>::type> run(F&& f, Args&& ...arguments)
		{
			using result_type = typename std::result_of<F(Args...)>::type;

			if (sharedState != nullptr && sharedState->finalize == true){
				throw std::logic_error("Operation not permitted");
			}else if (joinable() == false){

				sharedState = utils::make_shared<SharedState>();
				sharedState->finalize = false;

				thread = Thread([](utils::shared_ptr<SharedState> sharedState){

					// Pętla wykonująca kolejne operacje
					while (true){
						std::unique_lock<std::mutex> lock(sharedState->functionMutex);
						// Pętla czekająca na operację
						while (sharedState->functionCondition.wait_for(lock,
							std::chrono::milliseconds(100),
							[=]() { return (sharedState->functionWrapper != nullptr) || (sharedState->finalize == true); }) == false) {}

						if (sharedState->finalize == true){
							break;
						}

						if (sharedState->functionWrapper != nullptr){
							auto fw = std::move(sharedState->functionWrapper);
							(*fw)();
						}
					}
				}, sharedState);
			}

			std::future<result_type> ret;

			{
				std::lock_guard<std::mutex> lock(sharedState->functionMutex);

				if (sharedState->functionWrapper != nullptr && sharedState->functionWrapper->valid() == true){
					throw std::logic_error("Operation not permitted");
				}

				std::function<result_type()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

				auto l = [=]()->result_type
				{
					try{
						return ff();
					}
					catch (...){
						auto e = std::current_exception();
						ExceptionHandlePolicy::handle(e);
						// Musze zagwarantować że future dostanie jakiś wyjątek
						// gdyby ExceptionHandlePolicy chciał zgasić wyjątki,
						// może też rzucić inny wyjątek lub coś sobie z nim zrobić
						std::rethrow_exception(e);
					}
				};

				std::packaged_task<result_type()> innerTask(l);
				ret = innerTask.get_future();
				sharedState->functionWrapper.reset(new FunctionWrapper(std::move(innerTask)));
			}

			sharedState->functionCondition.notify_one();
			return ret;
		}

		void swap(MultipleRunThread& Other) 
		{
			std::swap(sharedState, Other.sharedState);
			std::swap(thread, Other.thread);
		}

		bool joinable() const 
		{
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

	private:

		void tryFinalize()
		{
			if (sharedState != nullptr){
				sharedState->finalize = true;
				sharedState->functionCondition.notify_one();
			}
		}

	private:
		Thread thread;
		utils::shared_ptr<SharedState> sharedState;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__RUNNABLETHREAD_H__
