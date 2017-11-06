/********************************************************************
	created:	2013/03/12
	created:	12:3:2013   21:11
	filename: 	ICommandStack.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__ICOMMANDSTACK_H__
#define HEADER_GUARD_UTILS__ICOMMANDSTACK_H__

#include <utils/ICommand.h>

namespace utils {

//! Przechowuje polecenia i zarz¹dza nimi. Umo¿liwia standardowe operacje undo/redo.
class ICommandStack
{
public:
	virtual ~ICommandStack() {}

public:
    //! Dodaje polecenie do stosu i automatycznie wykonuje je.
    //! \param command dodawane polecenie
    virtual void addCommand(ICommandPtr command) = 0;
    //! Cofa ostatnio wykonane polecenie.
    virtual void undo() = 0;
    //! Ponownie wykonuje ostatnio cofniête polecenie
    virtual void redo() = 0;
	//! Sprawdza, czy na stosie jest choæ jedno wykonane polecenie
    //! \return Czy mo¿na cofn¹æ efekt dzia³ania polecenia.
    virtual bool isUndoPossible() const = 0;
	//! Sprawdza, czy na stosie znajduje siê polecenie które zosta³o cofniête.
    //! \return Czy mo¿na ponownie wykonaæ polecenie?
    virtual bool isRedoPossible() const = 0;
    //! Usuwa wszystkie polecenia ze stosu 
    virtual void clear() = 0;
};
DEFINE_SMART_POINTERS(ICommandStack);

}

#endif