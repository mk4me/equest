/********************************************************************
	created:	2013/02/22
	created:	22:2:2013   11:31
	filename: 	DataProcessor.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DATAPROCESSOR_H__
#define HEADER_GUARD_NEWVDF__DATAPROCESSOR_H__

#include <plugins/newVdf/IDataProcessor.h>
#include <plugins/newVdf/Export.h>
#include <functional>

namespace df { class IProcessingNode; }

namespace vdf {

class PLUGIN_NEWVDF_EXPORT DataProcessor : public IDataProcessor
{
public:
	typedef std::function<df::IProcessingNode* (const df::IProcessingNode*)> Creator;

public:
	DataProcessor(df::IProcessingNode* impl, core::UniqueID id, const std::string& name, Creator creator);
    //virtual ~DataProcessor() { delete impl; }

public:
	//! \return Nazwa elementu logicznego
	virtual const std::string getName() const;;
	virtual void getOutputInfo( std::vector<OutputInfo> & output ) const;

	virtual core::UniqueID getID() const;
	virtual df::INode* getPrototype() const;
	virtual df::INode* create() const;
	virtual void getInputInfo( std::vector<InputInfo>& info ) const;
    QIcon getIcon() const { return icon; }
    void setIcon(const QIcon& val) { icon = val; }

private:
    QIcon icon;
	utils::unique_ptr<df::IProcessingNode> _impl;
	std::string _desc;
	std::string _name;
	core::UniqueID _id;
	Creator _creator;
};
DEFINE_SMART_POINTERS(DataProcessor);

}

#endif
