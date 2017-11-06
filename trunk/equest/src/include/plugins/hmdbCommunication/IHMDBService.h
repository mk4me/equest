/********************************************************************
	created:  2014/05/21	12:10:33
	filename: IHMDBService.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVICE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVICE_H__

#include <string>
#include <utils/Filesystem.h>
#include <networkUtils/Types.h>
#include <networkUtils/IWSDLService.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>

namespace hmdbCommunication
{
	class IHMDBService
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBService() {}

		//! \param motionServicesUrl Adres us³ug danych ruchu
		//! \param medicalServicesUrl Adres us³ug danych medycznych
		//! \param user Nazwa uzytkownika
		//! \param password Has³o uzytkownika
		//! \param motionDataUrl Adres danych ruchu
		//! \param medicalDataUrl Adres danych medycznych
		//! \param caPath Œcie¿ka certyfikatu uwierzytelniaj¹cego us³uge
		//! \param hostVerification Sposób weryfikacji hosta
		//! \return Sesja serwisów danych ruchu, nullptr je¿eli nie uda³o siê uwierzytelniæ
		//! u¿ytkownika w us³ugach danych ruchu i medycznych
		virtual const IHMDBSessionPtr createSession(const std::string & motionServicesUrl,
			const std::string & medicalServicesUrl,
			const std::string & user,
			const std::string & password,
			const std::string & motionDataUrl,
			const std::string & medicalDataUrl,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone) = 0;

		//! \param motion Czy sesja tylko dla motion? Jeœli false to tylko medical
		//! \param servicesUrl Adres us³ug danych ruchu		
		//! \param user Nazwa uzytkownika
		//! \param password Has³o uzytkownika
		//! \param dataUrl Adres danych ruchu	
		//! \param caPath Œcie¿ka certyfikatu uwierzytelniaj¹cego us³uge
		//! \param hostVerification Sposób weryfikacji hosta
		//! \return Sesja serwisów danych ruchu, nullptr je¿eli nie uda³o siê uwierzytelniæ
		//! u¿ytkownika w us³ugach danych ruchu i medycznych
		virtual const IHMDBSessionPtr createSession(const bool motion,
			const std::string & servicesUrl,			
			const std::string & user,
			const std::string & password,
			const std::string & dataUrl,			
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone) = 0;
		
		//! \param url Adres us³ugi
		//! \param user Nazwa uzytkownika
		//! \param password Has³o uzytkownika
		//! \param caPath Œcie¿ka certyfikatu uwierzytelniaj¹cego us³uge
		//! \param hostVerification Sposób weryfikacji hosta
		//! \param schemaPath Œcie¿ka do zapisu schematu serwisu
		//! \return Serwis WSDL obs³uguj¹cy dan¹ us³ugê
		virtual const networkUtils::IWSDLServicePtr createHMDBService(
			const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path()) = 0;

		//! \param url Adres us³ugi systemowej
		//! \param caPath Œcie¿ka certyfikatu uwierzytelniaj¹cego us³uge
		//! \param hostVerification Sposób weryfikacji hosta
		//! \param schemaPath Œcie¿ka do zapisu schematu serwisu
		//! \return Serwis WSDL obs³uguj¹cy dan¹ us³ugê systemow¹
		virtual const networkUtils::IWSDLServicePtr createHMDBSystemService(
			const std::string & url,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path()) = 0;

		//! \param url Adres serwera do pingowania
		//! \param timeout Maksymalny czas oczekiwania [ms]
		//! \param caPath Œcie¿ka ewentualnego certyfikatu SSL je¿eli wymagany
		//! \return Czy serwer odpowiedzia³
		virtual const bool serverOnline(const std::string & url,
			const unsigned int timeout = 250,
			const utils::Filesystem::Path & caPath = utils::Filesystem::Path()) const = 0;

		//! \param session Sesja us³ug bazy danych ruchu któr¹ chcemy obs³ugiwaæ
		virtual void attach(IHMDBSessionPtr session) = 0;

		//! \param session Sesja us³ug bazy danych ruchu któr¹ chcemy obs³ugiwaæ
		virtual void detach(IHMDBSessionPtr session) = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSERVICE_H__