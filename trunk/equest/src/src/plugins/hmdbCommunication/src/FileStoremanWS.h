/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:31
	filename: SimpleFileStoremanWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBCOMMUNICATION_FILESTOREMANWS_H__
#define HEADER_GUARD__HMDBCOMMUNICATION_FILESTOREMANWS_H__

#include "WSDLServiceTypes.h"
#include <hmdbserviceslib/IFileStoremanWS.h>

namespace hmdbCommunication
{
	//! Klasa przygotowująca pliki do pobrania
	class SimpleFileStoremanWS : public hmdbServices::IBasicStoremanWS
	{
	public:

		SimpleFileStoremanWS(const WSDLServiceCreator creator);

		virtual ~SimpleFileStoremanWS();

		virtual void downloadComplete(const int id, const std::string & path);

		virtual hmdbServices::xmlWsdl::FileData retrieve(const int id);

	private:
		//! Serwis obsługujący tą usługę
		const WSDLServiceCreator creator;
	};

	//! Klasa przygotowująca zdjęcia do pobrania
	class PhotoStoremanWS : public hmdbServices::IBasicStoremanWS
	{
	public:

		PhotoStoremanWS(const WSDLServiceCreator creator);

		virtual ~PhotoStoremanWS();

		virtual void downloadComplete(const int id, const std::string & path);

		virtual hmdbServices::xmlWsdl::FileData retrieve(const int id);

	private:
		//! Serwis obsługujący tą usługę
		const WSDLServiceCreator creator;
	};

	//! Klasa przygotowująca płytkie kopie do pobrania
	class ShallowStoremanWS : public hmdbServices::IShallowStoremanWS
	{
	public:

		ShallowStoremanWS(const WSDLServiceCreator creator);

		virtual ~ShallowStoremanWS();

		virtual std::string getShallowCopy();

		virtual std::string getMetadata();

		virtual void downloadComplete(const std::string & path);

		virtual std::string getShallowCopyIncrement(const std::string & dateTime);

		virtual std::string getShallowCopyBranchesIncrement(const std::string & dateTime);

	private:
		//! Serwis obsługujący tą usługę
		const WSDLServiceCreator creator;
	};

	class MotionFileStoremanWS : public hmdbServices::IMotionFileStoremanWS
	{
	public:

		MotionFileStoremanWS(const WSDLServiceCreator creator);

		virtual ~MotionFileStoremanWS();

		virtual void downloadComplete(const int fileID, const std::string & path);

		virtual void downloadComplete(const std::string & path);

		virtual hmdbServices::xmlWsdl::FileData retrieve(const int fileID);

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
		const WSDLServiceCreator creator;
	};

	class MedicalFileStoremanWS : public hmdbServices::IMedicalFileStoremanWS
	{
	public:

		MedicalFileStoremanWS(const WSDLServiceCreator creator);

		virtual ~MedicalFileStoremanWS();

		virtual void downloadComplete(const int fileID, const std::string & path);

		virtual void downloadComplete(const std::string & path);

		virtual hmdbServices::xmlWsdl::FileData retrieve(const int fileID);

		virtual std::string getShallowCopy();

		virtual std::string getMetadata();

		virtual std::string getShallowCopyIncrement(const std::string & dateTime);

		virtual std::string getShallowCopyBranchesIncrement(const std::string & dateTime);

	private:
		//! Serwis obsługujący tą usługę
		const WSDLServiceCreator creator;
	};
}

#endif	//	HEADER_GUARD__HMDBCOMMUNICATION_FILESTOREMANWS_H__
