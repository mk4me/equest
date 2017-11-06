/********************************************************************
    created:  2013/01/07
    created:  7:1:2013   12:31
    filename: IApplication.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IAPPLICATION_H__
#define HEADER_GUARD___IAPPLICATION_H__

namespace loglib {
	class ILog;
}
namespace core {

	class IPath;
	class IDataManagerReader;
	class IParserManagerReader;
	class IStreamManagerReader;
	class IFileManagerReader;
	class IRegisteredDataTypesManagerReader;	
	class IDataHierarchyManagerReader;
	class IPluginStorage;
	class IServiceManager;	
	class ISourceManager;
	class IVisualizerManager;
	class IApplicationDescription;
	class ILanguagesManager;
	class IJobManager;
	class IThreadPool;	

	//! Interfejs klasy dostarczaj¹cej pluginom ogólnych zasobów aplikacji:
	//! interfejs œcie¿ek, interfejs do czytania danych,
	//! interfejs do logowania oraz interfejs przechowywania bloków pamiêci o zadanym rozmiarze
	//! wg kluczy

	//! WA¯NE - pozosta³e elementy aplikacji których tutaj nie ma bêd¹ dostarczane poszczególnym modu³om indywidualnie
	//! wg przyjêtych regu³ i potrzeb - patrz serwisy, Ÿród³a danych
	class IApplication {		

	public:
		//! Destruktor wirtualny
		virtual ~IApplication() {}
		//! \return Opis aplikacji
		virtual const IApplicationDescription * description() = 0;
		//! \return Manager translacji aplikacji - pozwala zmieniaæ jêzyk
		virtual ILanguagesManager * languagesManager() = 0;
		//! \return Interfejs œcie¿ek aplikacji
		virtual IPath * paths() = 0;
		//! \return Interfejs do logowania
		virtual loglib::ILog * log() = 0;
		//! \return Interfejs do czytania danych
		virtual IDataManagerReader * dataManagerReader() = 0;
		//! \return Interfejs do czytania informacji o parserach
		virtual IParserManagerReader * parserManagerReader() = 0;
		//! \return Interfejs do czytania danych strumieniowych
		virtual IStreamManagerReader * streamDataManagerReader() = 0;
		//! \return Interfejs do czytania danych strumieniowych
		virtual IFileManagerReader * fileDataManagerReader() = 0;
		//! \return Interfejs do czytania opisu zarejestrowanych danych
		virtual IRegisteredDataTypesManagerReader * registeredDataTypesManagerReader() = 0;
		//! \return Interfejs do czytania opisu hierarchi danych
		virtual IDataHierarchyManagerReader * dataHierarchyManagerReader() = 0;
		//! \return Interfejs do dedykowanej przestrzeni sk³¹dowania danych
		virtual IPluginStorage * pluginStorage() = 0;
		//! \return Interfejs dostêpu do serwisów
		virtual IServiceManager * serviceManager() = 0;
		//! \return Interfejs dostêpu do Ÿróde³
		virtual ISourceManager * sourceManager() = 0;
		//! \return Interfejs managera wizualizatorów
		virtual IVisualizerManager * visualizerManager() = 0;
		//! \return Interfejs dostêpu do Ÿróde³
		virtual IThreadPool * threadPool() = 0;
		//! \return Interfejs managera wizualizatorów
		virtual IJobManager * jobManager() = 0;
	};
}

#endif	//	HEADER_GUARD___IAPPLICATION_H__
