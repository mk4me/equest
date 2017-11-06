#include "CoreLibPCH.h"
#include <corelib/HierarchyItem.h>

#include <QtCore/QStringList>
#include <loglib/Exceptions.h>

using namespace core;

HierarchyItem::HierarchyItem( const QString& name, const QString& description, const QIcon& icon) :
    icon(icon),
    name(name),
    desc(description)
{
}

HierarchyItem::~HierarchyItem()
{
}

void HierarchyItem::appendChild(IHierarchyItemPtr item)
{
    childItems.append(item);
    IHierarchyItemPtr ptr = utils::const_pointer_cast<IHierarchyItem>(item);
    auto thisShared = shared_from_this();
    if (ptr->getParent() != thisShared) {
        ptr->setParent(thisShared);
    }
}

IHierarchyItemConstPtr HierarchyItem::getChild(int row) const
{
    if (row < 0 || row >= getNumChildren()) {
        throw std::out_of_range("HierarchyItem : wrong child index");
    }
    return childItems.value(row);
}

int HierarchyItem::getNumChildren() const
{
    return childItems.count();
}


IHierarchyItemConstPtr HierarchyItem::getParent() const
{
    return parentItem.lock();
}

int HierarchyItem::getIndex() const
{
    auto parent = parentItem.lock();
    if (parent) {
        return parent->getChildIndex(shared_from_this());
    }
    return 0;
}

QIcon HierarchyItem::getIcon() const
{
    return icon;
}

QString HierarchyItem::getName() const
{
    return name;
}

int HierarchyItem::getChildIndex( IHierarchyItemConstPtr child ) const
{
    return childItems.indexOf(utils::const_pointer_cast<IHierarchyItem>(child));
}

void HierarchyItem::setParent( IHierarchyItemPtr parent )
{
    if (this->getParent() && this->getParent()->getChildIndex(shared_from_this()) != -1) {
        auto ptr = utils::const_pointer_cast<IHierarchyItem>(this->getParent());
        ptr->removeChild(shared_from_this());
    }

    this->parentItem = parent;
    if (parent && parent->getChildIndex(shared_from_this()) == -1) {
        auto ptr = utils::const_pointer_cast<IHierarchyItem>(parent);
        ptr->appendChild(shared_from_this());
    }
}

void HierarchyItem::removeChild( IHierarchyItemPtr child )
{
    if(childItems.indexOf(child) != -1) {
        childItems.removeOne(child);
        //auto ptr = utils::const_pointer_cast<IHierarchyItem>(child);
        child->setParent(IHierarchyItemPtr());
    } else {
        throw std::runtime_error("HierarchyItem : child was not found");
    }
}

QString core::HierarchyItem::getDescription() const
{
    return desc;
}

core::IHierarchyItemPtr core::HierarchyItem::shallowCopy(bool withChildren) const
{
    auto hi = utils::make_shared<core::HierarchyItem>(*this);
    hi->childItems.clear();

    if (withChildren) {
        for (auto it = childItems.begin(); it != childItems.end(); ++it) {
            hi->appendChild((*it)->shallowCopy());
        }
    }
    return hi;
}

void core::HierarchyItem::setIcon( const QIcon& icon )
{
    this->icon = icon;
}
