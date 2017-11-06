#include "CoreLibPCH.h"
#include <corelib/HierarchyDataItem.h>
#include <corelib/HierarchyHelper.h>

using namespace core;

HierarchyDataItem::HierarchyDataItem(VariantConstPtr wrapper,
    const QIcon& icon, const QString& name, const QString& description, HierarchyHelperPtr helper) :
    HierarchyItem(name, description, icon),
    data(wrapper), defaultHelperIDX(0)
{
    UTILS_ASSERT(helper);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(VariantConstPtr wrapper,
    const QIcon& icon, const QString& name, const QString& description, std::list<HierarchyHelperPtr> helpers) :
    HierarchyItem(name, description, icon),
    helpers(helpers),
    data(wrapper), defaultHelperIDX(-1)
{
#ifdef _DEBUG 
    for (auto it = helpers.begin(); it != helpers.end(); ++it) {
        UTILS_ASSERT(*it);
    }
#endif
}

HierarchyDataItem::HierarchyDataItem( const QIcon& icon, const QString& name, const QString& description ) :
    HierarchyItem(name, description, icon), defaultHelperIDX(-1)
{
    // helpery i data pozostaja niewypelnione danymi
    // obiekt taki nie jest gotowy do uzycia!
}


HierarchyDataItem::HierarchyDataItem(VariantConstPtr wrapper,
    const QIcon& icon, const QString& name, const QString& description) :
    HierarchyItem(name, description, icon),
    data(wrapper), defaultHelperIDX(0)
{
    HierarchyHelperPtr helper = utils::make_shared<WrappedItemHelper>(wrapper);
    UTILS_ASSERT(helper);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(VariantConstPtr wrapper, const QString& description, const QIcon& icon) :
    HierarchyItem( QString::fromStdString(wrapper->data()->getClassName()), description, icon),
    data(wrapper), defaultHelperIDX(0)
{
    HierarchyHelperPtr helper = utils::make_shared<WrappedItemHelper>(wrapper);
    UTILS_ASSERT(helper);
    helpers.push_back(helper);
}

HierarchyDataItem::HierarchyDataItem(const QIcon& icon, const QString& name, const QString& description, HierarchyHelperPtr helper) :
    HierarchyItem( name, description, icon), defaultHelperIDX(0)
{
    UTILS_ASSERT(helper);
    helpers.push_back(helper);
}

HierarchyDataItem::~HierarchyDataItem()
{
}

VariantConstPtr HierarchyDataItem::getData() const
{
    return data;
}

void HierarchyDataItem::addHelper( HierarchyHelperPtr helper )
{
    helper->setParent(HierarchyItem::shared_from_this());
    UTILS_ASSERT(helper);
    helpers.push_back(helper);

	if(defaultHelperIDX == -1){
		defaultHelperIDX = 0;
	}
}

std::list<HierarchyHelperPtr> HierarchyDataItem::getHelpers() const
{
    auto ptr = shared_from_this();

    // Ustawianie parenta helperow, helpery bez parenta to te, ktore byly przekazane w konstruktorze
    // Nie mozna wtedy wywolac shared_from_this
    for (auto it = helpers.begin(); it != helpers.end(); ++it) {
        // TODO : kiedy helper moze byc nullem?
        if (*it) {
            (*it)->setParent(ptr);
        }
    }
    

    return helpers;
}

void HierarchyDataItem::setDefaultHelper(const int idx)
{
	defaultHelperIDX = idx;
}

HierarchyHelperPtr HierarchyDataItem::getDefaultHelper() const
{
	if(defaultHelperIDX > -1){	
		auto ptr = shared_from_this();
		auto it = helpers.begin();
		std::advance(it, defaultHelperIDX);
		auto h = *it;
		h->setParent(ptr);
		return h;
	}

	return HierarchyHelperPtr();
}

IHierarchyItemPtr HierarchyDataItem::shallowCopy(bool withChildren) const
{
    auto hi = utils::make_shared<HierarchyDataItem>(*this);
    hi->childItems.clear();
    if (withChildren) {
        for (auto it = childItems.begin(); it != childItems.end(); ++it) {
            hi->appendChild((*it)->shallowCopy());
        }
    }
    return hi;
}

