/********************************************************************
	created:  2014/07/07	12:33:04
	filename: HMDBLocalContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBLOCALCONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBLOCALCONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IDataManager.h>
#include <corelib/IStreamDataManager.h>

namespace hmdbCommunication
{

	class HMDBLocalContext : public IHMDBLocalContext, public core::IDataManagerReader::IObserver
	{
	public:

		struct SharedState
		{
			//! kontekst danych
			IHMDBDataContextPtr dataContext_;
			//! Manager danych
			core::IDataManager * mdm;
			//! Manager strumieni
			core::IStreamDataManager * sdm;
			//! Zbiór danych którym zarz¹dzam
			std::list<core::VariantConstPtr> myData_;
		};

		typedef threadingUtils::TransactionSharedState<SharedState> TransactionSharedState;
		typedef threadingUtils::TransactionImplHelper<IHMDBLocalContext::transaction_type, TransactionSharedState>::SharedStatePtr SharedStatePtr;

	public:

		//! \param dataContext Kontekst dancyh z dostepem do storage
		//! \param mdm MemoryManager
		//! \param sdm StreaManager
		HMDBLocalContext(IHMDBDataContextPtr dataContext, core::IDataManager * mdm,
			core::IStreamDataManager * sdm);

		//! Destruktor wirtualny
		virtual ~HMDBLocalContext();

		virtual void observe(const core::IDataManagerReader::ChangeList & changes) override;

	//! ------------ Opis kontekstu ---------------------------

		//! \return Kontekst danych
		virtual const IHMDBDataContextPtr dataContext() override;
		//! \return Kontekst danych
		virtual const IHMDBDataContextConstPtr dataContext() const override;

	//! ------------- Operacje --------------------------------

		virtual TransactionPtr transaction() override;

		virtual TransactionConstPtr transaction() const override;

	private:
		//! kontekst danych
		SharedStatePtr sharedState;
	};

}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBLOCALCONTEXT_H__