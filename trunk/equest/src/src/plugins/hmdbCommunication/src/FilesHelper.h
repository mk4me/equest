//Login i hasło dla nowego testowego użytkownika: applet_user / aplet4Motion

/********************************************************************
	created:  2012/02/23
	created:  23:2:2012   22:36
	filename: FilesHelper.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__FILESHELPER_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__FILESHELPER_H__

#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/DataStatus.h>
#include "DataSourceStatusManager.h"

namespace hmdbCommunication
{

	//! Klasa pomocnicza przy zbieraniu plików dla poszczególnych poziomów hierarchi danych i filtorawaniu ich wg statusu
	class FilesHelper
	{
	public:

		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param shallowCopy Płytka kopia danych
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getDisorderFiles(int disorderID, const ShallowCopy & shallowCopy, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param shallowCopy Płytka kopia danych
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getPatientFiles(int patientID, const ShallowCopy & shallowCopy, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param shallowCopy Płytka kopia danych
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getSubjectFiles(int subjectID, const ShallowCopy & shallowCopy, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param shallowCopy Płytka kopia danych
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getSessionFiles(int sessionID, const ShallowCopy & shallowCopy, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param shallowCopy Płytka kopia danych
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getSessionSpecificFiles(int sessionID, const ShallowCopy & shallowCopy, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param shallowCopy Płytka kopia danych
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getMotionFiles(int motionID, const ShallowCopy & shallowCopy, std::set<int> & files);

		//! \param inFiles Zbiór plików które chcemy filtrować
		//! \param storage Stan składowania pliku względem którego filtrujemy pliki
		//! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
		//! \param filesStatusManager Manager statusu plików
		static void filterFiles(const std::set<int> & inFiles, DataStorage storage, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
		//! \param inFiles Zbiór plików które chcemy filtrować
		//! \param storage Stan załadowania pliku względem którego filtrujemy pliki
		//! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
		//! \param filesStatusManager Manager statusu plików
		static void filterFiles(const std::set<int> & inFiles, DataUsage usage, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
		//! \param inFiles Zbiór plików które chcemy filtrować
		//! \param status Stan względem którego filtrujemy pliki
		//! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
		//! \param filesStatusManager Manager statusu plików
		static void filterFiles(const std::set<int> & inFiles, const DataStatus & status, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);
		//! \param inFiles Zbiór plików które chcemy filtrować
		//! \param filteredFiles [out] Wyjściowy zbiór przefiltrowanych plików
		//! \param filesStatusManager Manager statusu plików
		static void filterFiles(const std::set<int> & inFiles, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager);

		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param shallowCopy Płytka kopia danych
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getFiles(const hmdbServices::MedicalShallowCopy::Disorder * disord, const ShallowCopy & shallowCopy, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getFiles(const hmdbServices::MedicalShallowCopy::Patient * patient, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getFiles(const hmdbServices::MotionShallowCopy::Performer * subject, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getFiles(const hmdbServices::MotionShallowCopy::Session * session, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getSpecificFiles(const hmdbServices::MotionShallowCopy::Session * session, std::set<int> & files);
		//! \param disorderID Identyfikator schorzenia dla którego chcemy pobrać pliki
		//! \param files [out] Zbiór plików związanych z chorobą
		static void getFiles(const hmdbServices::MotionShallowCopy::Trial * motion, std::set<int> & files);

		//! \param files Zbiór plików których sumaryczny rozmiar chcemy poznać
		//! \param shallowCopy Płytka kopia bazy danych
		//! \return Sumaryczny rozmiar plików
		static const unsigned long long filesSize(const std::set<int> & filesIDs, const ShallowCopy & shallowCopy);

		//! \param file Plik którego rozmiar chcemy poznać
		//! \param shallowCopy Płytka kopia bazy danych
		//! \return Rozmiar pliku
		static const unsigned long long fileSize(const int fileID, const ShallowCopy & shallowCopy);
	};

}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__FILESHELPER_H__
