#include <hmdbserviceslib/AuthorizationWS.h>
#include "loglib/Logger.h"

namespace hmdbServices
{
	AuthorizationWS::AuthorizationWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	AuthorizationWS::~AuthorizationWS()
	{
	}

	bool AuthorizationWS::checkMyLogin() const
	{
		wsdlService->setOperation("CheckMyLogin");

		try{
			wsdlService->invoke(true);
		}
		catch (networkUtils::WSDLServiceException& e){
			//nie mam usera
			UTILS_LOG_INFO("User was not found. " << e.what());
			return false;
		}

		//jeżeli problem inny niż prawa do usług
		//rzucam dalej

		bool ret = false;

		try{
			wsdlService->getValue<bool>("CheckMyLoginResult", ret);
		}
		catch (std::runtime_error& e) {
			UTILS_LOG_INFO("Authorization error. " << e.what());
		}
		catch (...){
			UTILS_LOG_INFO("Unknown authorization error");
		}

		return ret;
	}

	std::string AuthorizationWS::listMyUserGroupsAssigned() const
	{
		wsdlService->setOperation("ListMyUserGroupsAssigned");
		wsdlService->invoke();

		return wsdlService->xmlResponse();
	}

	std::string AuthorizationWS::listUsers() const
	{
		wsdlService->setOperation("ListUsers");
		wsdlService->invoke();

		return wsdlService->xmlResponse();
	}

	std::string AuthorizationWS::getMyUserData() const
	{
		wsdlService->setOperation("GetMyUserData");
		wsdlService->invoke();

		return wsdlService->xmlResponse();
	}

	void AuthorizationWS::updateUserAccount(const std::string & login, const std::string & email,
		const std::string & pass, const std::string & newPass,
		const std::string & firstName, const std::string & lastName)
	{
		wsdlService->setOperation("UpdateUserAccount");
		wsdlService->setValue("login", login);
		wsdlService->setValue("email", email);
		wsdlService->setValue("pass", pass);
		wsdlService->setValue("newPass", newPass);
		wsdlService->setValue("firstName", firstName);
		wsdlService->setValue("lastName", lastName);
		wsdlService->invoke();
	}

	void AuthorizationWS::evokeGroupMembership(const std::string & grantedUserLogin,
		const std::string & groupName)
	{
		wsdlService->setOperation("EvokeGroupMembership");
		wsdlService->setValue("grantedUserLogin", grantedUserLogin);
		wsdlService->setValue("groupName", groupName);
		wsdlService->invoke();
	}

	void AuthorizationWS::revokeGroupMembership(const std::string & grantedUserLogin,
		const std::string & groupName)
	{
		wsdlService->setOperation("RevokeGroupMembership");
		wsdlService->setValue("grantedUserLogin", grantedUserLogin);
		wsdlService->setValue("groupName", groupName);
		wsdlService->invoke();
	}

	std::string AuthorizationWS::listSessionPrivileges(const int sessionID) const
	{
		wsdlService->setOperation("ListSessionPrivileges");
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->invoke();
		return wsdlService->xmlResponse();
	}

	void AuthorizationWS::grantSessionPrivileges(const std::string & grantedUserLogin,
		const int sessionID, const bool write)
	{
		wsdlService->setOperation("GrantSessionPrivileges");
		wsdlService->setValue("grantedUserLogin", grantedUserLogin);
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->setValue("write", write);
		wsdlService->invoke();
	}

	void AuthorizationWS::removeSessionPrivileges(const std::string & grantedUserLogin,
		const int sessionID)
	{
		wsdlService->setOperation("RemoveSessionPrivileges");
		wsdlService->setValue("grantedUserLogin", grantedUserLogin);
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->invoke();
	}

	void AuthorizationWS::alterSessionVisibility(const int sessionID, const bool isPublic,
		const bool isWritable)
	{
		wsdlService->setOperation("AlterSessionVisibility");
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->setValue("isPublic", isPublic);
		wsdlService->setValue("isWritable", isWritable);
		wsdlService->invoke();
	}
}