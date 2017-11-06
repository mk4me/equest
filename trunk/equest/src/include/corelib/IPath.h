/********************************************************************
	created:	2011/06/07
	created:	7:6:2011   12:57
	filename: 	IPath.h
	author:		Wojciech Knieć
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IDIRECTORIESINFO_H__
#define HEADER_GUARD_CORE__IDIRECTORIESINFO_H__

#include <utils/Filesystem.h>

namespace core {

//! Interfejs dostarczający informacji o strukturze plików obsługiwanej przez aplikację.
class IPath
{
public:
	//! Destruktor wirtualny
	virtual ~IPath() {}

	//! \return Zwraca pelna sciezke do folderu "MyDocuments\APP"
	virtual const utils::Filesystem::Path& getUserDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\APP" dla wszystkich użytkowników
	virtual const utils::Filesystem::Path& getApplicationDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu "ApplicationData\APP" dla aktualnego użytkownika
	virtual const utils::Filesystem::Path& getUserApplicationDataPath() const = 0;
	//! \return Zwraca pelna sciezke do folderu z zasobami aplikacji
	virtual const utils::Filesystem::Path& getResourcesPath() const = 0;
    //! \return Zwraca pełną ścieżkę do tymczasowego folderu, który jest czyszczony podczas każdego zamykania aplikacji
    virtual const utils::Filesystem::Path& getTempPath() const = 0;
	//! \return Losowa nazwa pliku tymczasowego
	inline const utils::Filesystem::Path getTempFilePath() const { return getTempPath() / utils::Filesystem::temporaryFile(); }
	//! \return Zwraca pełną ścieżkę do folderu pluginu
	virtual const utils::Filesystem::Path& getPluginPath() const = 0;
};

}

#endif
