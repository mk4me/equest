#include "CoreLibPCH.h"
#include <corelib/AbstractFilterCommand.h>

using namespace core;


AbstractFilterCommand::AbstractFilterCommand( const QString& name, const QIcon& icon ) :
    name(name),
    icon(icon)
{

}

IHierarchyItemPtr AbstractFilterCommand::getFilteredTree( IHierarchyItemConstPtr root )
{
    IHierarchyItemPtr hi = root->shallowCopy(false); //new HierarchyItem(getName(), root->getDescription(), root->getIcon()));
    traverse(root, hi);
    if (hi->getNumChildren() == 1) {
        IHierarchyItemPtr itm = utils::const_pointer_cast<IHierarchyItem>(hi->getChild(0));
        itm->setParent(IHierarchyItemPtr());
        return itm;
    }
    return hi;
}

void AbstractFilterCommand::traverse(IHierarchyItemConstPtr source, IHierarchyItemPtr parent) 
{
    core::IHierarchyItemPtr item;
    bool good = checkItem(source);
    item = good ? 
        source->shallowCopy(false) : 
        utils::make_shared<HierarchyItem>(source->getName(), source->getDescription(), source->getIcon());
    
    auto count = source->getNumChildren();
    for (auto i = 0; i < count; ++i) {
        traverse(source->getChild(i), item);
    }
    
    if (good || item->getNumChildren() > 0) {
        parent->appendChild(item);
    }
    
}
