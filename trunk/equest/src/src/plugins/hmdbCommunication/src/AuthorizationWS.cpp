#include "AuthorizationWS.h"
#include <hmdbserviceslib/AuthorizationWS.h>

using namespace hmdbCommunication;

AuthorizationWS::AuthorizationWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

AuthorizationWS::~AuthorizationWS()
{
}

bool AuthorizationWS::checkMyLogin() const
{
	return hmdbServices::AuthorizationWS(creator()).checkMyLogin();
}

std::string AuthorizationWS::listMyUserGroupsAssigned() const
{
	return hmdbServices::AuthorizationWS(creator()).listMyUserGroupsAssigned();
}

std::string AuthorizationWS::listUsers() const
{
	return hmdbServices::AuthorizationWS(creator()).listUsers();
}

std::string AuthorizationWS::getMyUserData() const
{
	return hmdbServices::AuthorizationWS(creator()).getMyUserData();
}

void AuthorizationWS::updateUserAccount(const std::string & login, const std::string & email,
	const std::string & pass, const std::string & newPass,
	const std::string & firstName, const std::string & lastName)
{
	hmdbServices::AuthorizationWS(creator()).updateUserAccount(login,
		email, pass, newPass, firstName, lastName);
}

void AuthorizationWS::evokeGroupMembership(const std::string & grantedUserLogin,
	const std::string & groupName)
{
	hmdbServices::AuthorizationWS(creator()).evokeGroupMembership(grantedUserLogin,
		groupName);
}

void AuthorizationWS::revokeGroupMembership(const std::string & grantedUserLogin,
	const std::string & groupName)
{
	hmdbServices::AuthorizationWS(creator()).revokeGroupMembership(grantedUserLogin,
		groupName);
}

std::string AuthorizationWS::listSessionPrivileges(const int sessionID) const
{
	return hmdbServices::AuthorizationWS(creator()).listSessionPrivileges(sessionID);
}

void AuthorizationWS::grantSessionPrivileges(const std::string & grantedUserLogin,
	const int sessionID, const bool write)
{
	hmdbServices::AuthorizationWS(creator()).grantSessionPrivileges(grantedUserLogin,
		sessionID, write);
}

void AuthorizationWS::removeSessionPrivileges(const std::string & grantedUserLogin,
	const int sessionID)
{
	hmdbServices::AuthorizationWS(creator()).removeSessionPrivileges(grantedUserLogin,
		sessionID);
}

void AuthorizationWS::alterSessionVisibility(const int sessionID, const bool isPublic,
	const bool isWritable)
{
	hmdbServices::AuthorizationWS(creator()).alterSessionVisibility(sessionID,
		isPublic, isWritable);
}