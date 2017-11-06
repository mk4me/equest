#include "CommunicationPCH.h"
#include "HMDBFTP.h"
#include <networkUtils/CURLFTPHelper.h>
#include "CURLFTPManager.h"
#include "CURLFTPTransfer.h"
#include "CURLFTPProgress.h"
#include "CURLFTPTransferData.h"
#include "HMDBService.h"

using namespace hmdbCommunication;
using namespace networkUtils;

HMDBFTP::HMDBFTP(const std::string & user, const std::string & password,
	const std::string & url,
	utils::shared_ptr<CURLFTPManager> manager)
	: user(user), password(password), url(url), manager(manager)
{

}

HMDBFTP::~HMDBFTP()
{

}

HMDBFTP::TransferPtr HMDBFTP::preparePut(const std::string & destinationFileName,
	IHMDBStorageOperations::IStreamPtr stream, unsigned long long size)
{
	HMDBFTP::TransferPtr ret;

	auto m = manager.lock();

	if (m == nullptr){
		return ret;
	}

	auto curl = networkUtils::createCurlEasy();
	if (curl != nullptr){

		utils::shared_ptr<CURLFTPUploadTransferData> td(new CURLFTPUploadTransferData);
		td->stream = stream;
		td->sharedState = utils::make_shared<CURLFTPTransferSharedState>();
		td->sharedState->curl = curl;
		CURLFTPHelper::initializeFTPConnection(curl.get(), user, password);
		CURLFTPHelper::initializeFTPUpload(curl.get(), url + "/" + destinationFileName, td->stream.get(), &(td->progress));
		curl_easy_setopt(curl.get(), CURLOPT_INFILESIZE, size);
		curl_easy_setopt(curl.get(), CURLOPT_INFILESIZE_LARGE, size);
		//HMDBService::curlEnableLog(curl.get());
		ret.reset(new CURLFTPTransfer(CURLFTPTransfer::Upload, m, td, destinationFileName, size));
	}

	return ret;
}

HMDBFTP::TransferPtr HMDBFTP::prepareGet(const std::string & destinationFileName,
	IHMDBStorageOperations::OStreamPtr stream, unsigned long long size)
{
	HMDBFTP::TransferPtr ret;

	auto m = manager.lock();

	if (m == nullptr){
		return ret;
	}

	auto curl = networkUtils::createCurlEasy();
	if (curl != nullptr){

		utils::shared_ptr<CURLFTPDownloadTransferData> td(new CURLFTPDownloadTransferData);
		td->stream = stream;
		td->sharedState = utils::make_shared<CURLFTPTransferSharedState>();
		td->sharedState->curl = curl;
		CURLFTPHelper::initializeFTPConnection(curl.get(), user, password);
		CURLFTPHelper::initializeFTPDownload(curl.get(), url + "/" + destinationFileName, td->stream.get(), &(td->progress));
		ret.reset(new CURLFTPTransfer(CURLFTPTransfer::Download, m, td, destinationFileName, size));
	}

	return ret;
}