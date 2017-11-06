/********************************************************************
	created:  2014/05/28	17:47:47
	filename: CURLFTPManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPMANAGER_H__

#include <networkUtils/CURLManager.h>
#include "CURLFTPTransferData.h"

namespace hmdbCommunication
{
	class CURLFTPManager
	{
		class StateCURLManager;		

	public:
		//! Domyœlny konstruktor
		CURLFTPManager();
		//! \param multi Transfer multi
		explicit CURLFTPManager(networkUtils::CURLMPtr multi);
		//! Desturktor wirtualny
		virtual ~CURLFTPManager();

		//! \param curl Skonfigurowany uchwyt do obs³u¿enia
		//! \param wait Obiekt pozwalaj¹cy zaczekaæ na zakoñæenie operacji curla
		std::future<CURLcode> addRequest(utils::shared_ptr<CURLFTPTransferSharedState> transfer);
		//! \param curl Uchwyt do usuniêcia
		void removeRequest(networkUtils::CURLPtr curl);
		//! Przetwarza zapytania
		void run();
		//! Koñczy potencjalne przetwarzanie
		void finalize();

	private:
		//! Obiekt realizujacy ca³a funkcjonalnoœæ
		utils::shared_ptr<StateCURLManager> curlManager;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPMANAGER_H__