/********************************************************************
	created:	2013/02/08
	created:	8:2:2013   15:20
	filename: 	DarkConnectionsFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKCONNECTIONSFACTORY_H__
#define HEADER_GUARD_NEWVDF__DARKCONNECTIONSFACTORY_H__

#include <plugins/newVdf/IConnectionsFactory.h>

namespace vdf {

class DarkConnectionsFactory : public IConnectionsFactory
{
public:
	virtual ~DarkConnectionsFactory() {}

	virtual IVisualConnectionPtr createConnection() const;

	virtual const QUuid id() const;

	virtual const QString name() const;

	virtual const QString description() const;

};
DEFINE_SMART_POINTERS(DarkConnectionsFactory);
}

#endif
