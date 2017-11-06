/********************************************************************
    created:  2012/11/16
    created:  16:11:2012   9:39
    filename: IConnectionsFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ICONNECTIONSFACTORY_H__
#define HEADER_GUARD___ICONNECTIONSFACTORY_H__

#include <plugins/newVdf/IFactory.h>
#include <plugins/newVdf/IVisualConnection.h>

namespace vdf {

class IConnectionsFactory : public IFactory
{
public:
	virtual IVisualConnectionPtr createConnection() const = 0;
};
DEFINE_SMART_POINTERS(IConnectionsFactory);

}

#endif	//	HEADER_GUARD___ICONNECTIONSFACTORY_H__
