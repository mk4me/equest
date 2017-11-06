/********************************************************************
	created:	2015/10/22
	created:	10:04
	filename: 	WatcherConfig.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM_WATCHER__WatcherConfig_H__
#define HEADER_GUARD_DICOM_WATCHER__WatcherConfig_H__

#include <set>
#include <utils/Filesystem.h>

//! Przechowuje informacje o przetworzonych plikach.
//! Mo¿na unikn¹æ dziêki temu dwukrotnego przetworzenia tego samego pliku
class WatcherConfig
{
public:
	WatcherConfig(const utils::Filesystem::Path& path);
	~WatcherConfig() {}
	//! zapis informacji o wszystkich przetworzonych plikach
	void save();
	//! wczytanie listy przetworzonych plików
	void loadOrCreate();
	//! \return czy plik ju¿ byæ przetwarzany
	bool wasProcessed(const utils::Filesystem::Path& path);
	//! dodanie pliku do listy przetworzonych
	void addProcessed(const utils::Filesystem::Path& p);
private:
	//! lista przetworzonych
	std::set<utils::Filesystem::Path> processed;
	//! œcie¿ka do pliku ze statusami
	utils::Filesystem::Path configPath;
};


#endif // HEADER_GUARD 
