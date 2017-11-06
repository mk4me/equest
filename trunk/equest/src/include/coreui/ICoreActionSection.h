/********************************************************************
    created:  2013/01/18
    created:  18:1:2013   17:02
    filename: ICoreActionSection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ICOREACTIONSECTION_H__
#define HEADER_GUARD___ICOREACTIONSECTION_H__

#include <QtCore/QString>

namespace coreUI {

class ICoreActionSection
{
public:
	//! \return Nazwa sekcji w jakiej powinna znaleŸæ siê akcja
	virtual const QString section() const = 0;

};

}

#endif	//	HEADER_GUARD___ICOREACTIONSECTION_H__
