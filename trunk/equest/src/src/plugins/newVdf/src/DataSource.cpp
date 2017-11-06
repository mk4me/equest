#include <boost/lexical_cast.hpp>
#include "NewVdfPCH.h"
#include <plugins/newVdf/DataSource.h>
#include <dflib/Node.h>

using namespace vdf;

DataSource::DataSource( df::ISourceNode* impl, core::UniqueID id, const std::string& name, Creator creator ) :
_impl(impl), _id(id), _name(name), _creator(creator)
{
}

core::UniqueID DataSource::getID() const
{
	return _id;
}

void DataSource::getOutputInfo( std::vector<OutputInfo> & output ) const
{
	int size = _impl->outputSize();
	for (int i = 0; i < size; i++) {
		OutputInfo pin;
		pin.name = "out" + boost::lexical_cast<std::string>(i);
		output.push_back(pin);
	}
}

df::INode* DataSource::create() const
{
	return _creator(_impl);
}

df::INode* DataSource::getPrototype() const
{
	return _impl;
}

const std::string DataSource::getName() const
{
	return _name;
}

