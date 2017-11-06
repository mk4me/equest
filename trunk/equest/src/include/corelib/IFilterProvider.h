/********************************************************************
	created:	2013/05/24
	created:	24:5:2013   10:47
	filename: 	IFilterProvider.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORELIB__IFILTERPROVIDER_H__
#define HEADER_GUARD_CORELIB__IFILTERPROVIDER_H__

#include <list>
#include <corelib/IFilterBundle.h>

namespace core {

class IFilterProvider
{
public:
	virtual ~IFilterProvider() {}
    typedef std::list<core::IFilterBundlePtr> FilterBundles;
    virtual std::list<IFilterBundlePtr> getFilterBundles() const = 0;
};
DEFINE_SMART_POINTERS(IFilterProvider);
}



#endif
