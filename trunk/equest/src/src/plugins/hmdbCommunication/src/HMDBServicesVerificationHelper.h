/********************************************************************
	created:  2014/08/06	10:02:30
	filename: HMDBServicesVerificationHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSERVICESVERIFICATIONHELPER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSERVICESVERIFICATIONHELPER_H__

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
	class IFileStoremanWS;
}

namespace hmdbCommunication
{
	class IHMDBFtp;

	class HMDBServicesVerificationHelper
	{
	public:
		//! \param singleAccountFactory Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::ISingleAccountFactoryWS * singleAccountFactory);
		//! \param multiAccountFactory Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMultiAccountFactoryWS * multiAccountFactory);
		//! \param administration Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IAdministrationWS * administration);
		//! \param authorization Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IAuthorizationWS * authorization);
		//! \param generalQueries Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IGeneralBasicQueriesWS * generalQueries);
		//! \param motionQueries Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMotionBasicQueriesWS * motionQueries);
		//! \param motionUpdate Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMotionBasicUpdatesWS * motionUpdate);
		//! \param medicalUpdate Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMedicalBasicUpdatesWS * medicalUpdate);
		//! \param motionFilestoreman Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMotionFileStoremanWS * motionFilestoreman);
		//! \param medicalFilestoreman Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IMedicalFileStoremanWS * medicalFilestoreman);
		//! \param userPersonalSpace Us³uga do weryfikacji
		//! \return Prawda je¿eli us³uga poprawnie zweryfikowana
		static const bool verify(hmdbServices::IUserPersonalSpaceWS * userPersonalSpace);
		//! \param fileStoreman Us³uga plików
		//! \param ftp Dostêp do plików
		//! \return Prawda je¿eli poprawnie zweryfikowano dostêp do plików
		static const bool verify(hmdbServices::IFileStoremanWS * fileStoreman, IHMDBFtp * ftp);
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSERVICESVERIFICATIONHELPER_H__