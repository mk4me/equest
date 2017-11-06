/********************************************************************
	created:	2013/08/22
	created:	22:8:2013   20:31
	filename: 	AbstractFilterCommand.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORELIB__ABSTRACTFILTERCOMMAND_H__
#define HEADER_GUARD_CORELIB__ABSTRACTFILTERCOMMAND_H__

#include <corelib/IFilterCommand.h>
#include <corelib/HierarchyItem.h>

namespace core {

//! komenda bez konfiguratora, która tworzy przefiltrowane drzewo na podstawie filtra danych
class CORELIB_EXPORT AbstractFilterCommand : public IFilterCommand
{
public:
    //! Konstruktor
    AbstractFilterCommand(const QString& name, const QIcon& icon);

    virtual IHierarchyItemPtr getFilteredTree( IHierarchyItemConstPtr root );


    virtual QWidget* getConfigurationWidget() { return nullptr; }
    virtual void reset() {}

    virtual QString getName() const { return name; }
    virtual QIcon getIcon() const { return icon; }

protected:
    virtual bool checkItem(IHierarchyItemConstPtr item) = 0;

private:
    void traverse(IHierarchyItemConstPtr source, IHierarchyItemPtr parent);

private:
    //! filtr danych, który będzie użyty do tworzenia drzewa
    QString name;
    QIcon icon;
};
DEFINE_SMART_POINTERS(AbstractFilterCommand);


class NullFilter : public AbstractFilterCommand
{
public:
    NullFilter() : AbstractFilterCommand("", QIcon()) {}
    virtual ~NullFilter() {}
    virtual bool checkItem( IHierarchyItemConstPtr item ) { return true; }
};
DEFINE_SMART_POINTERS(NullFilter);

}


#endif
