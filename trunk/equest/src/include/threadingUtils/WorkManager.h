/********************************************************************
	created:  2014/10/22	10:14:31
	filename: WorkManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__WORKMANAGER_H__
#define __HEADER_GUARD_THREADINGUTILS__WORKMANAGER_H__

#include <type_traits>
#include <map>
#include <threadingUtils/Future.h>
#include <threadingUtils/Macros.h>
#include <threadingUtils/FunctionWrapper.h>
#include <threadingUtils/LogPolicy.h>

namespace threadingUtils {

	//! \tparam WorkQueue Typ kolejki zadañ
	//! \tparam RunnableThread Typ w¹tku który mo¿na uruchamiaæ (run)
	//! \tparam WorkExceptionHandlePolicy
	//! \tparam LogPolicy
	//! Klasa realizuj¹ca funkcjonalnoœæ managera prac, zarz¹dzaj¹cego zleconymi zadaniami i ich realizacj¹
	//! na zarz¹dzanych w¹tkach
	template<class WorkQueue, class RunnableThread, typename WorkExceptionHandlePolicy = ConsumeExceptionHandlePolicy, typename LogPolicy = EmptyLogPolicy>
	class WorkManager : private WorkExceptionHandlePolicy, private LogPolicy
	{
	private:

		//! RAII pomocnicze przy rozró¿nianiu w¹tków workerów od innych zlecaj¹cych zadania
		struct WorkerThreadGuard
		{
			WorkerThreadGuard()
			{
				WorkManager::isLocalThread = true;
			}

			~WorkerThreadGuard()
			{
				WorkManager::isLocalThread = false;
			}
		};

		friend class WorkerThreadGuard;

		//! Klasa obs³uguj¹ca przetwarzanie zleconych zadañ w pojedynczym w¹tku
		class WorkExecutor
		{
		private:

			//! RAII do obs³ugi inicjalizacji/deinicjalizacji obs³ugi w¹tku przez kolejkê zadañ
			struct LocalWorkQueueGuard
			{
			public:

				//! \param workQueue Kolejka zadañ z której ko¿ysta aktualny w¹tek
				LocalWorkQueueGuard(WorkQueue & workQueue) : workQueue(workQueue)
				{
					workQueue.initializeThread();
				}

				//! Destruktor
				~LocalWorkQueueGuard()
				{
					workQueue.deinitializeThread();
				}

			private:
				//! Kolejka zadañ z której ko¿ysta aktualny w¹tek
				WorkQueue & workQueue;
			};

			//! Struktura opisuj¹ca wspólny stan w¹tku i obiektu zarz¹dzaj¹cego jego prac¹ przy przetwarzaniu zleconych zadañ
			struct SharedState
			{
				//! Manager zadañ
				WorkManager * workManager;
				//! Czy w¹tek ma zakoñczyæ przetwarzanie zadañ
				volatile bool forceFinalize;
			};

		public:
			//! \param jobManager Manager zadañ
			//! \param thread W¹tek który przejmujemy na potrzeby prztwarzania zleconych zadañ
			WorkExecutor(WorkManager * workManager, RunnableThread && thread)
				: sharedState(utils::make_shared<SharedState>(workManager, false), thread(std::move(thread))
			{
				this->thread.run([](utils::shared_ptr<SharedState> sharedState) {
					WorkerThreadGuard localThreadGuard;
					LocalWorkQueueGuard lqqg(sharedState->workManager->workQueue);
					while (sharedState->forceFinalize == false && sharedState->workManager->forceFinalize == false){
						sharedState->workManager->runPendingTask(std::chrono::milliseconds(100), false);
					}
				}, sharedState);
			}

			//! \param Other Obiekt którego zasoby przejmujemy
			WorkExecutor(WorkExecutor && Other) : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread))
			{

			}

			//! Destruktor
			~WorkExecutor() {}

			//! Metoda koñczy dzia³anie przetwarzania zadañ przez obs³ugiwany w¹tek
			void forceFinalize() { if (sharedState != nullptr) sharedState->forceFinalize = true; }

			//! Metoda czeka na zakoñæzenie w¹tku
			void join()
			{
				thread.join();
			}

			//! \return Czy na w¹tek mo¿na czekaæ
			bool joinable()
			{
				return thread.joinable();
			}

			//! \return Identyfikator w¹tku
			std::thread::id get_id() const
			{
				return thread.get_id();
			}

			//! \return Identyfikator w¹tku
			void detach()
			{
				thread.detach();
			}

		private:
			//! Wspó³dzielony stan przez który kontrolujemy w¹tek obs³uguj¹cy zlecone zadania
			utils::shared_ptr<SharedState> sharedState;
			//! W¹tek obs³uguj¹cy zlecone zadania
			RunnableThread thread;
		};

		friend class WorkExector;

		using WorkExecutorsMap = std::map<std::thread::id, WorkExecutor>;

		using size_type = WorkExecutorsMap::size_type;

	public:

		template<typename T>
		struct FutureType
		{
			using type = std::future<T>;
		};

	public:

		//! Konstruktor domyœlny
		WorkManager(const WorkExceptionHandlePolicy & wehp = WorkExceptionHandlePolicy(), const LogPolicy & logPolicy = LogPolicy())
			: WorkExceptionHandlePolicy(wehp), LogPolicy(logPolicy)
		{

		}
		//! Destruktor
		~WorkManager()
		{
			LogPolicy::log("Work manager destructor: joining");
			join(true);
			LogPolicy::log("Work manager destructor: finished");
		}

		//! \tparam F Typ funkcji jak¹ chcemy wykonaæ
		//! \tparam Args Argumenty funkcji
		//! \param f Funkcja któr¹ chcemy wykonaæ
		//! \param arguments Argumenty z jakimi chcemy wykonaæ funkcjê
		//! \return Future pozwalaj¹cy czekaæ na rezultat zleconego zadania
		template<typename F, class ...Args>
		typename FutureType<typename std::result_of<F(Args)>::type>::type submit(F&& f, Args&& arguments...)
		{
			LogPolicy::log("Work manager: submiting work");
			using result_type = typename std::result_of<F(Args)>::type;

			std::lock_guard<std::mutex> lock(taskMutex);
			if (isLocalThread == false && finalize_ == true){
				throw std::runtime_error("Operation not permitted");				
			}

			std::function<result_type()> ff = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);
			LogPolicy::log("Work queue: submiting task");
			auto ret = workQueue.submit([=, this]()->result_type {
				this->LogPolicy::log("Work manager task running");
				try{
					return ff();
				}
				catch (...){
					auto e = std::current_exception();
					WorkExceptionHandlePolicy::handle(e);
					std::rethrow_exception(e);
				}
			});

			auto s = workQueue.size();
			LogPolicy::log("Work manager waking work executors");
			if (isLocalThread == false || s > 1){

				if (s > workExecutors.size()){
					taskCV.notify_all();
				}
				else{
					taskCV.notify_one();
					if (s > 0){
						--s;
						while (s-- > 0)	{ taskCV.notify_one(); };
					}
				}
			}
			LogPolicy::log("Work manager submit finished");
			return ret;
		}

		//! \param thread Dodawany w¹tek realizuj¹cy zadania
		//! \return Identyfikator w¹tku
		std::thread::id addWorkerThread(RunnableThread && thread)
		{
			if (finalize_ == true)
			{
				throw std::runtime_error("Operation not permitted");
			}

			WorkExecutor workExecutor(this, std::move(thread));
			const auto ret = workExecutor.get_id();
			workExecutors.insert({ ret, std::move(workExecutor) });
			//++activeCounter;
			return ret;
		}

		//! \param threadID Identyfikator usuwanego w¹teku realizuj¹cego zadania
		void removeWorkerThread(const std::thread::id threadID)
		{
			auto it = workExecutors.find(threadID);
			if (it != workExecutors.end()){
				it->second.forceFinalize();
				taskCV.notify_all();
				if (it->second.joinable() == true){
					it->second.join();
				}

				workExecutors.erase(it);
			}
		}

		//! \return Iloœæ w¹tków realizuj¹cych zadania
		unsigned int workerThreadsCount() const
		{
			return workExecutors.size();
		}

		//! \tparam T Typ wyniku future
		//! \tparam Rep Typ reprezentacji czasu
		//! \tparam Per Typ okresu czasu		
		template<typename Future, class Rep = long long, class Per = std::milli>
		//! \param future Future na ktï¿½rego wykonanie czekamy realizujï¿½c inne zadania
		//! \param timeout Czas uspania podczas czekania
		void waitForOtherTask(Future & future, const std::chrono::duration<Rep, Per> & timeout = std::chrono::duration<Rep, Per>(std::chrono::milliseconds(100)))
		{
			while (future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) { runPendingTask(timeout, true); }
		}

		//! Metoda czeka na zakoñczenie przetwarzanych zadañ
		//! \param force Czy wymyszamy koniec obs³ugi zleconych zadañ
		void join(const bool force = false)
		{
			{
				std::lock_guard<std::mutex> lock(taskMutex);
				finalize_ = true;
				if (force == true){
					forceFinalize();
				}
			}

			taskCV.notify_all();

			innerJoin();
			WorkExecutorsMap().swap(workExecutors);
		}

	private:

		void innerJoin()
		{
			for (auto we : workExecutors){
				if (we.second.joinable() == true){
					we.second.join();
				}
			}
		}

		//! Metoda sygnalizuje koniec przetwarzania dla wszystkich w¹tków przetwarzaj¹cych
		void forceFinalize()
		{
			for (auto we : workExecutors){
				we.second.forceFinalize();
			}
		}

		//! Metoda pobierajï¿½ca i przetwarzajï¿½ca kolejne zadania z kolejki zadaï¿½
		//! \tparam Rep Typ reprezentacji czasu
		//! \tparam Per Typ okresu czasu	
		template<class Rep = long long, class Per = std::milli>
		//! \param timeout Czas uspania podczas czekania
		//! \param waitForOther Czy oczekujemy na inne zadanie - wtedy nie mozemy sie uspaæ
		void runPendingTask(const std::chrono::duration<Rep, Per> & timeout = std::chrono::duration<Rep, Per>(std::chrono::milliseconds(100)),
			const bool waitForOther = false)
		{
			LogPolicy::log("Work manager in runPendingTask");
			Task task;
			if (workQueue.tryGet(task) == true){
				LogPolicy::log("Work manager running task");
				task.functionWrapper();
			}
			else if (isLocalThread == true){
				std::unique_lock<std::mutex> lock(taskMutex);
				if ((forceFinalize_ == true) || ((finalize_ == true) && (workQueue.empty() == true))){
					LogPolicy::log("Work manager finalizing work executor");
					forceFinalize_ = true;
					lock.unlock();
					taskCV.notify_all();
				}
				else if (waitForOther == true){
					LogPolicy::log("Work manager waiting for others");
					std::this_thread::sleep_for(timeout);
				}
				else{
					LogPolicy::log("Work manager waiting on CV");
					taskCV.wait(lock);
				}
			}
			else{
				LogPolicy::log("Work manager external waiting");
				std::this_thread::sleep_for(timeout);
			}
		}

	private:
		//! Mapa identyfikatorów w¹tków i obiektó obs³uguj¹cych ich przetwarzanie
		WorkExecutorsMap workExecutors;
		//! Iloœæ aktualnie przetwarzaj¹cych w¹tków
		//std::atomic<WorkExecutorsMap::size_type> activeCounter;
		//! Kolejka obs³uguj¹ca zlecane zadania
		WorkQueue workQueue;
		//! Czy koñczymy dzia³anie managera zadañ - dodawanie nowych nie bêdzie mo¿liwe
		volatile bool finalize_;
		//! Czy koñczymy dzia³anie managera zadañ - dodawanie nowych nie bêdzie mo¿liwe
		volatile bool forceFinalize_;
		//! Zmienna warunkowa do czekania na zadanie
		std::condition_variable taskCV;
		//! Obiekt synchronizuj¹cy
		std::mutex taskMutex;
		//! Info czy w¹tek jest workerem czy nie
		static thread_local bool isLocalThread;
	};

	template<class WorkQueue, class InterruptibleThread, typename WorkExceptionHandlePolicy, typename InterruptHandlePolicy, typename LogPolicy>
	thread_local bool InterruptibleWorkManager<WorkQueue, InterruptibleThread, WorkExceptionHandlePolicy, InterruptHandlePolicy, LogPolicy>::isLocalThread = false;
}

#endif	// __HEADER_GUARD_THREADINGUTILS__WORKMANAGER_H__