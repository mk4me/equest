/********************************************************************
	created:  2014/07/04	13:00:31
	filename: HMDBStatusManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSTATUSMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSTATUSMANAGER_H__

#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <mutex>
#include <map>

namespace hmdbCommunication
{

	class IHMDBStorage;

	class HMDBStatusManager : public IHMDBStatusManager
	{
	public:

		typedef std::map<hmdbServices::ID, DataStatus> StatusByID;
		typedef std::map<DataType, StatusByID> Statuses;
		typedef std::map<DataType, std::set<hmdbServices::ID>> UpdateData;

		struct SharedState
		{
			//! Statusy danych
			Statuses statuses;
			//! Mapa aktualizacji
			UpdateData updateData;
		};

		typedef threadingUtils::TransactionSharedState<SharedState> TransactionSharedState;
		typedef threadingUtils::TransactionImplHelper<IHMDBStatusManager::transaction_type, TransactionSharedState>::SharedStatePtr SharedStatePtr;

	public:
		HMDBStatusManager();
		virtual ~HMDBStatusManager();

		virtual TransactionPtr transaction() override;
		virtual TransactionConstPtr transaction() const override;

	private:
		
		SharedStatePtr sharedState;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSTATUSMANAGER_H__