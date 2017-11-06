/********************************************************************
	created:	2013/02/22
	created:	22:2:2013   11:38
	filename: 	DataSink.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DATASINK_H__
#define HEADER_GUARD_NEWVDF__DATASINK_H__

#include <plugins/newVdf/IDataSink.h>
#include <plugins/newVdf/Export.h>
#include <functional>

namespace df { class ISinkNode; }

namespace vdf {

class PLUGIN_NEWVDF_EXPORT DataSink : public IDataSink
{
public:
	typedef std::function<df::ISinkNode* (const df::ISinkNode*)> Creator;
	DataSink(df::ISinkNode* impl, core::UniqueID id, const std::string& name, Creator creator );
	//! \return Nazwa elementu logicznego
	virtual const std::string getName() const;;
	virtual df::INode* getPrototype() const;
	virtual df::INode* create() const;
	virtual core::UniqueID getID() const;
	virtual void getInputInfo( std::vector<InputInfo>& info ) const;

	bool  empty(void) const;
    QIcon getIcon() const { return icon; }
    void setIcon(const QIcon& val) { icon = val; }

private:
    QIcon icon;
	df::ISinkNode* _impl;
	std::string _desc;
	std::string _name;
	core::UniqueID _id;
	Creator _creator;
};
DEFINE_SMART_POINTERS(DataSink);
}

#endif
