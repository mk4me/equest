#include <boost/lexical_cast.hpp>
#include "NewVdfPCH.h"
#include <plugins/newVdf/DataSink.h>
#include <dflib/Node.h>

using namespace vdf;

DataSink::DataSink( df::ISinkNode* impl, core::UniqueID id, const std::string& name, Creator creator ) :
_impl(impl), _id(id), _name(name), _creator(creator)
{
}

void DataSink::getInputInfo( std::vector<InputInfo>& info ) const
{
	int size = _impl->inputSize();
	for (int i = 0; i < size; i++) {
		InputInfo pin;
		pin.name = "in" + boost::lexical_cast<std::string>(i);
		info.push_back(pin);
	}
}

core::UniqueID DataSink::getID() const
{
	return _id;
}

df::INode* DataSink::create() const
{
	return _creator(_impl);
}

df::INode* DataSink::getPrototype() const
{
	return _impl;
}

const std::string DataSink::getName() const
{
	return _name;
}
