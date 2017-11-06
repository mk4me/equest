/********************************************************************
	created:  2012/10/31
	created:  31:10:2012   12:55
	filename: JobManager.h
	author:   Mateusz Janiak

	purpose:  Klasa realizuj¹ca manager zadañ, pozwala manipulowac iloœci¹
	w¹tków realizuj¹cych zadania
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__JOBMANAGER_H__
#define HEADER_GUARD_THREADINGUTILS__JOBMANAGER_H__

#include <threadingUtils/IJob.h>
#include <threadingUtils/FunctionWrapper.h>
#include <mutex>
#include <type_traits>
#include <utils/SmartPtr.h>

namespace threadingUtils
{
	template<typename, typename>
	class JobManager;

	//! \tparam T Typ zwracany przez zadanie
	//! \tparam WorkManager Typ work managera któy bêdzie obs³ugiwa³ zadania	
	template<typename T, typename WorkManager>
	class JobImplementation : public IJob
	{
		friend class JobManager<WorkManager>;

	private:

		//! Wspó³dzielony stan zadania
		struct SharedState
		{
			//! Wrapper opakowanego zadania
			std::function<T()> functionWrapper;
			//! Status zadania
			volatile Status status_;
			//! Obiekt synchronizuj¹cy status
			std::recursive_mutex satusMutex;
		};

	private:

		//! \tparam FuncitonType Typ funkcji zlecanej do wykonania w zadaniu			
		//! \param workManager Manager prac w ramach którego bêdzie realizowane zadanie
		//! \param f Funkcja do wykonania
		template<typename F, class ...Args>
		JobImplementation(WorkManager * workManager, F&& f, Args&& arguments...) : workManager(workManager),
			sharedState(utils::make_shared<SharedState>()), detached_(false)
		{
			sharedState->functionWrapper = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);
			sharedState->status_ = Initialized;
		}

	public:

		//! Domyœlny konstruktor
		JobImplementation() : workManager(nullptr), detached_(false) {}

		//! Konstruktor przenosz¹cy w³asnoœæ
		JobImplementation(JobImplementation && Other) : workManager(std::move(Other.workManager)),
			sharedState(std::move((Other.sharedState)),
			future(std::move(Other.future)),
			detached_(std::move(Other.detached_))
		{

		}

		//! Konstruktor kopiuj¹cy
		JobImplementation(const JobImplementation & Other) = delete;

		//! Destruktor wirtualny - czeka na zakoñczenie zadanie je¿eli nie by³o ono od³¹czone i jest na co czekaæ (detach)
		virtual ~JobImplementation()
		{
			if (detached_ == false && future.valid() == true){
				wait();
			}
		}

		//! \param Other Zadanie którego zasoby przejmujemy
		JobImplementation& operator=(JobImplementation&& Other)
		{
			workManager = std::move(Other.workManager);
			sharedState = std::move((Other.sharedState);
			future = std::move(Other.future);
			detached_ = std::move(Other.detached_);
		}

		//! Operator przypisania kopiuj¹cego
		JobImplementation& operator=(const JobImplementation&) = delete;

		//! \param Other Inne zadanie z którym zamieniamy nasze zasoby
		void swap(JobImplementation & Other)
		{
			std::swap(workManager, Other.workManager);
			std::swap(sharedState, Other.sharedState);
			std::swap(future, Other.future);
			std::swap(detached_, Other.detached_);
		}

		virtual void start() override
		{
			if (sharedState == nullptr){
				throw std::runtime_error("No job assigned");
			}
			else if (workManager == nullptr){
				sharedState->status_ = Failed;
			}
			else if (sharedState->status_ != Initialized)
			{
				throw std::runtime_error("Job already started");
			}

			sharedState->status_ = Pending;

			try{
				future = workManager->submit([](utils::shared_ptr<SharedState> sharedState){

					{
						std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
						if (sharedState->status_ == Cancelled){
							throw std::runtime_error("Work canceled");
						}

						sharedState->status_ = Running;
					}
					try{
						T ret(sharedState->functionWrapper());
						{
							std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
							sharedState->status_ = Finished;
						}
						return ret;
					}
					catch (...){
						std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
						sharedState->status_ = Failed;
						throw;
					}
				}, sharedState);
			}
			catch (...){
				sharedState->status_ = Failed;
			}
		}

		virtual void wait() override
		{
			future.wait();
		}

		virtual void cancel() override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (cancellable() == true){
				sharedState->status_ = Cancelled;
			}
		}

		virtual const bool cancellable() const override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (sharedState == nullptr || sharedState->status_ == Cancelled || sharedState->status_ == Finished || sharedState->status_ == Failed || sharedState->status_ == Running){
				return false;
			}

			return true;
		}

		virtual void detach() override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (sharedState == nullptr || (sharedState != nullptr && (sharedState->status_ == Pending || sharedState->status_ == Running))){
				detached_ = true;
			}
		}

		template<class Rep, class Per>
		const std::future_status wait_for(
			const std::chrono::duration<Rep, Per>& relTime) const
		{	// wait for duration
			return future.wait_for(relTime);
		}

		template<class _Clock, class _Dur>
		const std::future_status wait_until(
			const std::chrono::time_point<_Clock, _Dur>& absTime) const
		{	// wait until time point
			return future.wait_until(absTime);
		}

		virtual const Status status() const override
		{
			return (sharedState == nullptr) ? Initialized : sharedState->status_;
		}

		const T get()
		{
			return future.get();
		}

	private:
		WorkManager * workManager;
		bool detached_;
		typename WorkManager::FutureType<T>::type future;
		utils::shared_ptr<SharedState> sharedState;
	};

	//! \tparam T Typ zwracany przez zadanie
	//! \tparam WorkManager Typ work managera któy bêdzie obs³ugiwa³ zadania	
	template<typename T, typename WorkManager>
	class JobImplementation<T&, WorkManager> : public IJob
	{
		friend class JobManager<WorkManager>;

	private:

		//! Wspó³dzielony stan zadania
		struct SharedState
		{
			//! Wrapper opakowanego zadania
			std::function<T&()> functionWrapper;
			//! Status zadania
			volatile Status status_;
			//! Obiekt synchronizuj¹cy status
			std::recursive_mutex satusMutex;
		};

	private:

		//! \tparam FuncitonType Typ funkcji zlecanej do wykonania w zadaniu			
		//! \param workManager Manager prac w ramach którego bêdzie realizowane zadanie
		//! \param f Funkcja do wykonania
		template<typename F, class ...Args>
		JobImplementation(WorkManager * workManager, F&& f, Args&& arguments...) : workManager(workManager),
			sharedState(utils::make_shared<SharedState>()), detached_(false)
		{
			sharedState->functionWrapper = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);
			sharedState->status_ = Initialized;
		}

	public:

		//! Domyœlny konstruktor
		JobImplementation() : workManager(nullptr), detached_(false) {}

		//! Konstruktor przenosz¹cy w³asnoœæ
		JobImplementation(JobImplementation && Other) : workManager(std::move(Other.workManager)),
			sharedState(std::move((Other.sharedState)),
			future(std::move(Other.future)),
			detached_(std::move(Other.detached_))
		{

		}

		//! Konstruktor kopiuj¹cy
		JobImplementation(const JobImplementation & Other) = delete;

		//! Destruktor wirtualny - czeka na zakoñczenie zadanie je¿eli nie by³o ono od³¹czone i jest na co czekaæ (detach)
		virtual ~JobImplementation()
		{
			if (detached_ == false && future.valid() == true){
				wait();
			}
		}

		//! \param Other Zadanie którego zasoby przejmujemy
		JobImplementation& operator=(JobImplementation&& Other)
		{
			workManager = std::move(Other.workManager);
			sharedState = std::move((Other.sharedState);
			future = std::move(Other.future);
			detached_ = std::move(Other.detached_);
		}

		//! Operator przypisania kopiuj¹cego
		JobImplementation& operator=(const JobImplementation&) = delete;

		//! \param Other Inne zadanie z którym zamieniamy nasze zasoby
		void swap(JobImplementation & Other)
		{
			std::swap(workManager, Other.workManager);
			std::swap(sharedState, Other.sharedState);
			std::swap(future, Other.future);
			std::swap(detached_, Other.detached_);
		}

		virtual void start() override
		{
			if (sharedState == nullptr){
				throw std::runtime_error("No job assigned");
			}
			else if (workManager == nullptr){
				sharedState->status_ = Failed;
			}
			else if (sharedState->status_ != Initialized)
			{
				throw std::runtime_error("Job already started");
			}

			sharedState->status_ = Pending;

			try{
				future = workManager->submit([](utils::shared_ptr<SharedState> sharedState){

					{
						std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
						if (sharedState->status_ == Cancelled){
							throw std::runtime_error("Work canceled");
						}

						sharedState->status_ = Running;
					}
					try{
						T ret(sharedState->functionWrapper());
						std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
						sharedState->status_ = Finished;
						return ret;
					}
					catch (...){
						std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
						sharedState->status_ = Failed;
						throw;
					}
				}, sharedState);
			}
			catch (...){
				sharedState->status_ = Failed;
			}
		}

		virtual void wait() override
		{
			future.wait();
		}

		virtual void cancel() override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (cancellable() == true){
				sharedState->status_ = Cancelled;
			}
		}

		virtual const bool cancellable() const override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (sharedState == nullptr || sharedState->status_ == Cancelled || sharedState->status_ == Finished || sharedState->status_ == Failed || sharedState->status_ == Running){
				return false;
			}

			return true;
		}

		virtual void detach() override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (sharedState == nullptr || (sharedState != nullptr && (sharedState->status_ == Pending || sharedState->status_ == Running))){
				detached_ = true;
			}
		}

		template<class Rep, class Per>
		const std::future_status wait_for(
			const std::chrono::duration<Rep, Per>& relTime) const
		{	// wait for duration
			return future.wait_for(relTime);
		}

		template<class _Clock, class _Dur>
		const std::future_status wait_until(
			const std::chrono::time_point<_Clock, _Dur>& absTime) const
		{	// wait until time point
			return future.wait_until(absTime);
		}

		virtual const Status status() const override
		{
			return (sharedState == nullptr) ? Initialized : sharedState->status_;
		}

		const T& get()
		{
			return future.get();
		}

	private:
		WorkManager * workManager;
		bool detached_;
		typename WorkManager::FutureType<T&>::type future;
		utils::shared_ptr<SharedState> sharedState;
	};
	
	//! \tparam WorkManager Typ work managera któy bêdzie obs³ugiwa³ zadania	
	template<typename WorkManager>
	class JobImplementation<void, WorkManager> : public IJob
	{
		friend class JobManager<WorkManager>;

	private:

		//! Wspó³dzielony stan zadania
		struct SharedState
		{
			//! Wrapper opakowanego zadania
			std::function<void()> functionWrapper;
			//! Status zadania
			volatile Status status_;
			//! Obiekt synchronizuj¹cy status
			std::recursive_mutex satusMutex;
		};

	private:

		//! \tparam FuncitonType Typ funkcji zlecanej do wykonania w zadaniu			
		//! \param workManager Manager prac w ramach którego bêdzie realizowane zadanie
		//! \param f Funkcja do wykonania
		template<typename F, class ...Args>
		JobImplementation(WorkManager * workManager, F&& f, Args&& arguments...) : workManager(workManager),
			sharedState(utils::make_shared<SharedState>()), detached_(false)
		{
			sharedState->functionWrapper = std::bind(utils::decay_copy(std::forward<F>(f)), utils::decay_copy(std::forward<Args>(arguments))...);
			sharedState->status_ = Initialized;
		}

	public:

		//! Domyœlny konstruktor
		JobImplementation() : workManager(nullptr), detached_(false) {}

		//! Konstruktor przenosz¹cy w³asnoœæ
		JobImplementation(JobImplementation && Other) : workManager(std::move(Other.workManager)),
			sharedState(std::move((Other.sharedState)),
			future(std::move(Other.future)),
			detached_(std::move(Other.detached_))
		{

		}

		//! Konstruktor kopiuj¹cy
		JobImplementation(const JobImplementation & Other) = delete;

		//! Destruktor wirtualny - czeka na zakoñczenie zadanie je¿eli nie by³o ono od³¹czone i jest na co czekaæ (detach)
		virtual ~JobImplementation()
		{
			if (detached_ == false && future.valid() == true){
				wait();
			}
		}

		//! \param Other Zadanie którego zasoby przejmujemy
		JobImplementation& operator=(JobImplementation&& Other)
		{
			workManager = std::move(Other.workManager);
			sharedState = std::move((Other.sharedState);
			future = std::move(Other.future);
			detached_ = std::move(Other.detached_);
		}

		//! Operator przypisania kopiuj¹cego
		JobImplementation& operator=(const JobImplementation&) = delete;

		//! \param Other Inne zadanie z którym zamieniamy nasze zasoby
		void swap(JobImplementation & Other)
		{
			std::swap(workManager, Other.workManager);
			std::swap(sharedState, Other.sharedState);
			std::swap(future, Other.future);
			std::swap(detached_, Other.detached_);
		}


		virtual void start() override
		{
			if (sharedState == nullptr){
				throw std::runtime_error("No job assigned");
			}
			else if (workManager == nullptr){
				sharedState->status_ = Failed;
			}
			else if (sharedState->status_ != Initialized)
			{
				throw std::runtime_error("Job already started");
			}

			sharedState->status_ = Pending;

			try{
				future = workManager->submit([](utils::shared_ptr<SharedState> sharedState){

					{
						std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
						if (sharedState->status_ == Cancelled){
							throw std::runtime_error("Work canceled");
						}

						sharedState->status_ = Running;
					}
					try{
						sharedState->functionWrapper();
						std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
						sharedState->status_ = Finished;
					}
					catch (...){
						std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
						sharedState->status_ = Failed;
						throw;
					}
				}, sharedState);
			}
			catch (...){
				sharedState->status_ = Failed;
			}
		}

		virtual void wait() override
		{
			future.wait();
		}

		virtual void cancel() override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (cancellable() == true){
				sharedState->status_ = Cancelled;
			}
		}

		virtual bool cancellable() const override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (sharedState == nullptr || sharedState->status_ == Cancelled || sharedState->status_ == Finished || sharedState->status_ == Failed || sharedState->status_ == Running){
				return false;
			}

			return true;
		}

		virtual void detach() override
		{
			std::lock_guard<std::recursive_mutex> lock(sharedState->satusMutex);
			if (sharedState == nullptr || (sharedState != nullptr && (sharedState->status_ == Pending || sharedState->status_ == Running))){
				detached_ = true;
			}
		}

		template<class Rep, class Per>
		std::future_status wait_for(
			const std::chrono::duration<Rep, Per>& relTime) const
		{	// wait for duration
			return future.wait_for(relTime);
		}

		template<class _Clock, class _Dur>
		std::future_status wait_until(
			const std::chrono::time_point<_Clock, _Dur>& absTime) const
		{	// wait until time point
			return future.wait_until(absTime);
		}

		virtual Status status() const override
		{
			return (sharedState == nullptr) ? Initialized : sharedState->status_;
		}

		void get()
		{
			return future.get();
		}

	private:
		WorkManager * workManager;
		bool detached_;
		typename WorkManager::FutureType<void>::type future;
		utils::shared_ptr<SharedState> sharedState;
	};


	//! \tparam WorkManager Manager prac w ramach którego realizujemy zadania	
	template<typename WorkManager>
	class JobManager
	{
	public:

		template<typename T>
		using Job = JobImplementation < T, WorkManager > ;

	public:
		
		JobManager(WorkManager * workManager);
		JobManager(JobManager && Other);
		JobManager(const JobManager & Other) = delete;

		template<typename F, class ...Args>
		Job<typename std::result_of<F(Args...)>::type> create(F&& f, Args&& ...arguments)
		{
			return Job<typename std::result_of<F(Args...)>::type>(workManager, std::forward<F>(f), std::forward<Args>(arguments)...);
		}

		//! \tparam T Typ wyniku future
		//! \tparam FuturePolicy Typ future (shared | future)
		//! \param future Future na którego wykonanie czekamy realizuj¹c inne zadania		
		template<typename JobType>
		void waitForOtherJob(JobType & job)
		{
			workManager->waitForOtherTask(job.future);
		}

	private:
		WorkManager * workManager;
	};
}

#endif  //  HEADER_GUARD_THREADINGUTILS__JOBMANAGER_H__
