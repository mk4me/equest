/********************************************************************
	created:  2014/05/21	12:24:31
	filename: IHMDBSession.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSESSION_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSESSION_H__

#include <string>
#include <utils/SmartPtr.h>

namespace hmdbServices
{
	class ISingleAccountFactoryWS;
	class IMultiAccountFactoryWS;
	class IAdministrationWS;
	class IAuthorizationWS;
	class IGeneralBasicQueriesWS;
	class IMotionBasicQueriesWS;
	class IMotionBasicUpdatesWS;
	class IMedicalBasicUpdatesWS;
	class IMotionFileStoremanWS;
	class IMedicalFileStoremanWS;
	class IUserPersonalSpaceWS;
}

namespace hmdbCommunication
{
	class IHMDBFtp;

	//! Interfejs dostarczaj¹cy us³ug bazy danych ruchu i medycznych
	class IHMDBSession
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBSession() {}

		//! \return Adres us³ug ruchu
		virtual const std::string motionServicesUrl() const = 0;
		//! \return Adres us³ug medycznych
		virtual const std::string medicalServicesUrl() const = 0;
		//! \return Adres danych ruchu
		virtual const std::string motionFtpUrl() const = 0;
		//! \return Adres danych medycznych
		virtual const std::string medicalFtpUrl() const = 0;
		//! \return Nazwa u¿ytkownika
		virtual const std::string userName() const = 0;

		//! \return Us³uga tworz¹ca konta danych ruchu
		virtual hmdbServices::ISingleAccountFactoryWS * motionAccountFactory() = 0;
		//! \return Us³uga tworz¹ca konta danych medycznych
		virtual hmdbServices::ISingleAccountFactoryWS * medicalAccountFactory() = 0;
		//! \return Us³uga tworz¹ca konta danych euchu i medycznych
		virtual hmdbServices::IMultiAccountFactoryWS * multiAccountFactory() = 0;
		//! \return Us³ugi administracyjne
		virtual hmdbServices::IAdministrationWS * administration() = 0;
		//! \return Us³ugi autoryzacyjne
		virtual hmdbServices::IAuthorizationWS * authorization() = 0;
		//! \return Us³ugi zapytañ o dane ruchu
		virtual hmdbServices::IMotionBasicQueriesWS * motionQueries() = 0;
		//! \return Us³ugi zapytañ o dane medyczne
		virtual hmdbServices::IGeneralBasicQueriesWS * medicalQueries() = 0;
		//! \return Us³ugi aktualizuj¹ce dane ruchu
		virtual hmdbServices::IMotionBasicUpdatesWS * motionUpdate() = 0;
		//! \return Us³ugi aktualizuj¹ce dane medyczne
		virtual hmdbServices::IMedicalBasicUpdatesWS * medicalUpdate() = 0;
		//! \return Us³ugi dostêpu do danych ruchu
		virtual hmdbServices::IMotionFileStoremanWS * motionFilestoreman() = 0;
		//! \return Us³ugi dostêpu do danych medycznych
		virtual hmdbServices::IMedicalFileStoremanWS * medicalFilestoreman() = 0;
		//! \return Us³ugi dostêpu do danych prywatnych u¿ytkownika
		virtual hmdbServices::IUserPersonalSpaceWS * userPersonalSpace() = 0;
		//! \return Po³¹czenie do pobierania danych ruchu
		virtual IHMDBFtp * motionFtp() = 0;
		//! \return Po³¹czenie do pobierania danych medycznych
		virtual IHMDBFtp * medicalFtp() = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBSession);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSESSION_H__