#include "CommunicationPCH.h"
#include "HMDBSession.h"
#include "HMDBFTP.h"
#include "AccountFactoryWS.h"
#include "AdministrationWS.h"
#include "AuthorizationWS.h"
#include "BasicQueriesWS.h"
#include "BasicUpdatesWS.h"
#include "FileStoremanWS.h"
#include "UserPersonalSpaceWS.h"
#include <networkUtils/CURLManager.h>
#include "HMDBService.h"

using namespace hmdbCommunication;

const WSDLServiceCreator HMDBSession::serviceCreator(
	utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
	const std::string & user,
	const std::string & password,
	const utils::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const utils::Filesystem::Path & schemaPath)
{
	if (CAPath.empty() == false){
		return std::bind(&HMDBService::createSecureWSDL, executor, url, user, password, CAPath, hostVerification, schemaPath);
	}else{
		return std::bind(&HMDBService::createUnsecureWSDL, executor, url, user, password, schemaPath);
	}
}

const WSDLServiceCreator HMDBSession::systemServiceCreator(
	utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
	const utils::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification,
	const utils::Filesystem::Path & schemaPath)
{
	return serviceCreator(executor, url, "hmdbServiceUser", "4accountCreation", CAPath, hostVerification);
}

HMDBSession::HMDBSession(utils::shared_ptr<CURLFTPManager> ftpManager,
	networkUtils::CURLManagerPtr wsdlManager,
	utils::shared_ptr<XmlUtils::CURLExecutor> wsdlExecutor,
	const std::string & user,
	const std::string & password,
	const std::string & motionUrl,
	const std::string & medicalUrl,
	const std::string & motionFtp,
	const std::string & medicalFtp,
	const utils::Filesystem::Path & CAPath,
	const networkUtils::SSLHostVerification hostVerification)
	: user_(user), motionUrl_(motionUrl),
	medicalUrl_(medicalUrl), motionFtpUrl_(motionFtp),
	medicalFtpUrl_(medicalFtp),
	ftpManager_(ftpManager),
	wsdlManager_(wsdlManager),
	motionAccountFactory_(motionUrl.empty() == false ? new SingleAccountFactoryWS(serviceCreator(wsdlExecutor, motionUrl_ + "/AccountFactoryWS.svc?wsdl", "hmdbServiceUser", "4accountCreation", CAPath, hostVerification)) : nullptr),
	medicalAccountFactory_(medicalUrl.empty() == false ? new SingleAccountFactoryWS(serviceCreator(wsdlExecutor, medicalUrl_ + "/AccountFactoryWS.svc?wsdl", "hmdbServiceUser", "4accountCreation", CAPath, hostVerification)) : nullptr),
	multiAccountFactory_((motionUrl.empty() == false && medicalUrl.empty() == false) ? new MultiAccountFactoryWS(serviceCreator(wsdlExecutor, medicalUrl_ + "/AccountFactoryWS.svc?wsdl", "hmdbServiceUser", "4accountCreation", CAPath, hostVerification)) : nullptr),
	administration_(motionUrl.empty() == false ? new AdministrationWS(serviceCreator(wsdlExecutor, motionUrl_ + "/AdministrationWS.svc?wsdl", user, password, CAPath, hostVerification)) : nullptr),
	authorization_(motionUrl.empty() == false ? new AuthorizationWS(serviceCreator(wsdlExecutor, motionUrl_ + "/AuthorizationWS.svc?wsdl", user, password, CAPath, hostVerification)) : nullptr),
	motionBasicQueries_(motionUrl.empty() == false ? new MotionBasicQueriesWS(serviceCreator(wsdlExecutor, motionUrl_ + "/res/BasicQueriesWSStandalone.wsdl", user, password, CAPath, hostVerification)) : nullptr),
	medicalBasicQueries_(medicalUrl.empty() == false ? new MedicalBasicQueriesWS(serviceCreator(wsdlExecutor, medicalUrl_ + "/res/BasicQueriesWSStandalone.wsdl", user, password, CAPath, hostVerification)) : nullptr),
	motionBasicUpdate_(motionUrl.empty() == false ? new MotionBasicUpdatesWS(serviceCreator(wsdlExecutor, motionUrl_ + "/BasicUpdatesWS.svc?wsdl", user, password, CAPath, hostVerification)) : nullptr),
	medicalBasicUpdate_(medicalUrl.empty() == false ? new MedicalBasicUpdatesWS(serviceCreator(wsdlExecutor, medicalUrl_ + "/BasicUpdatesWS.svc?wsdl", user, password, CAPath, hostVerification)) : nullptr),
	motionFileStoreman_(motionUrl.empty() == false ? new MotionFileStoremanWS(serviceCreator(wsdlExecutor, motionUrl_ + "/FileStoremanWS.svc?wsdl", user, password, CAPath, hostVerification)) : nullptr),
	medicalFileStoreman_(medicalUrl.empty() == false ? new MedicalFileStoremanWS(serviceCreator(wsdlExecutor, medicalUrl_ + "/FileStoremanWS.svc?wsdl", user, password, CAPath, hostVerification)) : nullptr),
	userPersonalSpace_(motionUrl.empty() == false ? new UserPersonalSpaceWS(serviceCreator(wsdlExecutor, motionUrl_ + "/res/UserPersonalSpaceWSStandalone.wsdl", user, password, CAPath, hostVerification)) : nullptr),
	motionFtp_(motionFtp.empty() == false ? new HMDBFTP("testUser", "testUser", motionFtpUrl_, ftpManager) : nullptr),
	medicalFtp_(medicalFtp.empty() == false ? new HMDBFTP("testUser", "testUser", medicalFtpUrl_, ftpManager) : nullptr)
{

}

HMDBSession::~HMDBSession()
{

}

const std::string HMDBSession::motionServicesUrl() const
{
	return motionUrl_;
}

const std::string HMDBSession::medicalServicesUrl() const
{
	return medicalUrl_;
}

const std::string HMDBSession::motionFtpUrl() const
{
	return motionFtpUrl_;
}

const std::string HMDBSession::medicalFtpUrl() const
{
	return medicalFtpUrl_;
}

const std::string HMDBSession::userName() const
{
	return user_;
}

hmdbServices::ISingleAccountFactoryWS * HMDBSession::motionAccountFactory()
{
	return motionAccountFactory_.get();
}

hmdbServices::ISingleAccountFactoryWS * HMDBSession::medicalAccountFactory()
{
	return medicalAccountFactory_.get();
}

hmdbServices::IMultiAccountFactoryWS * HMDBSession::multiAccountFactory()
{
	return multiAccountFactory_.get();
}

hmdbServices::IAdministrationWS * HMDBSession::administration()
{
	return administration_.get();
}

hmdbServices::IAuthorizationWS * HMDBSession::authorization()
{
	return authorization_.get();
}

hmdbServices::IMotionBasicQueriesWS * HMDBSession::motionQueries()
{
	return motionBasicQueries_.get();
}

hmdbServices::IGeneralBasicQueriesWS * HMDBSession::medicalQueries()
{
	return medicalBasicQueries_.get();
}

hmdbServices::IMotionBasicUpdatesWS * HMDBSession::motionUpdate()
{
	return motionBasicUpdate_.get();
}

hmdbServices::IMedicalBasicUpdatesWS * HMDBSession::medicalUpdate()
{
	return medicalBasicUpdate_.get();
}

hmdbServices::IMotionFileStoremanWS * HMDBSession::motionFilestoreman()
{
	return motionFileStoreman_.get();
}

hmdbServices::IMedicalFileStoremanWS * HMDBSession::medicalFilestoreman()
{
	return medicalFileStoreman_.get();
}

hmdbServices::IUserPersonalSpaceWS * HMDBSession::userPersonalSpace()
{
	return userPersonalSpace_.get();
}

IHMDBFtp * HMDBSession::motionFtp()
{
	return motionFtp_.get();
}

IHMDBFtp * HMDBSession::medicalFtp()
{
	return medicalFtp_.get();
}