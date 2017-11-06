/********************************************************************
	created:	2013/10/02
	created:	2:10:2013   10:52
	filename: 	TypeFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__TYPEFILTER_H__
#define HEADER_GUARD_NEWVDF__TYPEFILTER_H__

#include <corelib/AbstractFilterCommand.h>

namespace vdf {

template<class T>
class TypeFilter : public core::AbstractFilterCommand
{
public:
    TypeFilter() : core::AbstractFilterCommand("", QIcon()) {}
    virtual ~TypeFilter() {}

    virtual bool checkItem( core::IHierarchyItemConstPtr item )
    {
        core::IHierarchyDataItemConstPtr data = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
        if (data) {
			return data->getData() ? data->getData()->data()->isSupported(typeid(T)) : false;
        } else {
            return false;
        }
    }

};

}

#endif