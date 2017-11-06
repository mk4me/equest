/********************************************************************
    created:  2012/12/10
    created:  10:12:2012   16:22
    filename: MRConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___STMRCONNECTION_H__
#define HEADER_GUARD___STMRCONNECTION_H__

#include <dflib/IConnection.h>

class STMRInputPin;
class STMROutputPin;

class STMRConnection
{
public:
	STMRConnection(STMROutputPin * src, STMRInputPin * dest, df::IConnection * connection);
	virtual ~STMRConnection();

public:

	//! \return Węzeł źródłowy
	STMROutputPin * source();

	//! \return Węzeł docelowy
	STMRInputPin * destination();

	//! \return Węzeł źródłowy
	const STMROutputPin * source() const;

	//! \return Węzeł docelowy
	const STMRInputPin * destination() const;

	df::IConnection * innerConnection();
	const df::IConnection * innerConnection() const;

private:
	STMROutputPin * source_;
	STMRInputPin * destination_;
	df::IConnection * innerConnection_;
};

#endif	//	HEADER_GUARD___MRCONNECTION_H__
