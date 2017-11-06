/********************************************************************
	created:  2014/10/24	09:43:55
	filename: InterruptibleThreadPool.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREADPOOL_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREADPOOL_H__

#include <list>
#include <utils/Debug.h>
#include <threadingUtils/InterruptibleThread.h>
#include <threadingUtils/FutureWrapper.h>
#include <threadingUtils/ThreadPoolException.h>

namespace threadingUtils
{
	//! \tparam RunnableThread Typ w�tku runnable kt�ry wrapujemy
	//! \tparam CallPolicy Polityka wo�ania zada� w�tk�w
	//! \tparam InterruptPolicy Polityka przerywania dzia�ania w�tku
	//! \tparam InterruptHandlingPolicy Polityka obs�ugi przerwania w�tku
	//! Klasa realizuj�ca funkcjonalno�� puli w�tk�w
	template<class InterruptibleMultipleRunThreadFactory>
	class InterruptibleThreadPool : private InterruptibleMultipleRunThreadFactory
	{
	public:		

		using InterruptibleMultipleRunThread = decltype(std::declval<InterruptibleMultipleRunThreadFactory>().create());

		//! Typ listy w�tk�w wielokrotnego uruchamiania
		using InnerThreadsList = std::list<InterruptibleMultipleRunThread>;

		//! Typ opisuj�cy ilo��
		using size_type = unsigned int;

		//! Klasa w�tku oferowanego z puli
		class Thread
		{
			//! Zaprzyja�nienie z pula w�tku
			friend class MyThreadPoolType;

		public:
			//! Typ puli w�tk�w
			using MyThreadPoolType = InterruptibleThreadPool<InterruptibleMultipleRunThreadFactory>;
			using InterruptiblePolicy = typename InterruptibleMultipleRunThread::InterruptiblePolicy;

		private:

			void finalize()
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

		public:

			//! \param threadPool Pula w�tk�w do kt�rej nale�a� pierwotnie w�tek
			Thread(MyThreadPoolType * threadPool) : threadPool(threadPool)
			{

			}

			//! \param threadPool Pula w�tk�w do kt�rej nale�a� pierwotnie w�tek
			//! \param thread W�tek wielokrotnego uruchamiania
			Thread(MyThreadPoolType * threadPool, InterruptibleMultipleRunThread && thread)
				: threadPool(threadPool), thread(std::move(thread))
			{

			}

		public:

			Thread() : threadPool(nullptr)
			{

			}

			Thread(const Thread & Other) = delete;

			//! \param Other W�tek kt�rego zasoby przejmujemy
			Thread(Thread && Other) : threadPool(std::move(Other.threadPool)),
				thread(std::move(Other.thread)), futureWrapper(std::move(Other.futureWrapper))
			{
				Other.threadPool = nullptr;
			}

			//! Destruktor
			~Thread()
			{
				finalize();
			}

			Thread& operator=(Thread&& Other)
			{
				finalize();
				threadPool = std::move(Other.threadPool);
				Other.threadPool = nullptr;
				thread = std::move(Other.thread);
				futureWrapper = std::move(Other.futureWrapper);
				return *this;
			}

			Thread& operator=(const Thread&) = delete;

			template<typename F, class ...Args>
			void run(F&& f, Args&& ...arguments)
			{
				futureWrapper = std::move(thread.run(std::forward<F>(f), std::forward<Args>(arguments)...));
			}

			void swap(Thread& Other)
			{
				std::swap(threadPool, Other.threadPool);
				std::swap(thread, Other.thread);
				std::swap(futureWrapper, Other.futureWrapper);
			}

			bool joinable() const { return futureWrapper.valid() && thread.joinable(); }
			void join() { futureWrapper.wait(); }
			void detach() { threadPool->detach(); threadPool = nullptr; futureWrapper.reset(); thread.detach(); }
			std::thread::id get_id() const { return thread.get_id(); }
			std::thread::native_handle_type native_handle() { return thread.native_handle(); }
			void interrupt() { if (futureWrapper.valid() == false || thread.interruptible() == false) { throw std::logic_error("Operation not permitted"); } thread.interrupt(); }
			bool interruptible() const { return futureWrapper.valid() && thread.interruptible(); }
			static void interruptionPoint() { InterruptibleMultipleRunThread::interruptionPoint(); }
			static void resetInterruption() { InterruptibleMultipleRunThread::resetInterruption(); }

		private:
			//! Pula w�tk�w do kt�rej nale�y w�tek
			MyThreadPoolType * threadPool;
			//! Wewn�trzny w�tek wielokrotnego uruchamiania
			InterruptibleMultipleRunThread thread;
			//! Wrapper dla future celem okre�lenia czy w�tek zako�czy� przetwarzanie zleconego zadania
			FutureWrapper futureWrapper;
		};

		//! Zaprzyja�nienie w�tku z pula w�tk�w
		friend class Thread;

		class CustomThread
		{
			friend class InterruptibleThreadPool;
		private:
			CustomThread(InterruptibleThreadPool * tp, size_type size)
				: tp(tp), size_(size)
			{
				if (tp == nullptr){
					throw std::runtime_error("Invalid custom threads configuration");
				}
			}

		public:

			~CustomThread()
			{
				tp->returnCustom(size_);
			}

			size_type size() const { return size_; }

		private:
			InterruptibleThreadPool * tp;
			const size_type size_;
		};

		friend class CustomThread;

		using CustomThreadProxy = utils::unique_ptr<CustomThread>;

		//! Typ agreguj�cy w�tki kt�re pobieraj� klienci
		using Threads = std::list<Thread>;

	public:

		InterruptibleThreadPool(const InterruptibleThreadPool & Other) = delete;
		InterruptibleThreadPool(InterruptibleThreadPool && Other) = delete;

		//! \param minThreads Minimalna ilo�� w�tk�w do utrzymania
		//! \param maxThreads Maksymalna ilo�� w�tk�w do utrzymania
		InterruptibleThreadPool(const size_type minThreads, const size_type maxThreads,
			const InterruptibleMultipleRunThreadFactory & imrtf = InterruptibleMultipleRunThreadFactory())
			: InterruptibleMultipleRunThreadFactory(imrtf), minThreads_(minThreads),
			maxThreads_(maxThreads), threadsCount_(0) {}
		
		//! Domy�lny konstruktor
		InterruptibleThreadPool(const InterruptibleMultipleRunThreadFactory & imrtf = InterruptibleMultipleRunThreadFactory())
			: InterruptibleThreadPool((std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1), std::max<unsigned int>(std::thread::hardware_concurrency() * 8, 4), imrtf) {}
		
		//! Desturktor
		~InterruptibleThreadPool()
		{
			UTILS_ASSERT((threadsCount_ == 0), "ThreadPool finished while some threads are still alive");
			for (auto & t : innerThreads){
				if (t.joinable() == true){
					t.join();
				}
			}
		}

		//! \param maxThreads Maksymalna ilo�� w�tk�w jakie mo�na stworzy�
		void setMaxThreads(size_type maxThreads)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (maxThreads < minThreads_)
			{
				throw ThreadPoolConfigurationException("Improper max threads value. Must be greater or equal to minimum threads value");
			}

			maxThreads_ = maxThreads;
		}
		//! \param minThreads Minimalna ilo�� w�tk�w jaka b�dzie utrzymywana
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
		//! \return Maksymalna ilo�c w�tk�w jakie mo�na utworzy�
		size_type maxThreads() const { return maxThreads_; }
		//! \return Minimalna ilo�� w�tk�w utrzymywana przez manager
		size_type minThreads() const { return minThreads_; }
		//! \return Ilo�� aktualnie zaj�tych w�tk�w
		size_type threadsCount() const { return threadsCount_; }
		//! \return Ilo�� w�tk�w w cache
		size_type cachedThreadsCount() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return innerThreads.size();
		}
		//! \return Nowy w�tek		
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
					th = Thread(this, InterruptibleMultipleRunThreadFactory::create());
				}				
				++threadsCount_;
				return th;
			}
			else{
				throw std::runtime_error("ThreadPool empty");
			}
		}

		//! \param groupSize Ilo�� w�tk�w w grupie
		//! \param threads [out] Lista z nowymi w�tkami, dopisujemy zawsze na ko�cu
		//! \return Ilo�� faktycznie dostarczonych w�tk�w
		size_type get(const size_type groupSize, Threads & threads, const bool exact)
		{
			std::lock_guard<std::mutex> lock(mutex);

			if (exact == true && ((groupSize + threadsCount_) > maxThreads_)){
				throw std::runtime_error("ThreadPool limited resources");
			}

			const auto toDeliver = std::min(maxThreads_ - threadsCount_, groupSize);

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
					threads.push_back(Thread(this, InterruptibleMultipleRunThreadFactory::create()));
					++size;
				}

				threadsCount_ += toDeliver;
			}

			return toDeliver;
		}

		CustomThreadProxy getCustom(const size_type size, bool exact = true)
		{
			if (size <= 0){
				throw std::invalid_argument("Invalid number of custom threads");
			}

			std::lock_guard<std::mutex> lock(mutex);

			if (exact == true && ((size + threadsCount_) > maxThreads_)){
				throw std::runtime_error("ThreadPool limited resources");
			}

			auto s = std::min(maxThreads_ - threadsCount_, size);

			threadsCount_ += s;

			return CustomThreadProxy(new CustomThread(this, s));
		}

	private:

		//! Metoda zmniejsza aktualn� ilo�c w�tk�w w puli
		void detach() { --threadsCount_; }

		//! Metoda zwraca customowe w�tki tworzone poza thread pool
		void returnCustom(const size_type size)
		{			
			std::lock_guard<std::mutex> lock(mutex);
			threadsCount_ -= size;		
		}

		//! \param innerThread Wewn�trzny w�tek wielokrotnego uruchamiania, kt�ry pr�bujemy zwr�ci� przy niszczeniu dostarczonego w�tku
		void tryReturn(InterruptibleMultipleRunThread & innerThread)
		{
			std::lock_guard<std::mutex> lock(mutex);
			detach();
			if (threadsCount_ < minThreads_ && innerThread.joinable() == true){
				innerThreads.push_back(std::move(innerThread));
			}
		}

	private:
		//! Aktualna ilo�� w�tk�w w puli
		volatile size_type threadsCount_;
		//! Minimalna ilo�c w�tk�w w puli jak� chcemy utrzymywa�
		size_type minThreads_;
		//! Maksymalna ilo�� w�tk�w jakie pozwala stworzy� pula
		size_type maxThreads_;
		//! W�tki wielokrotnego uruchamiania czekaj�ce na ponowne wyko�ystanie
		InnerThreadsList innerThreads;
		//! Obiekt synchronizuj�cy pobieraj�cych
		mutable std::mutex mutex;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLETHREADPOOL_H__
