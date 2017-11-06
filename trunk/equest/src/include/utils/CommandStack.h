/********************************************************************
	created:	2013/03/12
	created:	12:3:2013   21:14
	filename: 	CommandStack.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__COMMANDSTACK_H__
#define HEADER_GUARD_UTILS__COMMANDSTACK_H__

#include <utils/ICommandStack.h>

namespace utils {
			
//! Przechowuje polecenia i zarz¹dza nimi. Umo¿liwia standardowe operacje undo/redo.
class CommandStack : public QObject, public ICommandStack
{
    Q_OBJECT;
    friend class CommandStackDebug;
public:
    using Commands = std::list<ICommandPtr>;
public:
    CommandStack();
    virtual ~CommandStack() {}

public:
	//! Dodaje polecenie do stosu i automatycznie wykonuje je.
	//! \param command dodawane polecenie
    void addCommand(ICommandPtr command);
	//! Cofa ostatnio wykonane polecenie.
    void undo();
	//! Ponownie wykonuje ostatnio cofniête polecenie
    void redo();
	//! Sprawdza, czy na stosie jest choæ jedno wykonane polecenie
	//! \return Czy mo¿na cofn¹æ efekt dzia³ania polecenia.
	bool isUndoPossible() const;
	//! Sprawdza, czy na stosie znajduje siê polecenie które zosta³o cofniête.
	//! \return Czy mo¿na ponownie wykonaæ polecenie?
    bool isRedoPossible() const;
	//! Usuwa wszystkie polecenia ze stosu
    void clear();

signals:
	//! Stan Stosu zmieni³ siê
    void changed();

private:
	//! Wszystkie agregowane polecenia
    Commands commands;
	//! Aktualne polecenie (ostatnio wykonane, za nim mog¹ znaleŸæ siê polecenia cofniête)
    Commands::iterator currentCommand;
};

DEFINE_SMART_POINTERS(CommandStack);
}

#endif
