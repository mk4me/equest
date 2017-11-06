#include "CommunicationPCH.h"
#include "CURLFTPManager.h"
#include "CURLFTPTransferData.h"
#include <map>

class hmdbCommunication::CURLFTPManager::StateCURLManager : public networkUtils::CURLManager
{
public:

	//! Domyœlny konstruktor
	StateCURLManager() : networkUtils::CURLManager() {}
	//! \param multi Uchwyt dla obs³ugiwanych po³¹czeñ
	explicit StateCURLManager(networkUtils::CURLMPtr multi)
		: networkUtils::CURLManager(multi) {}
	//! Destruktor wirtualny
	virtual ~StateCURLManager() {}

	//! \param curl Skonfigurowany uchwyt do obs³u¿enia
	//! \param wait Obiekt pozwalaj¹cy zaczekaæ na zakoñæenie operacji curla
	std::future<CURLcode> addRequest(utils::shared_ptr<CURLFTPTransferSharedState> transfer)
	{
		transfers.insert({transfer->curl, transfer});
		try{
			return networkUtils::CURLManager::addRequest(transfer->curl);
		}
		catch (...){
			transfers.erase(transfer->curl);
			throw;
		}
	}

private:
	//! \param curl Uchwyt który w³aœnie dodano
	virtual void onAddRequest(networkUtils::CURLPtr curl) override
	{		
		transfers.find(curl)->second->status.setStatus(threadingUtils::IOperation::Running);
	}

	//! \param curl Uchwyt który w³aœnie usuniêto
	virtual void onRemoveRequest(networkUtils::CURLPtr curl) override
	{
		onCancelRequest(curl);
	}

	//! \param curl Uchwyt który zosta³ anulowany
	virtual void onCancelRequest(networkUtils::CURLPtr curl) override
	{
		auto it = transfers.find(curl);
		it->second->status.setStatus(threadingUtils::IOperation::Aborted);
		transfers.erase(it);
	}

	//! \param curl Uchwyt który spowodowa³ b³¹d
	//! \param error Opis b³edu
	virtual void onErrorRequest(networkUtils::CURLPtr curl, const std::string & error) override
	{
		auto it = transfers.find(curl);
		it->second->status.setError(error);
		it->second->status.setStatus(threadingUtils::IOperation::Error);
		transfers.erase(it);

	}
	//! \param curl Uchwyt który zakoñczy³ przetwarzanie danych poprawnie
	virtual void onFinishRequest(networkUtils::CURLPtr curl) override
	{
		auto it = transfers.find(curl);
		it->second->status.setStatus(threadingUtils::IOperation::Finished);
		transfers.erase(it);
	}

private:

	std::map<networkUtils::CURLPtr, utils::shared_ptr<CURLFTPTransferSharedState>> transfers;
};

using namespace hmdbCommunication;

CURLFTPManager::CURLFTPManager() : curlManager(new StateCURLManager)
{

}

CURLFTPManager::CURLFTPManager(networkUtils::CURLMPtr multi) : curlManager(new StateCURLManager(multi))
{

}

CURLFTPManager::~CURLFTPManager()
{

}

std::future<CURLcode> CURLFTPManager::addRequest(utils::shared_ptr<CURLFTPTransferSharedState> transfer)
{
	return curlManager->addRequest(transfer);
}

void CURLFTPManager::removeRequest(networkUtils::CURLPtr curl)
{
	curlManager->removeRequest(curl);
}

void CURLFTPManager::run()
{
	curlManager->run();
}

void CURLFTPManager::finalize()
{
	curlManager->finalize();
}