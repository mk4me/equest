#include "STMRPin.h"
#include "ISTMRNode.h"
#include "STMRConnection.h"
#include <algorithm>
#include <stdexcept>

STMRInputPin::STMRInputPin(ISTMRSink * sink, df::IInputPin * pin, df::IDFInput * dfInput) : sink_(sink), pin_(pin), dfInput_(dfInput), consumed_(false), outputsReady(0)
{

}

STMRInputPin::~STMRInputPin()
{

}

ISTMRSink * STMRInputPin::sink()
{
	return sink_;
}

const ISTMRSink * STMRInputPin::sink() const
{
	return sink_;
}

void STMRInputPin::reset()
{
	dfInput_->reset();
}

void STMRInputPin::update()
{
	consumed_ = false;
	++outputsReady;
	if(outputsReady == connections.size())
	{
		sink_->updateSnk();	
	}
}

const bool STMRInputPin::updated() const
{
	return outputsReady == connections.size();
}

void STMRInputPin::consumeData()
{
	try{
		for(auto it = connections.begin(); it != connections.end(); ++it){
			dfInput_->copyData((*it)->source()->dfOutput());
			(*it)->source()->consumeData();
		}
		outputsReady = 0;
		consumed_ = true;
	}catch(std::exception &){
		
	}catch(...){

	}
}

const bool STMRInputPin::dataConsumed() const
{
	return consumed_;
}

STMRConnection * STMRInputPin::connection(size_type idx)
{
	return connections[idx];
}

const STMRConnection * STMRInputPin::connection(size_type idx) const
{
	return connections[idx];
}

void STMRInputPin::addConnection(STMRConnection * connection)
{
	if(std::find(connections.begin(), connections.end(), connection) != connections.end())
	{
		throw std::runtime_error("Connection already registered in pin");
	}

	connections.push_back(connection);
}

STMRInputPin::size_type STMRInputPin::size() const
{
	return connections.size();
}

const bool STMRInputPin::empty() const
{
	return connections.empty();
}

df::IInputPin * STMRInputPin::innerPin()
{
	return pin_;
}

const df::IInputPin * STMRInputPin::innerPin() const
{
	return pin_;
}

df::IDFInput * STMRInputPin::dfInput()
{
	return dfInput_;
}

const df::IDFInput * STMRInputPin::dfInput() const
{
	return dfInput_;
}

STMROutputPin::STMROutputPin(ISTMRSource * source, df::IOutputPin * pin, df::IDFOutput * dfOutput) : source_(source), pin_(pin), dfOutput_(dfOutput), updated_(false), inputsConsumed(0)
{

}

STMROutputPin::~STMROutputPin()
{

}

ISTMRSource * STMROutputPin::source()
{
	return source_;
}

const ISTMRSource * STMROutputPin::source() const
{
	return source_;
}

void STMROutputPin::reset()
{
	dfOutput_->reset();
}

void STMROutputPin::update()
{
	inputsConsumed = 0;
	updated_ = true;

	for(auto it = connections.begin(); it != connections.end(); ++it)
	{
		(*it)->destination()->update();
	}
}

const bool STMROutputPin::updated() const
{
	return updated_;
}

void STMROutputPin::consumeData()
{
	++inputsConsumed;
	if(inputsConsumed == connections.size())
	{
		//source_->updateSrc();
		updated_ = false;
	}
}

const bool STMROutputPin::dataConsumed() const
{
	return inputsConsumed == connections.size();
}

STMRConnection * STMROutputPin::connection(size_type idx)
{
	return connections[idx];
}

const STMRConnection * STMROutputPin::connection(size_type idx) const
{
	return connections[idx];
}

void STMROutputPin::addConnection(STMRConnection * connection)
{
	if(std::find(connections.begin(), connections.end(), connection) != connections.end())
	{
		throw std::runtime_error("Connection already registered in pin");
	}

	connections.push_back(connection);
}

STMROutputPin::size_type STMROutputPin::size() const
{
	return connections.size();
}

const bool STMROutputPin::empty() const
{
	return connections.empty();
}

df::IOutputPin * STMROutputPin::innerPin()
{
	return pin_;
}

const df::IOutputPin * STMROutputPin::innerPin() const
{
	return pin_;
}

df::IDFOutput * STMROutputPin::dfOutput()
{
	return dfOutput_;
}

const df::IDFOutput * STMROutputPin::dfOutput() const
{
	return dfOutput_;
}
