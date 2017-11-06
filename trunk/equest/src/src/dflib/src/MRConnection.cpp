#include "MRConnection.h"

MRConnection::MRConnection(MROutputPin * src, MRInputPin * dest, df::IConnection * connection) : source_(src), destination_(dest), innerConnection_(connection)
{

}

MRConnection::~MRConnection()
{

}

MROutputPin * MRConnection::source()
{
	return source_;
}

MRInputPin * MRConnection::destination()
{
	return destination_;
}

const MROutputPin * MRConnection::source() const
{
	return source_;
}

const MRInputPin * MRConnection::destination() const
{
	return destination_;
}

df::IConnection * MRConnection::innerConnection()
{
	return innerConnection_;
}

const df::IConnection * MRConnection::innerConnection() const
{
	return innerConnection_;
}