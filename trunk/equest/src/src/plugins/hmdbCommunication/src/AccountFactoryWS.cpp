#include "AccountFactoryWS.h"
#include <hmdbserviceslib/AccountFactoryWS.h>

using namespace hmdbCommunication;

SingleAccountFactoryWS::SingleAccountFactoryWS(const WSDLServiceCreator creator)
	: creator(creator)
{

}

SingleAccountFactoryWS::~SingleAccountFactoryWS()
{

}

void SingleAccountFactoryWS::createUserAccount(const std::string & login,
	const std::string & email, const std::string & password,
	const std::string & firstName, const std::string & lastName)
{
	hmdbServices::SingleAccountFactoryWS(creator()).createUserAccount(login,
		email, password, firstName, lastName);
}

bool SingleAccountFactoryWS::activateUserAccount(const std::string & login,
	const std::string & activationCode)
{
	return hmdbServices::SingleAccountFactoryWS(creator()).activateUserAccount(login,
		activationCode);
}

bool SingleAccountFactoryWS::resetPassword(const std::string & email)
{
	return hmdbServices::SingleAccountFactoryWS(creator()).resetPassword(email);
}

//Multi

MultiAccountFactoryWS::MultiAccountFactoryWS(const WSDLServiceCreator creator)
	: creator(creator)
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
	hmdbServices::MultiAccountFactoryWS(creator()).createUserAccount(login,
		email, password, firstName, lastName, propagateToHMDB);
}

bool MultiAccountFactoryWS::activateUserAccount(const std::string & login,
	const std::string & activationCode, const bool propagateToHMDB)
{
	return hmdbServices::MultiAccountFactoryWS(creator()).activateUserAccount(login,
		activationCode, propagateToHMDB);
}

bool MultiAccountFactoryWS::resetPassword(const std::string & email,
	const bool propagateToHMDB)
{
	return hmdbServices::MultiAccountFactoryWS(creator()).resetPassword(email,
		propagateToHMDB);
}
