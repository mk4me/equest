#include <dflib/Connection.h>

namespace df {

	Connection::Connection(IOutputPin * src, IInputPin * dest)
		: source_(src), destination_(dest)
	{

	}

Connection::~Connection(void)
{

}

IOutputPin * Connection::source()
{
	return source_;
}

IInputPin * Connection::destination()
{
	return destination_;
}

const IOutputPin * Connection::source() const
{
	return source_;
}

const IInputPin * Connection::destination() const
{
	return destination_;
}

}