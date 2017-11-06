/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:31
	filename: SimpleFileStoremanWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_FILESTOREMANWS_H__
#define HEADER_GUARD__HMDBSERVICES_FILESTOREMANWS_H__

#include <hmdbserviceslib/Export.h>
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <networkUtils/IWSDLService.h>

namespace hmdbServices
{
	//! Klasa przygotowująca pliki do pobrania
	class HMDBSERVICES_EXPORT SimpleFileStoremanWS : public IBasicStoremanWS
	{
	public:

		SimpleFileStoremanWS(networkUtils::IWSDLServicePtr wsdlService);

		virtual ~SimpleFileStoremanWS();

		virtual void downloadComplete(const int id, const std::string & path);

		virtual xmlWsdl::FileData retrieve(const int id);

	private:
		//! Serwis obsługujący tą usługę
		networkUtils::IWSDLServicePtr wsdlService;
	};

	//! Klasa przygotowująca zdjęcia do pobrania
	class HMDBSERVICES_EXPORT PhotoStoremanWS : public IBasicStoremanWS
	{
	public:

		PhotoStoremanWS(networkUtils::IWSDLServicePtr wsdlService);

		virtual ~PhotoStoremanWS();

		virtual void downloadComplete(const int id, const std::string & path);

		virtual xmlWsdl::FileData retrieve(const int id);

	private:
		//! Serwis obsługujący tą usługę
		networkUtils::IWSDLServicePtr wsdlService;
	};

	//! Klasa przygotowująca płytkie kopie do pobrania
	class HMDBSERVICES_EXPORT ShallowStoremanWS : public IShallowStoremanWS
	{
	public:

		ShallowStoremanWS(networkUtils::IWSDLServicePtr wsdlService);

		virtual ~ShallowStoremanWS();

		virtual std::string getShallowCopy();

		virtual std::string getMetadata();

		virtual void downloadComplete(const std::string & path);

		virtual std::string getShallowCopyIncrement(const std::string & dateTime);

		virtual std::string getShallowCopyBranchesIncrement(const std::string & dateTime);

	private:
		//! Serwis obsługujący tą usługę
		networkUtils::IWSDLServicePtr wsdlService;
	};

	class HMDBSERVICES_EXPORT MotionFileStoremanWS : public IMotionFileStoremanWS
	{
	public:

		MotionFileStoremanWS(networkUtils::IWSDLServicePtr wsdlService);

		virtual ~MotionFileStoremanWS();

		virtual void downloadComplete(const int fileID, const std::string & path);

		virtual void downloadComplete(const std::string & path);

		virtual xmlWsdl::FileData retrieve(const int fileID);

		virtual std::string getShallowCopy();

		virtual std::string getShallowCopyIncrement(const std::string & dateTime);

		virtual std::string getShallowCopyBranchesIncrement(const std::string & dateTime);

		virtual std::string getMetadata();

		virtual int storeMeasurementConfFile(const int mcID, const std::string & path,
			const std::string & description, const std::string & filename);

		virtual void storeMeasurementConfFiles(const int measurementConfID,
			const std::string & path, const std::string & description);

		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual int storeSessionFile(const int sessionID, const std::string & path,
			const std::string & description, const std::string & filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param sessionID id sesji która wcześniej została juz umieszczona w bazie danych
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		@param description
		*/
		virtual void storeSessionFiles(const int sessionID, const std::string & path,
			const std::string & description);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrolę bazy danych.
		@param trialID id trial
		@param path względna ścieżka do pliku na dysku serwera w stosunku do korzenia obszaru Usługi Transportu Plików
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		virtual int storeTrialFile(const int trialID, const std::string & path,
			const std::string & description, const std::string & filename);
		/**
		Realizuje wprowadzenie plików pod kontrolę bazy danych.
		@param trialID id trial
		@param path ścieżka do katalogu z plikami do wgrania na serwer
		*/
		virtual void storeTrialFiles(const int trialID, const std::string & path,
			const std::string & description);

		virtual void replaceFile(int fileID, const std::string& path, const std::string& filename);

	private:
		//! Serwis obsługujący tą usługę
		networkUtils::IWSDLServicePtr wsdlService;
	};

	class HMDBSERVICES_EXPORT MedicalFileStoremanWS : public IMedicalFileStoremanWS
	{
	public:

		MedicalFileStoremanWS(networkUtils::IWSDLServicePtr wsdlService);

		virtual ~MedicalFileStoremanWS();

		virtual void downloadComplete(const int fileID, const std::string & path);

		virtual void downloadComplete(const std::string & path);

		virtual xmlWsdl::FileData retrieve(const int fileID);

		virtual std::string getShallowCopy();

		virtual std::string getMetadata();

		virtual std::string getShallowCopyIncrement(const std::string & dateTime);

		virtual std::string getShallowCopyBranchesIncrement(const std::string & dateTime);

	private:
		//! Serwis obsługujący tą usługę
		networkUtils::IWSDLServicePtr wsdlService;
	};
}

#endif	//	HEADER_GUARD__HMDBSERVICES_FILESTOREMANWS_H__
