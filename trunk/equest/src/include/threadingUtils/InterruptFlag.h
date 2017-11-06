/********************************************************************
	created:  2014/10/22	07:34:27
	filename: InterruptFlag.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTFLAG_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTFLAG_H__

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <threadingUtils/Macros.h>
#include <threadingUtils/Export.h>

namespace threadingUtils
{
	//! Klasa pomocnicza przy realizacji funkcjonalności bezpiecznie przerywalnych wątków
	class THREADINGUTILS_EXPORT InterruptFlag
	{
	private:
		//! Flaga czy wątek faktycznie był przerwany
		std::atomic<bool> flag;
		//! Zmienna warunkowa pozwalająca przerywać oczekiwanie
		std::condition_variable * threadCond;
		//! Zmienna warunkowa pozwalająca przerywać oczekiwanie własnych struktur synchronizacyjnych
		std::condition_variable_any * threadCondAny;
		//! Obiekt synchronizujący
		std::mutex setClearMutex;

	public:

		//! Struktura pomocnicza przy zwalnianiu zmiennej warunkowej w przypadku wyjątku przerwania
		struct ClearConditionVariableOnDestruct
		{
			//! Destruktor
			~ClearConditionVariableOnDestruct()
			{
				threadInterruptFlag()->clearConditionVariable();
			}
		};

		//! \tparam Lockable Typ obiektu synchronizującego
		//! Klasa wpiera funkcjonalnośc przerywania oczekiwania na customowych obiektach synchronizujących
		template<typename Lockable>
		class CustomLock
		{
		private:
			//! Flaga synchronizująca
			InterruptFlag * self;
			//! Obiekt synchronizujący
			Lockable & lk;

		public:
			//! RAII
			//! \param self Flaga synchronizująca
			//! \param cond Zmienna warunkowa
			//! \param lk Obiekt synchronizujący
			CustomLock(InterruptFlag * self_,
				std::condition_variable_any & cond,
				Lockable & lk) :
				self(self_), lk(lk)
			{
				self->setClearMutex.lock();
				self->threadCondAny = &cond;
			}

			//! Zwalnia obiekt synchronizujący
			void unlock()
			{
				lk.unlock();
				self->setClearMutex.unlock();
			}

			//! Blokuje obiekt synchronizujący
			void lock()
			{
				std::lock(self->setClearMutex, lk);
			}

			//! Destruktor
			//! Przywraca stan zasobów flagi
			~CustomLock()
			{
				self->threadCondAny = nullptr;
				self->setClearMutex.unlock();
			}
		};

		template<typename Lockable>
		friend class CustomLock;

	public:

		//! Konstruktor domyślny
		InterruptFlag();
		//! Destruktor
		~InterruptFlag();

		//! Ustawia flagę przerwania
		void set();

		//! Resetuje flagę przerwania
		void reset();

		//! \return Czy flaga przerwania jest ustawiona
		bool isSet() const;

		//! \return Czy flaga aktualnego wątku jest ustawiona
		static bool threadIsSet();

		//! Resetuje flage przerwania aktualnego wątku
		static void threadReset();

		//! \return Wskaźnik flagi przerwania aktualnego wątku
		static InterruptFlag *& threadInterruptFlag();

		//! \param cv Zmienna warunkowa
		void setConditionVariable(std::condition_variable & cv);

		//! Zeruje zmienną warunkową
		void clearConditionVariable();
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTFLAG_H__
