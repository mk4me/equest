#include "CommunicationPCH.h"
#include "DataSourceUser.h"

User::User() : id_(-1)
{

}

User::~User()
{

}

const int User::id() const
{
    return id_;
}

void User::setID(int id)
{
    id_ = id;
}

const std::string & User::name() const
{
    return name_;
}

void User::setName(const std::string & name)
{
    name_ = name;
}

const std::string & User::password() const
{
    return password_;
}

void User::setPassword(const std::string & password)
{
    password_ = password;
}

const UserPrivilages & User::userPrivilages() const
{
    return userPrivilages_;
}

void User::setUserPrivilages(const UserPrivilages & userPrivilages)
{
    userPrivilages_ = userPrivilages;
}

const UserData & User::userData() const
{
    return userData_;
}

void User::setUserData(const UserData & userData)
{
    userData_ = userData;
}

const User::UserGroups User::userGroups() const
{
	return userGroups_;
}

void User::setUserGroups(const UserGroups & userGroups)
{
	userGroups_ = userGroups;
}
