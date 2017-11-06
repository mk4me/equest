/********************************************************************
	created:  2012/04/06
	created:  6:4:2012   8:54
	filename: AccountFactoryWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_ACCOUNTFACTORYWS_H__
#define HEADER_GUARD__HMDBSERVICES_ACCOUNTFACTORYWS_H__

#include <hmdbserviceslib/Export.h>
#include <hmdbserviceslib/IAccountFactoryWS.h>
#include <networkUtils/IWSDLService.h>

namespace hmdbServices{
	//! Klasa tworząca konto po stronie bazy danych
	class HMDBSERVICES_EXPORT SingleAccountFactoryWS : public ISingleAccountFactoryWS
	{
	public:
		SingleAccountFactoryWS(networkUtils::IWSDLServicePtr wsdlService);
		virtual ~SingleAccountFactoryWS();

		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName);

		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode);

		virtual bool resetPassword(const std::string & email);

	private:
		//! Serwis obsługujący tą usługę
		networkUtils::IWSDLServicePtr wsdlService;
	};

	//! Klasa tworząca konto po stronie bazy danych (medycznej i ruchu)
	class HMDBSERVICES_EXPORT MultiAccountFactoryWS : public IMultiAccountFactoryWS
	{
	public:
		MultiAccountFactoryWS(networkUtils::IWSDLServicePtr wsdlService);
		virtual ~MultiAccountFactoryWS();

		//! \param login Login użytkownika
		//! \param email Adres email użytkownika
		//! \param password Hasło użytkownika
		//! \param firstName Imię użytkownika
		//! \param lastName Nazwisko użytkownika
		//! \param propagateToHMDB Czy propagować tworzenie konta na bazę pacjentów
		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName, const bool propagateToHMDB);

		//! \param login Login użytkownika
		//! \param activationCode Kod aktywacyjny użytkownika
		//! \param propagateToHMDB Czy propagować aktywację konta na bazę pacjentów
		//! \return Czy udało się aktywować konto
		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode,
			const bool propagateToHMDB);

		//! \param email Adres email użytkownika dla którego chcemy zresetować hasło
		//! \param propagateToHMDB Czy propagować reset hasła na bazę pacjentów
		//! \return Czy udało się zresetować hasło
		virtual bool resetPassword(const std::string & email, const bool propagateToHMDB);

	private:
		//! Serwis obsługujący tą usługę
		networkUtils::IWSDLServicePtr wsdlService;
	};
}

#endif	//	HEADER_GUARD__HMDBSERVICES_ACCOUNTFACTORYWS_H__
