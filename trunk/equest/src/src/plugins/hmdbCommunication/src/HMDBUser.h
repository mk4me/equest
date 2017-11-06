/********************************************************************
    created:  2012/02/03
    created:  3:2:2012   17:10
    filename: CommunicationDataSourceUser.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__COMMUNICATIONDATASOURCEUSER_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__COMMUNICATIONDATASOURCEUSER_H__

#include <plugins/hmdbCommunication/IDataSourceUser.h>
#include "DataSourceUserData.h"
#include "DataSourceUserPrivilages.h"

//! Implementacja obsługująca informacje o aktualnym użytkowniku
class User : public hmdbCommunication::IUser
{
public:

    //! Konstruktor domyślny
    User();

    //! Destruktor wirtualny
    virtual ~User();

    //! \return Identyfikator użytkownika
    virtual const int id() const;
    //! \param id Nazwa użytkownika
    void setID(int id);

    //! \return Nazwa użytkownika
    virtual const std::string & name() const;
    //! \param name Nazwa użytkownika
    void setName(const std::string & name);

    //! \return Hasło użytkownika
    const std::string & password() const;
    //! \param password Hasło użytkownika
    void setPassword(const std::string & password);

    //! \return Prawa użytkownika
    virtual const UserPrivilages & userPrivilages() const;
    //! \param userPrivilages Nowe uprawnienia użytkownika
    void setUserPrivilages(const UserPrivilages & userPrivilages);

    //! \return Dane użytkownika
    virtual const UserData & userData() const;
    //! \param userData Nowe dane użytkownika
    void setUserData(const UserData & userData);
	//! \return Grupy użytkowników bazy danych do których należymy
	virtual const UserGroups userGroups() const;
	//! \param userGroups Grupy użytkowników bazy danych do których należymy
	void setUserGroups(const UserGroups & userGroups);

private:
    //! Identyfikator użytkownika
    int id_;
    //! Nazwa użytkownika
    std::string name_;
    //! Hasło użytkownika
    std::string password_;
    //! Uprawnienia użytkownika
    UserPrivilages userPrivilages_;
    //! Dane użytkownika
    UserData userData_;
	//! Grupy użytkowników bazy danych do których należymy
	UserGroups userGroups_;
};

#endif  //  HEADER_GUARD_HMDBCOMMUNICATION__COMMUNICATIONDATASOURCEUSER_H__
