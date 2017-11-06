/********************************************************************
	created:  2014/10/24	09:44:12
	filename: InterruptibleWorkManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEWORKMANAGER_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEWORKMANAGER_H__

#include <type_traits>
#include <map>
#include <utils/Utils.h>
#include <threadingUtils/Macros.h>
#include <threadingUtils/WorkQueuePolicy.h>
#include <threadingUtils/FunctionWrapper.h>
#include <threadingUtils/InterruptibleFuture.h>
#include <threadingUtils/InterruptiblePolicy.h>
#include <threadingUtils/InterruptiblePackagedTask.h>
#include <threadingUtils/LogPolicy.h>

namespace threadingUtils
{
	//! \tparam WorkQueue Typ kolejki zadaï¿½
	//! \tparam InterruptibleThread Typ wï¿½tku ktï¿½ry moï¿½na uruchamiaï¿½ (run) oraz przerywaï¿½ (interrupt)
	//! \tparam WorkExceptionHandlePolicy
	//! \tparam InterruptHandlePolicy
	//! \tparam LogPolicy Polityka logowania
	//! Klasa realizujï¿½ca funkcjonalnoï¿½ï¿½ managera prac, zarzï¿½dzajï¿½cego zleconymi zadaniami i ich realizacjï¿½
	//! na zarzï¿½dzanych wï¿½tkach
	template<class WorkQueue, class InterruptibleThread, typename WorkExceptionHandlePolicy,
		typename InterruptHandlePolicy, typename LogPolicy = EmptyLogPolicy>
	class InterruptibleWorkManager : private WorkExceptionHandlePolicy,
		private InterruptHandlePolicy, private LogPolicy
	{
	private:

		//! RAII pomocnicze przy rozrï¿½nianiu wï¿½tkï¿½w workerï¿½w od innych zlecajï¿½cych zadania
		struct LocalThreadGuard
		{
			LocalThreadGuard()
			{
				InterruptibleWorkManager::isLocalThread = true;
			}

			~LocalThreadGuard()
			{
				InterruptibleWorkManager::isLocalThread = false;
			}
		};

		friend struct LocalThreadGuard;

		//! Klasa obsï¿½ugujï¿½ca przetwarzanie zleconych zadaï¿½ w pojedynczym wï¿½tku
		class WorkExecutor
		{
		private:

			//! RAII do obsï¿½ugi inicjalizacji/deinicjalizacji obsï¿½ugi wï¿½tku przez kolejkï¿½ zadaï¿½
			struct ThreadLocalWorkQueueController
			{
			public:

				//! \param workQueue Kolejka zadaï¿½ z ktï¿½rej koï¿½ysta aktualny wï¿½tek
				ThreadLocalWorkQueueController(WorkQueue & workQueue) : workQueue(workQueue)
				{
					workQueue.initializeThread();
				}

				//! Destruktor
				~ThreadLocalWorkQueueController()
				{
					workQueue.deinitializeThread();
				}

			private:
				//! Kolejka zadaï¿½ z ktï¿½rej koï¿½ysta aktualny wï¿½tek
				WorkQueue & workQueue;
			};

			//! Struktura opisujï¿½ca wspï¿½lny stan wï¿½tku i obiektu zarzï¿½dzajï¿½cego jego pracï¿½ przy przetwarzaniu zleconych zadaï¿½
			struct SharedState
			{
				SharedState() : workManager(nullptr), forceFinalize(false)
				{

				}

				SharedState(InterruptibleWorkManager * workManager,
					bool forceFinalize) : workManager(workManager),
					forceFinalize(forceFinalize)
				{

				}

				~SharedState()
				{

				}

				//! Manager zadaï¿½
				InterruptibleWorkManager * workManager;
				//! Czy wï¿½tek ma zakoï¿½czyï¿½ przetwarzanie zadaï¿½
				volatile bool forceFinalize;
			};

		public:
			//! \param jobManager Manager zadaï¿½
			//! \param thread Wï¿½tek ktï¿½ry przejmujemy na potrzeby prztwarzania zleconych zadaï¿½
			WorkExecutor(InterruptibleWorkManager * workManager, InterruptibleThread && thread)
				: sharedState(utils::make_shared<SharedState>(workManager, false)), thread(std::move(thread))
			{				
				this->thread.run([](utils::shared_ptr<SharedState> sharedState){
					LocalThreadGuard localThreadGuard;
					sharedState->workManager->LogPolicy::log("Starting work executor");
					ThreadLocalWorkQueueController tlwqc(sharedState->workManager->workQueue);
					sharedState->workManager->LogPolicy::log("Work queue initialized");
					while (sharedState->forceFinalize == false && sharedState->workManager->forceFinalize_ == false){
						sharedState->workManager->LogPolicy::log("Running pending task");
						sharedState->workManager->runPendingTask(std::chrono::milliseconds(100), false);
					}
					sharedState->workManager->LogPolicy::log("Finalizing work executor");
				}, sharedState);
			}

			//! \param Other Obiekt ktï¿½rego zasoby przejmujemy
			WorkExecutor(WorkExecutor && Other) : sharedState(std::move(Other.sharedState)), thread(std::move(Other.thread))
			{

			}

			WorkExecutor(const WorkExecutor & Other) = delete;

			//! Destruktor
			~WorkExecutor()
			{
			
			}

			void finalize()
			{
				if (sharedState != nullptr){
					sharedState->forceFinalize = true;
				}
			}

			//! Metoda koï¿½czy dziaï¿½anie przetwarzania zadaï¿½ przez obsï¿½ugiwany wï¿½tek
			void forceFinalize()
			{
				finalize();
				
				if (interruptible() == true){
					try{
						interrupt();
					}
					catch (...){

					}
				}
			}

			//! Metoda czeka na zakoï¿½ï¿½zenie wï¿½tku
			void join()
			{
				thread.join();
			}

			//! Metoda przerywa dziaï¿½anie watku
			void interrupt()
			{
				thread.interrupt();
			}

			bool interruptible() const
			{
				return thread.interruptible();
			}

			//! \return Czy na wï¿½tek moï¿½na czekaï¿½
			bool joinable()
			{
				return thread.joinable();
			}

			//! \return Identyfikator wï¿½tku
			std::thread::id get_id() const
			{
				return thread.get_id();
			}

		private:
			//! Wspï¿½dzielony stan przez ktï¿½ry kontrolujemy wï¿½tek obsï¿½ugujï¿½cy zlecone zadania
			utils::shared_ptr<SharedState> sharedState;
			//! Wï¿½tek obsï¿½ugujï¿½cy zlecone zadania
			InterruptibleThread thread;
		};

		friend class WorkExecutor;

		using WorkExecutorsMap = std::map<std::thread::id, WorkExecutor>;

		using size_type = typename WorkExecutorsMap::size_type;

		struct WorkSharedState
		{
			std::promise<void*> interruptPrivateData;
			utils::shared_ptr<bool> interrupt;
		};

	public:

		struct InteruptGuard
		{
			~InteruptGuard()
			{
				InterruptibleThread::resetInterruption();
			}
		};

		template<typename T>
		using FutureType = InterruptibleFuture < T, typename InterruptibleThread::InterruptiblePolicy >;

	public:

		//! Konstruktor domyï¿½lny
		InterruptibleWorkManager(const WorkExceptionHandlePolicy & wehp = WorkExceptionHandlePolicy(),
			const InterruptHandlePolicy & ihp = InterruptHandlePolicy(), const LogPolicy & logPolicy = LogPolicy())
			: WorkExceptionHandlePolicy(wehp), InterruptHandlePolicy(ihp), LogPolicy(logPolicy),
			finalize_(false), forceFinalize_(false)//,activeCounter(0)
		{

		};
		//! Destruktor
		~InterruptibleWorkManager()
		{
			join(true);
		}

		//! \tparam F Typ funkcji jakï¿½ chcemy wykonaï¿½
		//! \tparam Args Argumenty funkcji
		//! \param f Funkcja ktï¿½rï¿½ chcemy wykonaï¿½
		//! \param arguments Argumenty z jakimi chcemy wykonaï¿½ funkcjï¿½
		//! \return Future pozwalajï¿½cy czekaï¿½ na rezultat zleconego zadania
		template<typename F, class ...Args>
		FutureType<typename std::result_of<F(Args...)>::type> submit(F&& f, Args&& ...arguments)
		{
			LogPolicy::log("Submitting work");
			using result_type = typename std::result_of<F(Args...)>::type;

			std::lock_guard<std::mutex> lock(taskMutex);
			if (isLocalThread == false && finalize_ == true){
				throw std::runtime_error("Operation not permitted");
			}
			
			LogPolicy::log("Creating function");
			std::function<result_type()> intf = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);

			auto wss = utils::make_shared<WorkSharedState>();
			wss->interrupt = utils::make_shared<bool>(false);
			
			LogPolicy::log("Shared state created");
			auto innerFuture = submitWork(wss, intf);
			LogPolicy::log("Work submitted");
			InterruptibleFuture<result_type, typename InterruptibleThread::InterruptiblePolicy> ret(std::move(innerFuture), wss->interruptPrivateData.get_future(), wss->interrupt);

			auto s = workQueue.size();
			LogPolicy::log("Waking up work executors");
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
			LogPolicy::log("Finishing work submit");
			return ret;
		}
		
		template<typename Ret>
		std::future<Ret> submitWork(utils::shared_ptr<WorkSharedState> sharedState, std::function<Ret()> intf)
		{
			return workQueue.submit([sharedState, intf, this]
			{
				LogPolicy::log("In work");
				if (*(sharedState->interrupt) == true){
					throw ThreadInterruptedException();
				}

				InteruptGuard intGuard;
				InterruptibleTaskGuard<typename InterruptibleThread::InterruptiblePolicy> taskGuard(sharedState->interruptPrivateData);
				try{
					LogPolicy::log("Doing work");
					return intf();
				}
				catch (ThreadInterruptedException & e){
					LogPolicy::log("Work failed");
					this->InterruptHandlePolicy::handle(e);
					throw;
				}
				catch (...){
					LogPolicy::log("Work failed");
					auto e = std::current_exception();
					this->WorkExceptionHandlePolicy::handle(e);
					std::rethrow_exception(e);
				}
			});
		}

		//! \param thread Dodawany wï¿½tek realizujï¿½cy zadania
		//! \return Identyfikator wï¿½tku
		std::thread::id addWorkerThread(InterruptibleThread && thread)
		{
			if (finalize_ == true)
			{
				throw std::runtime_error("Operation not permitted");
			}

			WorkExecutor workExecutor(this, std::move(thread));
			const auto ret = workExecutor.get_id();
			auto pair = std::make_pair(ret, std::move(workExecutor) );
			workExecutors.insert(std::move(pair));
			//++activeCounter;
			return ret;
		}

		//! \param threadID Identyfikator usuwanego wï¿½teku realizujï¿½cego zadania
		void removeWorkerThread(const std::thread::id threadID)
		{
			auto it = workExecutors.find(threadID);
			if (it != workExecutors.end()){
				it->second.finalize();
				taskCV.notify_all();
				if (it->second.joinable() == true){
					it->second.join();
				}

				workExecutors.erase(it);
			}
		}

		//! \return Iloï¿½ï¿½ wï¿½tkï¿½w realizujï¿½cych zadania
		std::size_t workerThreadsCount() const
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

		//! Metoda czeka na zakoï¿½czenie przetwarzanych zadaï¿½
		//! \param force Czy wymyszamy koniec obsï¿½ugi zleconych zadaï¿½
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
			for (auto & we : workExecutors){
				if (we.second.joinable() == true){
					we.second.join();
				}
			}
		}

		//! Metoda sygnalizuje koniec przetwarzania dla wszystkich wï¿½tkï¿½w przetwarzajï¿½cych
		void forceFinalize()
		{
			for (auto & we : workExecutors){
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
			LogPolicy::log("In runPendingTask");
			Task task;
			if (workQueue.tryGet(task) == true){
				LogPolicy::log("Got task");
				if (task.functionWrapper.valid() == true) {
					try {
						LogPolicy::log("Running task");
						task.functionWrapper();
						LogPolicy::log("Task finished");
					}
					catch (const std::exception & e) {
						LogPolicy::log("Failed running task: " + std::string(e.what()));
					}
					catch (...) {
						LogPolicy::log("Failed running task");
					}
				}
				else {
					LogPolicy::log("Nullptr task function");
				}
			}
			else if (isLocalThread == true){
				std::unique_lock<std::mutex> lock(taskMutex);
				if ((forceFinalize_ == true) || ((finalize_ == true) && (workQueue.empty() == true))){
					LogPolicy::log("Finalizing our worker");
					forceFinalize_ = true;
					lock.unlock();
					taskCV.notify_all();
				}
				else if (waitForOther == true){
					LogPolicy::log("Waiting for other workers");
					std::this_thread::sleep_for(timeout);
				}else{
					LogPolicy::log("Waiting on conditional");
					taskCV.wait(lock);
				}				
			}
			else{
				LogPolicy::log("Sleeping");
				std::this_thread::sleep_for(timeout);
			}
		}

	private:
		//! Mapa identyfikatorï¿½w wï¿½tkï¿½w i obiektï¿½ obsï¿½ugujï¿½cych ich przetwarzanie
		WorkExecutorsMap workExecutors;
		//! Iloï¿½ï¿½ aktualnie przetwarzajï¿½cych wï¿½tkï¿½w
		//std::atomic<size_type> activeCounter;
		//! Kolejka obsï¿½ugujï¿½ca zlecane zadania
		WorkQueue workQueue;
		//! Czy koï¿½czymy dziaï¿½anie managera zadaï¿½ - dodawanie nowych nie bï¿½dzie moï¿½liwe
		volatile bool finalize_;
		//! Czy koï¿½czymy dziaï¿½anie managera zadaï¿½ - dodawanie nowych nie bï¿½dzie moï¿½liwe
		volatile bool forceFinalize_;
		//! Zmienna warunkowa do czekania na zadanie
		std::condition_variable taskCV;
		//! Obiekt synchronizujï¿½cy
		std::mutex taskMutex;
		//! Info czy wï¿½tek jest workerem czy nie
		static thread_local bool isLocalThread;
	};

	template<class WorkQueue, class InterruptibleThread, typename WorkExceptionHandlePolicy, typename InterruptHandlePolicy, typename LogPolicy>
	thread_local bool InterruptibleWorkManager<WorkQueue, InterruptibleThread, WorkExceptionHandlePolicy, InterruptHandlePolicy, LogPolicy>::isLocalThread = false;
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTIBLEWORKMANAGER_H__
