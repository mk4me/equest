/********************************************************************
	created:	2012/12/26
	created:	26:12:2012   22:37
	filename: 	Command.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__ICOMMAND_H__
#define HEADER_GUARD_UTILS__ICOMMAND_H__

#include <utils/SmartPtr.h>
#include <QtCore/QObject>

namespace utils {

//! podstawowy interfejs reprezentuj¹cy pojedyncze polecenie (zgodnie ze wzorcem)
class ICommand
{
public:
	virtual ~ICommand() {}

public:
    //! metoda wykonuje polecenie
    virtual void doIt() = 0;
    //! cofa wykonane ju¿ polecenie
    virtual void undoIt() = 0;
	//! \return nazwa polecenia (dla ewentualnej reprezentacji stosu poleceñ)
	virtual QString name() = 0; 
};
DEFINE_SMART_POINTERS(ICommand);
}

#endif
