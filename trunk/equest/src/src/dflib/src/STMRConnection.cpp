#include "STMRConnection.h"

STMRConnection::STMRConnection(STMROutputPin * src, STMRInputPin * dest, df::IConnection * connection) : source_(src), destination_(dest), innerConnection_(connection)
{

}

STMRConnection::~STMRConnection()
{

}

STMROutputPin * STMRConnection::source()
{
	return source_;
}

STMRInputPin * STMRConnection::destination()
{
	return destination_;
}

const STMROutputPin * STMRConnection::source() const
{
	return source_;
}

const STMRInputPin * STMRConnection::destination() const
{
	return destination_;
}

df::IConnection * STMRConnection::innerConnection()
{
	return innerConnection_;
}

const df::IConnection * STMRConnection::innerConnection() const
{
	return innerConnection_;
}
