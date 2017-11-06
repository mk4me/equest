/********************************************************************
    created:  2015/11/26 13:17
    filename: ContextConfigurationSettingsFile.h
    author:   Wojciech Knieæ
    
    purpose: 
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__ContextConfigurationSettingsFile_h__
#define HEADER_GUARD_HMDBCOMMUNICATION__ContextConfigurationSettingsFile_h__

#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <plugins/hmdbCommunication/Export.h>

namespace hmdbCommunication {
	//! Klasa 
	class HMDBCOMMUNICATION_EXPORT ContextConfigurationSettingsFile
	{
	public:
		virtual ~ContextConfigurationSettingsFile() {}

	public:
		//! Wczytuje plik konfiguracyjny nadpisuj¹c wpisy w przekazanej konfiguracji. Metoda nie zwraca b³êdu przy braku pliku. 
		//! \param path Œcie¿ka do pliku konfiguracyjnego
		//! \param defaultConf Nadpisywana konfiguracja
		//! \return nowa, zaktualizowana konfiguracja
		static IHMDBSourceViewManager::ContextConfiguration read(const QString& path, const IHMDBSourceViewManager::ContextConfiguration& defaultConf = defaultConfig());
		//! Zapisuje podan¹ konfiguracjê do podanego pliku
		//! \param path Œcie¿ka, gdzie ma zostaæ zapisana konfiguracja
		//! \param ccfg Zapisywana konfiguracja
		static void write(const QString& path, const IHMDBSourceViewManager::ContextConfiguration& ccfg);
		//! \return Zwraca domyœln¹ konfiguracjê stosowan¹ w PJ
		static IHMDBSourceViewManager::ContextConfiguration defaultConfig();
	};
}

#endif