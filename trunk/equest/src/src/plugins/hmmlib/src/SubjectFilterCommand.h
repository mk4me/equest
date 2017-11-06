/********************************************************************
	created:	2011/10/07
	created:	7:10:2011   9:59
	filename: 	FilterCommand.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMMLIB__FILTERCOMMAND_H__
#define HEADER_GUARD_HMMLIB__FILTERCOMMAND_H__

#include <set>
#include <utils/SmartPtr.h>
#include <corelib/IFilterCommand.h>
#include <corelib/HierarchyItem.h>
#include <corelib/AbstractFilterCommand.h>
#include <utils/TypeInfo.h>
#include <plugins/subject/Types.h>
#include <plugins/subject/SubjectDataFilters.h>

//! komenda bez konfiguratora, która tworzy przefiltrowane drzewo na podstawie filtra danych
class SimpleFilterCommand : public core::AbstractFilterCommand
{
public:
    //! Konstruktor
    SimpleFilterCommand(const std::set<utils::TypeInfo>& typeinfos, const QString& name, const QIcon& icon);

    SimpleFilterCommand(const utils::TypeInfo& typeinfo, const QString& name, const QIcon& icon);

protected:
    virtual bool checkItem( core::IHierarchyItemConstPtr item );

private:
    std::set<utils::TypeInfo> typeinfos;
};
typedef utils::shared_ptr<SimpleFilterCommand> SimpleFilterCommandPtr;
typedef utils::shared_ptr<const SimpleFilterCommand> SimpleFilterCommandConstPtr;


#endif
