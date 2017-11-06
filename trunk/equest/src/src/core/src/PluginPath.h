/********************************************************************
    created:  2013/01/07
    created:  7:1:2013   15:49
    filename: CustomPath.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CUSTOMPATH_H__
#define HEADER_GUARD___CUSTOMPATH_H__

#include <corelib/IPath.h>

namespace core {

class PluginPath : public IPath
{
public:
	PluginPath(const std::string & pluginName);
	virtual ~PluginPath();

	//! \return Zwraca pelna sciezke do folderu "MyDocuments\APP"
	virtual const utils::Filesystem::Path& getUserDataPath() const;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\APP"
	virtual const utils::Filesystem::Path& getApplicationDataPath() const;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\APP"
	virtual const utils::Filesystem::Path& getUserApplicationDataPath() const;
	//! \return Zwraca pelna sciezke do folderu z zasobami aplikacji
	virtual const utils::Filesystem::Path& getResourcesPath() const;
	//! \return Zwraca pe³n¹ œcie¿kê do tymczasowego folderu, który jest czyszczony podczas ka¿dego zamykania aplikacji
	virtual const utils::Filesystem::Path& getTempPath() const;
	//! \return Zwraca pe³n¹ œcie¿kê do folderu pluginu
	virtual const utils::Filesystem::Path& getPluginPath() const;

private:
	const utils::Filesystem::Path pluginPath;
	const utils::Filesystem::Path pluginTmpPath;
};

}

#endif	//	HEADER_GUARD___CUSTOMPATH_H__
