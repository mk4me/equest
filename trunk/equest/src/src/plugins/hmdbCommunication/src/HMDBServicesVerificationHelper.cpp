#include "CommunicationPCH.h"
#include "HMDBServicesVerificationHelper.h"
#include <hmdbserviceslib/IAccountFactoryWS.h>
#include <hmdbserviceslib/IAdministrationWS.h>
#include <hmdbserviceslib/IBasicQueriesWS.h>
#include <hmdbserviceslib/IBasicUpdatesWS.h>
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <hmdbserviceslib/IUserPersonalSpaceWS.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBFtp.h>

using namespace hmdbCommunication;

const bool HMDBServicesVerificationHelper::verify(hmdbServices::ISingleAccountFactoryWS * singleAccountFactory)
{
	bool ret = true;

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IMultiAccountFactoryWS * multiAccountFactory)
{
	bool ret = true;

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IAdministrationWS * administration)
{
	bool ret = true;

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IAuthorizationWS * authorization)
{
	bool ret = true;

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IGeneralBasicQueriesWS * generalQueries)
{
	bool ret = false;
	try{
		auto dmt = generalQueries->dataModificationTime();
		auto mdmt = generalQueries->metadataModificationTime();
		ret = true;
	}
	catch (...){

	}

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IMotionBasicQueriesWS * motionQueries)
{
	bool ret = false;
	try{
		auto dmt = motionQueries->dataModificationTime();
		auto mdmt = motionQueries->metadataModificationTime();
		ret = true;
	}
	catch (...){

	}

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IMotionBasicUpdatesWS * motionUpdate)
{
	bool ret = true;

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IMedicalBasicUpdatesWS * medicalUpdate)
{
	bool ret = true;

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IMotionFileStoremanWS * motionFilestoreman)
{
	bool ret = true;

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IMedicalFileStoremanWS * medicalFilestoreman)
{
	bool ret = true;

	return ret;
}

const bool HMDBServicesVerificationHelper::verify(hmdbServices::IUserPersonalSpaceWS * userPersonalSpace)
{
	bool ret = true;

	return ret;
}


const bool HMDBServicesVerificationHelper::verify(hmdbServices::IFileStoremanWS * fileStoreman, IHMDBFtp * ftp)
{
	bool ret = true;

	return ret;
}