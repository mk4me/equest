#include <boost/lexical_cast.hpp>
#include "NewVdfPCH.h"
#include <plugins/newVdf/DataProcessor.h>
#include <dflib/Node.h>

using namespace vdf;

DataProcessor::DataProcessor( df::IProcessingNode* impl, core::UniqueID id, const std::string& name, Creator creator ) :
    _impl(impl), _id(id), _name(name), _creator(creator)
{
}


void DataProcessor::getInputInfo( std::vector<InputInfo>& info ) const
{
	int size = _impl->inputSize();
	for (int i = 0; i < size; i++) {
		InputInfo pin;
		pin.name = "in" + boost::lexical_cast<std::string>(i);
		info.push_back(pin);
	}
}

df::INode* DataProcessor::create() const
{
	return _creator(_impl.get());
}

df::INode* DataProcessor::getPrototype() const
{
    // todo zwrocic smart_ptr
	return _impl.get();
}

core::UniqueID DataProcessor::getID() const
{
	return _id;
}

void DataProcessor::getOutputInfo( std::vector<OutputInfo> & output ) const
{
	int size = _impl->outputSize();
	for (int i = 0; i < size; i++) {
		OutputInfo pin;
		pin.name = "out" + boost::lexical_cast<std::string>(i);
		output.push_back(pin);
	}
}

const std::string DataProcessor::getName() const
{
	return _name;
}
