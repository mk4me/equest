/********************************************************************
	created:	2013/05/07
	created:	7:5:2013   14:18
	filename: 	HierarchyItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__HIERARCHYITEM_H__
#define HEADER_GUARD_CORE__HIERARCHYITEM_H__

#include <QtCore/QList>
#include <QtCore/QVariant>
//#include <objectwrapperlib/ObjectWrapper.h>
#include <corelib/IHierarchyItem.h>
#include <corelib/Export.h>

namespace core {

class CORELIB_EXPORT HierarchyItem : public IHierarchyItem, public utils::enable_shared_from_this<HierarchyItem>
{
public:
    HierarchyItem(const QString& name, const QString& description, const QIcon& icon = QIcon() );
    virtual ~HierarchyItem();

public:
    virtual void appendChild(IHierarchyItemPtr child);
    virtual void removeChild(IHierarchyItemPtr child);
    virtual IHierarchyItemConstPtr getChild(int row) const;
    virtual int getNumChildren() const;
    virtual int getIndex() const;
    virtual int getChildIndex(IHierarchyItemConstPtr child) const;
    virtual IHierarchyItemConstPtr getParent() const;
    virtual QString getName() const;
    virtual QIcon getIcon() const;
    void setIcon(const QIcon& icon);
    virtual void setParent(IHierarchyItemPtr parent);

    virtual QString getDescription() const;

    virtual IHierarchyItemPtr shallowCopy(bool withChildren = true) const;

protected:
    QList<IHierarchyItemConstPtr> childItems;
    QString name;
    QString desc;
    QIcon icon;
    IHierarchyItemConstWeakPtr parentItem;
};
DEFINE_SMART_POINTERS(HierarchyItem)

}

#endif
