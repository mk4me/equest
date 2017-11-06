/********************************************************************
	created:  2014/05/28	21:55:20
	filename: CURLFTPTransferData.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__

#include <networkUtils/CURLManager.h>
#include <networkUtils/CURLUtils.h>
#include "CURLFTPStatus.h"
#include "CURLFTPProgress.h"
#include <plugins/hmdbCommunication/IHMDBStorage.h>

namespace hmdbCommunication
{
	//! Struktura opisująca transfer
	struct CURLFTPTransferSharedState
	{
		//! Obiekt statusu
		CURLFTPStatus status;
		//! Uchwyt transferu
		networkUtils::CURLPtr curl;
	};

	struct CURLFTPTransferData
	{
		//! Obiekt do czekania
		std::future<CURLcode> result;
		//! Obiekt postępu
		CURLFTPProgress progress;
		//! Obiekt opisujący podstawy transferu
		utils::shared_ptr<CURLFTPTransferSharedState> sharedState;

		virtual void resetStream() = 0;
	};

	//! Opis uploadu
	struct CURLFTPUploadTransferData : public CURLFTPTransferData
	{
		//! Strumień źródłowy
		IHMDBStorageOperations::IStreamPtr stream;

		virtual void resetStream() override { stream.reset(); }
	};

	//! Opis downloadu
	struct CURLFTPDownloadTransferData : public CURLFTPTransferData
	{
		//! Strumień docelowy
		IHMDBStorageOperations::OStreamPtr stream;

		virtual void resetStream() override { stream.reset(); }
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPTRANSFERDATA_H__
