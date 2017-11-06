/********************************************************************
	created:  2012/04/06
	created:  6:4:2012   8:49
	filename: IAccountFactoryWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_IACCOUNTFACTORYWS_H__
#define HEADER_GUARD__HMDBSERVICES_IACCOUNTFACTORYWS_H__

#include <string>
#include <utils/SmartPtr.h>
#include <hmdbserviceslib/Export.h>

namespace hmdbServices
{
	//! Interfejs serwisu do rejestracji i aktywacji kont w jednej usłudze - Med lub Motion
	class ISingleAccountFactoryWS
	{
	public:
		//! Destruktor wirtualny
		virtual ~ISingleAccountFactoryWS() {}

		//! \param login Login nowego konta
		//! \param email Skojarzone konto email (np. kod aktywacyjny, odzyskanie hasła)
		//! \param password Hasło dla nowego konta
		//! \param firstName Imię użytkownika
		//! \param lastName Nazwisko użytkownika
		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName) = 0;

		//! \param login Login który chcemy aktywować
		//! \param activationCode Kod aktywacyjny którym aktywujemy - przysdzedł na zarejestrowany email
		//! \return Prawda jeśli poprawnie aktywowano
		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode) = 0;

		//! \param email Konto pocztowe związane z naszym użytkownikiem, dla którego chcemy zmienić hasło
		//! \return Czy poprawnie zresetowano hasło
		virtual bool resetPassword(const std::string & email) = 0;
	};

	using SingleAccountFactoryWSPtr = utils::shared_ptr<ISingleAccountFactoryWS>;
	using SingleAccountFactoryWSConstPtr = utils::shared_ptr<const ISingleAccountFactoryWS>;

	//! Interfejs serwisu do rejestracji i aktywacji kont w obu usługach - Med i Motion
	class HMDBSERVICES_EXPORT IMultiAccountFactoryWS
	{
	public:
		//! Destruktor wirtualny
		virtual ~IMultiAccountFactoryWS() {}

		//! \param login Login nowego konta
		//! \param email Skojarzone konto email (np. kod aktywacyjny, odzyskanie hasła)
		//! \param password Hasło dla nowego konta
		//! \param firstName Imię użytkownika
		//! \param lastName Nazwisko użytkownika
		//! \param propagateToHMDB Informacja czy utworzyć automatycznie konta w obu usługach
		virtual void createUserAccount(const std::string & login, const std::string & email, const std::string & password,
			const std::string & firstName, const std::string & lastName, const bool propagateToHMDB) = 0;

		//! \param login Login który chcemy aktywować
		//! \param activationCode Kod aktywacyjny którym aktywujemy - przysdzedł na zarejestrowany email
		//! \param propagateToHMDB Informacja czy aktywować automatycznie konta w obu usługach
		//! \return Prawda jeśli poprawnie aktywowano
		virtual bool activateUserAccount(const std::string & login, const std::string & activationCode,
			const bool propagateToHMDB) = 0;

		//! \param email Konto pocztowe związane z naszym użytkownikiem, dla którego chcemy zmienić hasło
		//! \param propagateToHMDB Informacja czy aktywować automatycznie konta w obu usługach
		//! \return Czy poprawnie zresetowano hasło
		virtual bool resetPassword(const std::string & email, const bool propagateToHMDB) = 0;
	};

	using MultiAccountFactoryWSPtr = utils::shared_ptr<IMultiAccountFactoryWS>;
	using MultiAccountFactoryWSConstPtr = utils::shared_ptr<const IMultiAccountFactoryWS>;
}

#endif	//	HEADER_GUARD__HMDBSERVICES_IACCOUNTFACTORYWS_H__
