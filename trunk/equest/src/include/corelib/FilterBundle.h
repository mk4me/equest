/********************************************************************
	created:	2013/05/24
	created:	24:5:2013   13:00
	filename: 	FilterBundle.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORELIB__FILTERBUNDLE_H__
#define HEADER_GUARD_CORELIB__FILTERBUNDLE_H__

#include <vector>
#include <corelib/Export.h>
#include <corelib/IFilterCommand.h>
#include <corelib/IFilterBundle.h>

namespace core {

class CORELIB_EXPORT FilterBundle : public IFilterBundle
{
public:
    FilterBundle(const QString& name, const QIcon& icon = QIcon()) : name(name), icon(icon) {}
	virtual ~FilterBundle() {}

public:
    virtual void addFilterCommand(IFilterCommandPtr command);
    virtual IFilterCommandPtr getFilter( int idx );
    virtual int genNumFilters() const;

    virtual QString getName() const { return name; }
    virtual QIcon getIcon() const { return icon; }
    
private:
    std::vector<IFilterCommandPtr> commands;
    QIcon icon;
    QString name;
};
DEFINE_SMART_POINTERS(FilterBundle);
}
#endif
