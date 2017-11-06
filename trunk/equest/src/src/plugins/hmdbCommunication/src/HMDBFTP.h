/********************************************************************
	created:  2014/05/29	09:01:08
	filename: HMDBFTP.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCONNECTION__HMDBFTP_H__
#define __HEADER_GUARD_HMDBCONNECTION__HMDBFTP_H__

#include <plugins/hmdbCommunication/IHMDBFtp.h>

namespace hmdbCommunication
{
	class CURLFTPManager;

	//! Klasa realizuj¹ca transfery FTP z baz¹ danych
	class HMDBFTP : public IHMDBFtp
	{
	public:

		//"testUser", "testUser"

		//! Konstruktor
		//! \param user U¿ytkownik po³¹czenia
		//! \param password Has³o u¿ytkownika
		//! \param url Adres serwera
		//! \param manager Manager obs³uguj¹cy transfery
		HMDBFTP(const std::string & user, const std::string & password,
			const std::string & url,
			utils::shared_ptr<CURLFTPManager> manager);

		//! Desturktor wirtualny
		virtual ~HMDBFTP();

		//! \param destinationFileName Nazwa docelowa uploadowanego pliku na serwerze bazy ruchu
		//! \param stream Strumieñ z którego bêdziemy wysy³aæ zawartoœæ pliku
		//! \param size Rozmiar danych do wys³ania
		virtual TransferPtr preparePut(const std::string & destinationFileName,
			IHMDBStorageOperations::IStreamPtr stream, const unsigned long long size);

		//! \param destinationFileName Nazwa pobieranego pliku z serwera bazy ruchu
		//! \param stream Strumieñ do którego bêdziemy zapisywaæ zawartoœæ œci¹ganego pliku
		virtual TransferPtr prepareGet(const std::string & destinationFileName,
			IHMDBStorageOperations::OStreamPtr stream, const unsigned long long size);

	private:
		//! U¿ytkownik po³¹czenia
		const std::string user;
		//! Has³o u¿ytkownika
		const std::string password;
		//! Adres serwera
		const std::string url;
		//! Manager obs³uguj¹cy transfery
		utils::weak_ptr<CURLFTPManager> manager;
	};
}

#endif	// __HEADER_GUARD_HMDBCONNECTION__HMDBFTP_H__