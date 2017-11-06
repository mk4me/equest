/********************************************************************
    created:  2013/01/15
    created:  15:1:2013   8:54
    filename: Path.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PATH_H__
#define HEADER_GUARD___PATH_H__

#include <corelib/IPath.h>

namespace core {

	class Path : public IPath
	{
	public:
		Path(const utils::Filesystem::Path & userDataPath,
		const utils::Filesystem::Path & applicationDataPath,
		const utils::Filesystem::Path & userApplicationDataPath,
		const utils::Filesystem::Path & resourcesPath,
		const utils::Filesystem::Path & tempPath,
		const utils::Filesystem::Path & pluginPath);
		virtual ~Path();

	public:
		//! \return Zwraca pelna sciezke do folderu "MyDocuments\APP"
		virtual const utils::Filesystem::Path& getUserDataPath() const;
		//! \return Zwraca pelna sciezke do folderu "ApplicationData\APP" dla wszystkich u¿ytkowników
		virtual const utils::Filesystem::Path& getApplicationDataPath() const;
		//! \return Zwraca pelna sciezke do folderu "ApplicationData\APP" dla aktualnego u¿ytkownika
		virtual const utils::Filesystem::Path& getUserApplicationDataPath() const;
		//! \return Zwraca pelna sciezke do folderu z zasobami aplikacji
		virtual const utils::Filesystem::Path& getResourcesPath() const;
		//! \return Zwraca pe³n¹ œcie¿kê do tymczasowego folderu, który jest czyszczony podczas ka¿dego zamykania aplikacji
		virtual const utils::Filesystem::Path& getTempPath() const;
		//! \return Zwraca pe³n¹ œcie¿kê do folderu pluginu
		virtual const utils::Filesystem::Path& getPluginPath() const;
		//! \return Zwraca pe³n¹ œcie¿kê do folderu z t³umaczeniami aplikacji
		const utils::Filesystem::Path& getTranslationsPath() const;

	private:
		utils::Filesystem::Path translationsPath_;
		utils::Filesystem::Path userDataPath_;
		utils::Filesystem::Path applicationDataPath_;
		utils::Filesystem::Path userApplicationDataPath_;
		utils::Filesystem::Path resourcesPath_;
		utils::Filesystem::Path tempPath_;
		utils::Filesystem::Path pluginPath_;
	};
}

#endif	//	HEADER_GUARD___PATH_H__
