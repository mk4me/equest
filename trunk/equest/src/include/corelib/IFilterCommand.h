/********************************************************************
	created:	2013/05/24
	created:	24:5:2013   13:01
	filename: 	IFilterCommand.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORELIB__IFILTERCOMMAND_H__
#define HEADER_GUARD_CORELIB__IFILTERCOMMAND_H__

#include <corelib/IHierarchyItem.h>

namespace core {

//! interfejs odpowiada za tworzenie przefiltrowanego drzewa danych lub jego czesci
class IFilterCommand
{
public:
    //! jeśli komenda dostarcza konfiguratora to powinien on zwrocic jedna z tych wartości
    enum ConfigurationResult
    {
        OK,
        Cancel
    };

public:
	virtual ~IFilterCommand() {}

public:
    virtual IHierarchyItemPtr getFilteredTree(IHierarchyItemConstPtr root) = 0;
    //! resetuje ustawienia konfiguratora
    virtual void reset() {}
    //! \return widget z konfiguratorem lub nullptr jeśli nie jest on dostarczany
    virtual QWidget* getConfigurationWidget() { return nullptr; }
    //! \return nazwa filtru używana w GUI
    virtual QString getName() const = 0;
    virtual QIcon getIcon() const = 0;
};
DEFINE_SMART_POINTERS(IFilterCommand)

}
#endif
