/********************************************************************
	created:	2013/02/22
	created:	22:2:2013   11:35
	filename: 	DataSource.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DATASOURCE_H__
#define HEADER_GUARD_NEWVDF__DATASOURCE_H__

#include <plugins/newVdf/IDataSource.h>
#include <plugins/newVdf/Export.h>
#include <functional>

namespace df { class ISourceNode; }

namespace vdf {

class PLUGIN_NEWVDF_EXPORT DataSource : public IDataSource
{
public:
	typedef std::function<df::ISourceNode* (const df::ISourceNode*)> Creator;
	DataSource(df::ISourceNode* impl, core::UniqueID id, const std::string& name, Creator creator);
	//! \return Nazwa elementu logicznego
	virtual const std::string getName() const;;
	virtual df::INode* getPrototype() const;
	virtual df::INode* create() const;
	virtual void getOutputInfo( std::vector<OutputInfo> & output ) const;
    virtual core::UniqueID getID() const;

    QIcon getIcon() const { return icon; }
    void setIcon(const QIcon& val) { icon = val; }

private:
    QIcon icon;
	df::ISourceNode* _impl;
	std::string _desc;
	std::string _name;
	core::UniqueID _id;
	Creator _creator;
};
DEFINE_SMART_POINTERS(DataSource);
}

#endif
