#include <hmdbserviceslib/AccountFactoryWS.h>

namespace hmdbServices {
	SingleAccountFactoryWS::SingleAccountFactoryWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	SingleAccountFactoryWS::~SingleAccountFactoryWS()
	{
	}

	void SingleAccountFactoryWS::createUserAccount(const std::string & login,
		const std::string & email, const std::string & password,
		const std::string & firstName, const std::string & lastName)
	{
		wsdlService->setOperation("CreateUserAccount");
		wsdlService->setValue("login", login);
		wsdlService->setValue("email", email);
		wsdlService->setValue("pass", password);
		wsdlService->setValue("firstName", firstName);
		wsdlService->setValue("lastName", lastName);
		wsdlService->invoke();
	}

	bool SingleAccountFactoryWS::activateUserAccount(const std::string & login,
		const std::string & activationCode)
	{
		wsdlService->setOperation("ActivateUserAccount");
		wsdlService->setValue("login", login);
		wsdlService->setValue("activationCode", activationCode);
		wsdlService->invoke(true);

		bool ret = false;
		wsdlService->getValue("ActivateUserAccountResult", ret);

		return ret;
	}

	bool SingleAccountFactoryWS::resetPassword(const std::string & email)
	{
		wsdlService->setOperation("ResetPassword");
		wsdlService->setValue("email", email);
		wsdlService->invoke(true);

		bool ret = false;
		wsdlService->getValue("ResetPasswordResult", ret);

		return ret;
	}

	//Multi

	MultiAccountFactoryWS::MultiAccountFactoryWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	MultiAccountFactoryWS::~MultiAccountFactoryWS()
	{
	}

	void MultiAccountFactoryWS::createUserAccount(const std::string & login,
		const std::string & email, const std::string & password,
		const std::string & firstName, const std::string & lastName,
		const bool propagateToHMDB)
	{
		wsdlService->setOperation("CreateUserAccount");
		wsdlService->setValue("login", login);
		wsdlService->setValue("email", email);
		wsdlService->setValue("pass", password);
		wsdlService->setValue("firstName", firstName);
		wsdlService->setValue("lastName", lastName);
		wsdlService->setValue("propagateToHMDB", propagateToHMDB);
		wsdlService->invoke();
	}

	bool MultiAccountFactoryWS::activateUserAccount(const std::string & login,
		const std::string & activationCode, const bool propagateToHMDB)
	{
		wsdlService->setOperation("ActivateUserAccount");
		wsdlService->setValue("login", login);
		wsdlService->setValue("activationCode", activationCode);
		wsdlService->setValue("propagateToHMDB", propagateToHMDB);
		wsdlService->invoke(true);

		bool ret = false;
		wsdlService->getValue("ActivateUserAccountResult", ret);

		return ret;
	}

	bool MultiAccountFactoryWS::resetPassword(const std::string & email,
		const bool propagateToHMDB)
	{
		wsdlService->setOperation("ResetPassword");
		wsdlService->setValue("propagateToHMDB", propagateToHMDB);
		wsdlService->setValue("email", email);
		wsdlService->invoke(true);

		bool ret = false;
		wsdlService->getValue("ResetPasswordResult", ret);

		return ret;
	}
}