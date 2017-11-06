/********************************************************************
	created:  2013/03/26
	created:  26:3:2013   15:30
	filename: ThreadPool.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__THREADPOOL_H__
#define HEADER_GUARD_THREADINGUTILS__THREADPOOL_H__

#include <list>
#include <threadingUtils/ThreadPoolException.h>
#include <threadingUtils/Export.h>
#include <threadingUtils/FutureWrapper.h>
#include <utils/SmartPtr.h>
#include <mutex>

namespace threadingUtils {

	//! \tparam InnerThread Typ wątku runnable który wrapujemy
	//! \tparam InterruptPolicy Polityka przerywania działania wątku
	//! \tparam InterruptHandlingPolicy Polityka obsługi przerwania wątku
	//! Klasa realizująca funkcjonalność puli wątków
	template<class MultipleRunThreadFactory>
	class ThreadPool : private MultipleRunThreadFactory
	{
	private:
		//! Typ puli wątków
		using MyThreadPoolType = ThreadPool<MultipleRunThread>;
		//! Typ listy wątków wielokrotnego uruchamiania
		using InnerThreadsList = std::list<MultipleRunThread>;

	public:

		using MultipleRunThread = decltype(std::declval<MultipleRunThreadFactory>().create());

		//! Typ opisujący ilość
		using size_type = unsigned int;

		//! Klasa wątku oferowanego z puli
		class Thread
		{
			//! Zaprzyjaźnienie z pula wątku
			friend class MyThreadPoolType;

		private:

			//! \param threadPool Pula wątków do której należał pierwotnie wątek
			Thread(MyThreadPoolType * threadPool) : threadPool(threadPool)
			{

			}

			//! \param threadPool Pula wątków do której należał pierwotnie wątek
			//! \param thread Wątek wielokrotnego uruchamiania
			Thread(MyThreadPoolType * threadPool, MultipleRunThread && thread)
				: threadPool(threadPool), thread(std::move(thread))
			{

			}

		public:

			Thread() : threadPool(nullptr) {}

			Thread(const Thread & Other) = delete;

			//! \param Other Wątek którego zasoby przejmujemy
			Thread(Thread && Other) : threadPool(std::move(Other.threadPool)),
				thread(std::move(Other.thread)), futureWrapper(std::move(Other.futureWrapper))
			{
				Other.threadPool = nullptr;
			}

			//! Destruktor
			~Thread()
			{
				if (joinable() == true){
					join();
				}

				if (threadPool != nullptr){
					threadPool->tryReturn(thread);
				}

				if (thread.joinable() == true){
					thread.join();
				}
			}

			Thread& operator=(Thread&& Other) { threadPool(std::move(Other.threadPool)); thread(std::move(Other.thread)); futureWrapper = std::move(Other.futureWrapper); Other.threadPool = nullptr; return *this; }
			Thread& operator=(const Thread&) = delete;

			template<typename F, class ...Args>
			void run(F&& f, Args&& ...arguments)
			{
				futureWrapper = std::move(thread.run(std::forward<F>(f), std::forward<Args>(arguments)...));
			}

			void swap(Thread& Other) { std::swap(threadPool, Other.threadPool); std::swap(thread, Other.thread); std::swap(futureWrapper, Other.futureWrapper); }
			bool joinable() const { return futureWrapper.valid() && thread.joinable(); }
			void join() { futureWrapper.wait(); }
			void detach() { threadPool->secureDetach(); threadPool = nullptr; futureWrapper.reset(); thread.detach(); }
			std::thread::id get_id() const { return thread.get_id(); }
			std::thread::native_handle_type native_handle() { return thread.native_handle(); }

		private:
			//! Pula wątków do której należy wątek
			MyThreadPoolType * threadPool;
			//! Wewnętrzny wątek wielokrotnego uruchamiania
			MultipleRunThread thread;
			//! Wrapper dla future celem określenia czy wątek zakończył przetwarzanie zleconego zadania
			FutureWrapper futureWrapper;
		};

		//! Zaprzyjaźnienie wątku z pula wątków
		friend class Thread;

		//! Typ agregujący wątki które pobierają klienci
		using Threads = std::list<Thread>;

	public:

		ThreadPool(const ThreadPool & other) = delete;
		ThreadPool(ThreadPool && other) = delete;

		//! Domyślny konstruktor
		ThreadPool(const MultipleRunThreadFactory & mrtf = MultipleRunThreadFactory())
			: ThreadPool((std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1), std::max<unsigned int>(std::thread::hardware_concurrency() * 8, 4), mrtf) {}
		//! \param minThreads Minimalna ilość wątków do utrzymania
		//! \param maxThreads Maksymalna ilość wątków do utrzymania
		ThreadPool(size_type minThreads, size_type maxThreads, const MultipleRunThreadFactory & mrtf = MultipleRunThreadFactory())
			: MultipleRunThreadFactory(mrtf), minThreads(minThreads), maxThreads(maxThreads), threadsCount_(0)
		{
			if (maxThreads_ <= 0){
				throw ThreadPoolConfigurationException("Improper max threads value. Must be greater than 0");
			}

			if (minThreads_ > maxThreads_){
				throw ThreadPoolConfigurationException("Improper min threads value. Must be less or equal to maximum threads value");
			}
		}
		//! Desturktor
		~ThreadPool()
		{
			std::lock_guard<std::mutex> lock(mutex);
			UTILS_ASSERT((threadsCount_ == 0), "Kończymy ThreadPool kiedy część wątków nadal żyje");
			for (auto & t : innerThreads){
				if (t.joinable() == true){
					t.join();
				}
			}
		}

		//! \param maxThreads Maksymalna ilość wątków jakie można stworzyć
		void setMaxThreads(size_type maxThreads)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (maxThreads < minThreads_)
			{
				throw ThreadPoolConfigurationException("Improper max threads value. Must be greater or equal to minimum threads value");
			}

			maxThreads_ = maxThreads;
		}

		//! \param minThreads Minimalna ilość wątków jaka będzie utrzymywana
		void setMinThreads(size_type minThreads)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (maxThreads < minThreads_)
			{
				throw ThreadPoolConfigurationException("Improper min threads value. Must be less or equal to maximum threads value");
			}

			if (minThreads < 0)
			{
				throw ThreadPoolConfigurationException("Improper min threads value. Must be greater or equal 0");
			}
			
			minThreads_ = minThreads;
			while (innerThreads.size() > minThreads_)
			{
				auto t = std::move(innerThreads.front());
				innerThreads.pop_front();
				if (t.joinable() == true){
					try{
						t.join();
					}
					catch (...){

					}
				}				
			}
		}
		//! \return Maksymalna ilośc wątków jakie można utworzyć
		size_type maxThreads() const { return maxThreads_; }
		//! \return Minimalna ilość wątków utrzymywana przez manager
		size_type minThreads() const { return minThreads_; }
		//! \return Ilość aktualnie zajętych wątków
		size_type threadsCount() const { return threadsCount_; }
		//! \return Nowy wątek		
		Thread get()
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (threadsCount_ < maxThreads_){
				Thread th;
				if (innerThreads.empty() == false){
					th = Thread(this, std::move(innerThreads.front()));
					innerThreads.pop_front();
				}
				else{
					th = Thread(this, MultipleRunThreadFactory::create());
				}			
				++threadsCount_;
				return th;
			}
			else{
				throw ThreadPoolEmptyException("ThreadPool empty");
			}
		}

		//! \param groupSize Ilość wątków w grupie
		//! \param threads [out] Lista z nowymi wątkami, dopisujemy zawsze na końcu
		//! \return Ilość faktycznie dostarczonych wątków
		size_type get(const size_type groupSize, Threads & threads, const bool exact)
		{
			std::lock_guard<std::mutex> lock(mutex);

			if (exact == true && groupSize + threadsCount_ >= maxThreads_){
				throw ThreadPoolEmptyException("Insufficient ThreadPool resources");
			}

			const auto toDeliver =  std::min(maxThreads_ - threadsCount_, groupSize);

			if (toDeliver > 0){

				size_type size = 0;
				while (innerThreads.empty() == false && size < toDeliver)
				{
					threads.push_back(Thread(this, std::move(innerThreads.front())));
					innerThreads.pop_front();
					++size;
				}

				while (size < toDeliver)
				{
					threads.push_back(Thread(this, MultipleRunThreadFactory::create()));
					++size;
				}

				threadsCount_ += toDeliver;
			}

			return toDeliver;
		}

	private:

		//! Metoda zmniejsza aktualną ilośc wątków w puli
		void detach() { --threadsCount_; }

		void secureDetach()
		{
			std::lock_guard<std::mutex> lock(mutex);
			detach();
		}

		//! \param innerThread Wewnętrzny wątek wielokrotnego uruchamiania, który próbujemy zwrócić przy niszczeniu dostarczonego wątku
		void tryReturn(MultipleRunThread & innerThread)
		{
			std::lock_guard<std::mutex> lock(mutex);
			detach();
			if (threadsCount_ < minThreads_){
				innerThreads.push_back(std::move(innerThread));
			}
		}

	private:
		//! Aktualna ilość wątków w puli
		volatile size_type threadsCount_;
		//! Minimalna ilośc wątków w puli jaką chcemy utrzymywać
		size_type minThreads_;
		//! Maksymalna ilość wątków jakie pozwala stworzyć pula
		size_type maxThreads_;
		//! Wątki wielokrotnego uruchamiania czekające na ponowne wykożystanie
		InnerThreadsList innerThreads;
		//! Obiekt synchronizujący pobierających
		std::mutex mutex;
	};
}

#endif	//	HEADER_GUARD_THREADINGUTILS__THREADPOOL_H__
