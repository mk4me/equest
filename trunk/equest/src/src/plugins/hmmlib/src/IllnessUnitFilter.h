/********************************************************************
	created:	2013/06/04
	created:	4:6:2013   10:18
	filename: 	IllnessUnitFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM_PLUGIN__ILLNESSUNITFILTER_H__
#define HEADER_GUARD_HMM_PLUGIN__ILLNESSUNITFILTER_H__

#include <corelib/IFilterCommand.h>
#include <corelib/IHierarchyItem.h>

class EndoFilter : public core::IFilterCommand
{
public:
	virtual ~EndoFilter() {}
    virtual core::IHierarchyItemPtr getFilteredTree( core::IHierarchyItemConstPtr root );
    virtual void reset();

};
DEFINE_SMART_POINTERS(EndoFilter);

#endif
