#include <dflib/INode.h>
#include <dflib/IPin.h>

namespace df
{

void ISourceNode::connections(Connections & connections) const
{
	auto s = outputSize();
	for(size_type i = 0; i < s; ++i)
	{
		auto * pin = outputPin(i);
		auto ps = pin->size();
		for(int j = 0; j < ps; ++j)
		{
			connections.push_back(const_cast<IConnection*>(pin->connection(j)));
		}
	}
}

void ISinkNode::connections(Connections & connections) const
{
	auto s = inputSize();
	for(size_type i = 0; i < s; ++i)
	{
		const IInputPin * pin = inputPin(i);
		auto ps = pin->size();
		for(int j = 0; j < ps; ++j)
		{
			connections.push_back(const_cast<IConnection*>(pin->connection(j)));
		}
	}
}

}
