/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   16:01
	filename: 	IncrementalBranchShallowCopy.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_HMDBSERVICES__INCREMENTALBRANCHSHALLOWCOPY_H__
#define HEADER_GUARD_HMDBSERVICES__INCREMENTALBRANCHSHALLOWCOPY_H__

#include <hmdbserviceslib/ShallowCopy.h>
#include <vector>

namespace hmdbServices {

	struct IncrementalBranchShallowCopy
	{
		struct File;
		struct Trial;
		struct Session;
		struct Performer;
		struct PerformerConf;
		struct GroupAssigment;
		struct ShallowCopy;

		//! Mapa atrybut -> wartoœæ
		using Attrs = std::map<std::string, std::string>;

		//! Plik
		struct File
		{
			//! \return Czy plik nale¿y do sesji
			inline bool isSessionFile() const
			{
				return trialID < 0;
			}

			int trialID;					//! Próba pomiarowa
			int sessionID;				//! Sesja
			int fileID;						//! Identyfikator
			std::string fileName;			//! Nazwa pliku (nazwa + rozszerzenie)
			std::string fileDescription;	//! Opis pliku
			std::string subdirPath;			//! Katalog pliku po stronie bazy danych
			FileSize fileSize;				//! Rozmiar pliku (w bajtach)

			File() : trialID(-1), sessionID(-1), fileID(-1) {}
		};
		//! Mapa ID pliku -> plik
		using Files = std::vector<File>;

		//! Próba pomiarowa
		struct Trial
		{
			int sessionID;				//! Sesja
			Attrs attrs;					//! Atrybuty
			Files modifiedFiles;		    //! Pliki zmodyfikowane
			Files addedFiles;               //! Pliki dodane
			int trialID;					//! Identyfikator
			std::string trialName;			//! Nazwa próby pomiarowej
			std::string trialDescription;	//! Opis próby pomiarowej

			Trial() : sessionID(-1), trialID(-1) {}
		};

		using Trials = std::vector<Trial>;

		//! Sesje
		struct Session
		{
			int performerConfID;		//! Konfiguracja aktora
			int groupAssigmentID;	//! Przynale¿noœc do grupy
			Trials trials;						//! Próby pomiarowe
			Attrs attrs;						//! Atrybuty
			int sessionID;						//! ID sesji
			int userID;							//! ID u¿ytkownika (w³aœciciel danych)
			int labID;							//! ID laboratorium gdzie nagrywano sesjê
			std::string motionKind;				//! Typ ruchu
			DateTime sessionDate;				//! Data sesji
			std::string sessionName;			//! Nazwa sesji
			std::string tags;					//! Tagi
			std::string sessionDescription;		//! Opis sesji
			Files files;						//! Pliki sesji
			std::string emgConf;				//! Konfiguracja EMG
			Session() : performerConfID(-1), groupAssigmentID(-1), sessionID(-1), userID(-1), labID(-1) {}
		};

		//! Mapa ID sesji -> sesja
		using Sessions = std::vector<Session>;

		//! Grupa sesji
		struct GroupAssigment
		{
			int sessionGroupID;		//! Identyfikator
			Sessions sessions;		//! Sesje
			GroupAssigment() : sessionGroupID(-1) {}
		};

		//! Mapa ID grupy sesji -> grupa sesji
		using GroupAssigments = std::vector<GroupAssigment>;

		//! Konfiguracja aktora
		struct PerformerConf
		{
			int performerConfID;	//! Identyfikator
			Attrs attrs;			//! Atrybuty
			int performerID;	//! Aktor
			int sessionID;		//! Sesja
			PerformerConf() : performerConfID(-1), performerID(-1), sessionID(-1) {}
		};
		//! Mapa ID konfiguracji aktora -> konfiguracja aktora
		using PerformerConfs = std::vector<PerformerConf>;

		//! Aktor
		struct Performer
		{
			int performerID;						//! Identyfikator
			Attrs attrs;							//! Atrybuty
			PerformerConfs performerConfs;			//! Konfiguracja aktora
			//MedicalShallowCopy::Patient * patient;	//! Odpowiadaj¹cy pacjent z bazy medycznej, mo¿e byæ nullptr
		};

		//! Mapa ID aktora -> aktor
		using Performers = std::vector<Performer>;

		//! P³ytka kopia bazy danych
		struct ShallowCopy
		{
		public:
			//DateTime timestamp;					//! Data
			Sessions sessions;					//! Sesje
			GroupAssigments groupAssigments;	//! Grupy sesji
			Trials trials;						//! Próby pomiarowe
			Performers performers;				//! Aktorzy
			PerformerConfs performerConfs;		//! Konfiguracje aktorów
			Files files;						//! Pliki
		};

		//! Nowe dane
		ShallowCopy added;
		//! Dane zmienione
		ShallowCopy modified;
		//! Dane usuniete lokalnie
		ShallowCopy removedLocaly;
		//! Dane usuniete globalnie
		ShallowCopy removedGlobaly;
	};
}
#endif
