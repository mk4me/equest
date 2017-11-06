/********************************************************************
	created:  2014/07/04	12:33:41
	filename: ITTransaction.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__
#define __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__

#include <utils/SmartPtr.h>
#include <mutex>

namespace threadingUtils
{
	//! \tparam T Typ dla którego realizujemy transakcjê
	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITWritableTransaction
	{
	public:

		using transaction_type = T;

		//! Typ wskanika do transakcji
		using TransactionPtr = SmartPtr<T>;

	public:
		//! Destruktor wirtualny
		virtual ~ITWritableTransaction() {}

		//! \return Transakcja
		virtual TransactionPtr transaction() = 0;
	};

	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITWritableTransactionObject : public virtual T, public ITWritableTransaction < T, SmartPtr>
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITWritableTransactionObject() {}
	};

	//! \tparam T Typ dla którego realizujemy transakcjê
	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITReadableTransaction
	{
	public:

		using transaction_type = T;

		//! Typ wskanika do sta³ej transakcji
		using TransactionConstPtr = SmartPtr<const T>;

	public:
		//! Destruktor wirtualny
		virtual ~ITReadableTransaction() {}

		//! \return Transakcja
		virtual TransactionConstPtr transaction() const = 0;
	};

	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITReadableTransactionObject : public virtual T, public ITReadableTransaction < T, SmartPtr >
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITReadableTransactionObject() {}
	};

	//! \tparam T Typ dla którego realizujemy transakcjê
	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITTransaction : public ITWritableTransaction<T, SmartPtr>, public ITReadableTransaction<T, SmartPtr>
	{
	public:

		using transaction_type = T;

	public:
		//! Destruktor wirtualny
		virtual ~ITTransaction() {}

		using ITWritableTransaction<T, SmartPtr>::transaction;
		using ITReadableTransaction<T, SmartPtr>::transaction;
	};

	//! \tparam T Typ dla którego realizujemy transakcjê
	template<typename T, template<typename> class SmartPtr = utils::shared_ptr>
	class ITTransactionObject : public virtual T, public ITTransaction<T, SmartPtr>
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITTransactionObject() {}
	};

	//! Forward declaration
	template<typename TransactionT, typename SharedStateT>
	class TransactionImplHelper;

	//! Struktura wspoldzielonego stanu przez transakcje - obejmuje wewnêtrzny
	//! stan transakcji oraz obiekt synchronizuj¹cy
	//! \tparam SharedStateT Wspóldzielony stan kontekstu transakcji
	//! \tparam MutexType Typ obiektu synchronizuj¹cego
	template<typename SharedStateT, class MutexType = std::recursive_mutex>
	struct TransactionSharedState
	{
		template<typename TransactionT, typename SharedStateTT>
		friend class TransactionImplHelper;

	private:
		//! Typ mutexa
		using mutex_type = MutexType;

	public:
		//! Typ wewnetrznego stanu
		using inner_shared_state_type = SharedStateT;

	public:
		//! Wewnêtrzny stan
		SharedStateT innerState;

	private:
		//! Obiekt synchronizuj¹cy
		MutexType sync;
	};

	//! Klasa pomocnicza przy implementacji transakcji
	//! automatycznie zak³ada i ci¹ga blokadê
	//! \tparam TransactionT Typ transakcji
	//! \tparam SharedStateT Typ wspó³dzielonego stanu
	template<typename TransactionT, typename SharedStateT>
	class TransactionImplHelper : public TransactionT
	{
	public:

		using ImplType = TransactionImplHelper<TransactionT, SharedStateT>;
		using SharedStatePtr = utils::shared_ptr<SharedStateT>;

	protected:

		TransactionImplHelper(SharedStatePtr sharedState)
			: sharedState_(sharedState), lock(sharedState_->sync) {}
		virtual ~TransactionImplHelper() {}

		inline typename SharedStateT::inner_shared_state_type & sharedState()
		{
			return sharedState_->innerState;
		}

		inline const typename SharedStateT::inner_shared_state_type & sharedState() const
		{
			return sharedState_->innerState;
		}

	protected:
		SharedStatePtr sharedState_;
	private:
		std::lock_guard<typename SharedStateT::mutex_type> lock;
	};
}

#define TRANSACTION(obj) obj->transaction()
#define TRANSACTION_PART __t
#define BEGIN_TRANSACTION(obj) { auto TRANSACTION_PART = TRANSACTION(obj);
#define END_TRANSACTION }

#endif	// __HEADER_GUARD_THREADINGUTILS__ITTRANSACTION_H__
