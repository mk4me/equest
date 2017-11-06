/********************************************************************
	created:	2013/11/23
	created:	23:11:2013   21:44
	filename: 	MultiCommand.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__MULTICOMMAND_H__
#define HEADER_GUARD_COREUI__MULTICOMMAND_H__

#include <vector>
#include <utils/ICommand.h>
#include <coreui/Export.h>

namespace coreUI {

    //! Polecenie agreguje inne polecenia
    class COREUI_EXPORT MultiCommand : public utils::ICommand
    {
    public:
        //! Konstruktor
        //! \param c Wektor z poleceniami do wykonania
        MultiCommand(const std::vector<utils::ICommandPtr>& c);
    public:
        //! Kolejno wykonuje dostarczone polecenia
        virtual void doIt();
        //! Cofa polecenia w odwrotnej kolejnoœci do wykonania
        virtual void undoIt();
        //! Nazwa polecenia 
        virtual QString name() { return QString(typeid(this).name()); }

    private:
        //! Dostarczone polecenia do wykonania
        std::vector<utils::ICommandPtr> commands;
    };
    DEFINE_SMART_POINTERS(MultiCommand);

}

#endif
