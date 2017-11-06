/********************************************************************
    created:  2011/04/12
    created:  12:4:2011   18:15
    filename: LogInitializer.h
    author:	  Piotr Gwiazdowski

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LOGINITIALIZER_H__
#define HEADER_GUARD_CORE__LOGINITIALIZER_H__

#include <cstddef>
#include <loglib/ILog.h>
#include <utils/Filesystem.h>

namespace coreUI {

class CoreLogWidget;

}

namespace core {

//! Obiekt logu. Powinien być tworzony w głównej binarce na samym początku maina.
class LogInitializer
{
public:
    //! \param configPath Ścieżka do pliku konfiguracyjnego.
	LogInitializer(const utils::Filesystem::Path & configFile);
    //!
    ~LogInitializer();

public:
    //! \param log Widget konsoli.    
    void setLogWidget(coreUI::CoreLogWidget* widget);
};

}

#endif  // HEADER_GUARD_CORE__LOGINITIALIZER_H__111