/********************************************************************
	created:  2014/05/14	20:30:45
	filename: FTPConnection.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__CURLFTPHELPER_H__
#define __HEADER_GUARD_NETWORKUTILS__CURLFTPHELPER_H__

#include <threadingUtils/IProgress.h>
#include <networkUtils/Export.h>
#include <networkUtils/IFtpOperations.h>
#include <networkUtils/Types.h>
#define NOMINMAX
#include <curl/curl.h>

namespace networkUtils
{
	class ICURLFTPProgress : public threadingUtils::IProgressSupervisor
	{
	public:
		//! Destruktor wirtualny
		virtual ~ICURLFTPProgress() {}		
		//! \param processedData Ilosc przetworzonych danych [bytes]
		virtual void setProcessedData(const std::size_t processedData) = 0;
	};

	class NETWORKUTILS_EXPORT CURLFTPHelper
	{
	public:
		//! \param curl Uchwyt do połączenia z serwerem
		//! \param user Nazwa uzytkownika jaką łączymy się z serwerem
		//! \param password Hasło uzytkownika jakim łączymy się z serwerem
		//! \param workingDirectory Katalog startowy względem homeDirecotry
		//! \param hostVerification Sposób weryfikacji serwera
		//! \param caPath Ścieżka certyfikatu jakim weryfikujemy serwer
		static void initializeFTPConnection(CURL * curl,
			const std::string & user = std::string(),
			const std::string & password = std::string(),
			const SSLHostVerification hostVerification = HVNone,
			const std::string & caPath = std::string());

		//!	Metoda pobierająca plik z serwera.
		//! \param curl Uchwyt do połączenia z serwerem
		//! \param remoteFileUrl Adres serwera i pliku który ściągamy
		//! \param localDestination Strumień w którym zpaisujemy ściągany plik
		//! \param progress Obiekt informowany o postępie ściągania, steruje przerwaniem operacji
		static void initializeFTPDownload(CURL * curl,
			const std::string & remoteFileUrl,
			std::ostream * localDestination,
			ICURLFTPProgress * progress = nullptr);

		//!	Metoda ładująca plik na serwer.
		//! \param curl Uchwyt do połączenia z serwerem
		//! \param remoteFileUrl Adres serwera i pliku który ładujemy
		//! \param localSource Strumień z którego czytamy zawartość pliku do wysłania
		//! \param progress Obiekt informowany o postępie wysyłania, steruje przerwaniem operacji
		static void initializeFTPUpload(CURL * curl,
			const std::string & remoteFileUrl,
			std::istream * localSource,
			ICURLFTPProgress * progress = nullptr);
	};
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLFTPHELPER_H__
