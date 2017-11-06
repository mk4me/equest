#include "HmmLibPCH.h"
#include "SubjectFilterCommand.h"
#include <corelib/HierarchyItem.h>
#include <corelib/IHierarchyDataItem.h>


SimpleFilterCommand::SimpleFilterCommand( const std::set<utils::TypeInfo>& typeinfos, const QString& name, const QIcon& icon ) :
    core::AbstractFilterCommand(name, icon),
    typeinfos(typeinfos)
{
}

SimpleFilterCommand::SimpleFilterCommand( const utils::TypeInfo& typeinfo, const QString& name, const QIcon& icon ) :
    core::AbstractFilterCommand(name, icon)
{
    typeinfos.insert(typeinfo);
}

bool SimpleFilterCommand::checkItem( core::IHierarchyItemConstPtr item )
{
    core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
    if (dataItem && dataItem->getData() ) {
        auto info = dataItem->getData()->data()->getTypeInfo();
        if (typeinfos.find(info) != typeinfos.end()) {
            return true; 
        }
    }

    return false;
}




