/********************************************************************
	created:  2015/08/11	10:43:01
	filename: Thread.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__THREAD_H__
#define __HEADER_GUARD_CORELIB__THREAD_H__

#include <thread>
#include <string>
#include <functional>
#include <boost/thread/tss.hpp>
#include <utils/SmartPtr.h>
#include <corelib/Export.h>
#include <corelib/ThreadTypes.h>

namespace core
{
	//! Klasa wątku oferowanego z puli
	class CORELIB_EXPORT Thread
	{
	private:

		//! Struktura opisująca wątek
		struct Description
		{
			//! Kto zarządza wątkiem
			std::string owner;
			//! Co jest realizowane w wątku
			std::string destination;
		};

		//! Dane specyficzne dla wątku
		typedef boost::thread_specific_ptr<utils::shared_ptr<Description>> TLS;

		//! RAII pilnujące opisu wątku
		struct CORELIB_EXPORT DescriptionLocalGuard
		{
			DescriptionLocalGuard(utils::shared_ptr<Description> td);

			~DescriptionLocalGuard();
		};

		friend struct DescriptionLocalGuard;	

	public:

		using InterruptiblePolicy = InnerThreadPool::Thread::InterruptiblePolicy;

	public:

		//! \param threadPool Pula wątków do której należał pierwotnie wątek
		template<typename T>
		Thread(const T & logger, InnerThreadPool::Thread && innerThread, const std::string & owner,
			const std::string & destination) : logger(logger), innerThread(std::move(innerThread)),
			description(std::make_shared<Description>())
		{
			description->owner = owner;
			description->destination = destination;
		}

		//! Domyślny konstruktor
		Thread();

		//! \param Other Wątek którego zasoby przejmujemy
		Thread(Thread && Other);

		//! Destruktor
		~Thread();

		Thread& operator=(Thread&& Other) NOEXCEPT;
		Thread& operator=(const Thread&) = delete;

		template<typename F, class ...Args>
		void run(F&& f, Args&& ...arguments)
		{
			std::function<void()> ff = std::bind(utils::decay_copy(std::forward<F>(f)),
				utils::decay_copy(std::forward<Args>(arguments))...);

			this->innerThread.run([ff, this](utils::shared_ptr<Description> desc)
			{
				DescriptionLocalGuard dlg(desc);
				try{
					ff();
				}
				catch (std::exception & e){
					this->logger(e.what());
					throw;
				}
				catch (...){
					this->logger("UNKNOWN error");
					throw;
				}
			}, description);
		}

		void swap(Thread& Other) NOEXCEPT;
		const bool joinable() const NOEXCEPT;
		void join();
		void detach();
		std::thread::id get_id() const NOEXCEPT;
		std::thread::native_handle_type native_handle();
		void interrupt();
		const bool interruptible() const;
		static void interruptionPoint();
		static void resetInterruption();
		std::string owner() const;
		std::string destination() const;

		static std::string currentOwner();
		static std::string currentDestination();

	private:
		//! Obiekt logujący
		std::function<void(std::string)> logger;
		//! Wewnętrzny wątek wielokrotnego uruchamiania
		InnerThreadPool::Thread innerThread;
		//! Opis wątku
		utils::shared_ptr<Description> description;
		//! Opis wątku
		static TLS tlsTD;
	};
}

#endif	// __HEADER_GUARD_CORELIB__THREAD_H__