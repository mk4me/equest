/********************************************************************
	created:	2013/05/24
	created:	24:5:2013   13:00
	filename: 	IFilterBundle.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORELIB__IFILTERBUNDLE_H__
#define HEADER_GUARD_CORELIB__IFILTERBUNDLE_H__

#include <corelib/IFilterCommand.h>

namespace core {

class IFilterBundle
{
public:
	virtual ~IFilterBundle() {}
    virtual void addFilterCommand(IFilterCommandPtr command) = 0;
    virtual IFilterCommandPtr getFilter(int idx) = 0;
    virtual int genNumFilters() const = 0;
    virtual QString getName() const = 0;
    virtual QIcon getIcon() const = 0;
};
DEFINE_SMART_POINTERS(IFilterBundle);
}
#endif
