/********************************************************************
	created:	2013/02/08
	created:	8:2:2013   16:17
	filename: 	DarkPinsFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKPINSFACTORY_H__
#define HEADER_GUARD_NEWVDF__DARKPINSFACTORY_H__

#include <plugins/newVdf/IPinsFactory.h>

namespace vdf {

class DarkPinsFactory : public IPinsFactory
{
public:
	virtual ~DarkPinsFactory() {}

public:
	virtual IVisualInputPinPtr createInputPin() const;
	virtual IVisualOutputPinPtr createOutputPin() const;
	virtual const QUuid id() const;
	virtual const QString name() const;
	virtual const QString description() const;
};
DEFINE_SMART_POINTERS(DarkPinsFactory);

}

#endif
