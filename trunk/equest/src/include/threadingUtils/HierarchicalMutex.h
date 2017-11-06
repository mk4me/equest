/********************************************************************
	created:  2014/10/02	09:15:47
	filename: HierarchicalMutex.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__HIERARCHICALMUTEX_H__
#define __HEADER_GUARD_THREADINGUTILS__HIERARCHICALMUTEX_H__

#include <limits>
#include <stdexcept>
#include <threadingUtils/Macros.h>

namespace threadingUtils
{
	//! Typ mutexa hierarchicznego, gdzie kontroluje siê hierarchiê lokowania
	//! \tparam MutexType Typ mutexa u¿ywanego w hierarchii
	template<typename MutexType>
	class HierarchicalMutex
	{
	public:
		//! Typ opisuj¹cy poziom blokady
		using HierarchyLevelType = std::size_t;

	private:
		//! Wewnêtrzny mutex
		MutexType mutex;
		//! Aktualna wartoœæ hierarchi mutexów
		const HierarchyLevelType hierarchyValue;
		//! Poprzednia wartoœæ hierarchi mutexów
		HierarchyLevelType previousHierarchyValue;
		//! Wartoœæ hierarchi mutexów tego w¹tku
		static thread_local HierarchyLevelType threadHierarchyValue;

	private:

		//! Metoda sprawdza czy nie pogwa³cono zasad dla hierarchi mutexów
		void checkForHierarchyViolation()
		{
			if (threadHierarchyValue <= hierarchyValue)
			{
				throw std::logic_error("mutex hierarchy violated");
			}
		}

		//! Metoda aktualizuje informacje o aktualnej hierarchi mutexów
		void updateHierarchyValue()
		{
			previousHierarchyValue = threadHierarchyValue;
			threadHierarchyValue = hierarchyValue;
		}

	public:
		//! Konstruktor
		//! \param Wartoœæ hierarchii mutexa dla aktualnego w¹tku
		explicit HierarchicalMutex(const HierarchyLevelType value) : hierarchyValue(value), previousHierarchyValue(0) {}
		//! Destruktor
		~HierarchicalMutex() {}
		//! Lock mutexa
		void lock()
		{
			checkForHierarchyViolation();
			mutex.lock();
			updateHierarchyValue();
		}
		//! Unlock mutexa
		void unlock()
		{
			threadHierarchyValue = previousHierarchyValue;
			mutex.unlock();
		}
		//! Próba lokowania mutexa
		//! \return Prawda jeœli uda³o siê zrobiæ lock na mutexie
		bool try_lock()
		{
			checkForHierarchyViolation();
			if (!mutex.try_lock())
				return false;
			updateHierarchyValue();
			return true;
		}
	};

	//! Inicjalizacja lokalnych danych w¹tku na potrzeby obs³ugi mutexów hierarchicznych
	template<typename MutexType>
	thread_local HierarchyLevelType HierarchicalMutex<MutexType>::threadHierarchyValue = std::numeric_limits<HierarchyLevelType>::max();
}

#endif	// __HEADER_GUARD_THREADINGUTILS__HIERARCHICALMUTEX_H__