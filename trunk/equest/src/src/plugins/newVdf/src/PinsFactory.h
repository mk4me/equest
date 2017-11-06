/********************************************************************
	created:	2012/12/16
	created:	16:12:2012   19:38
	filename: 	PinsFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__PINSFACTORY_H__
#define HEADER_GUARD_NEWVDF__PINSFACTORY_H__

#include <plugins/newVdf/IPinsFactory.h>

namespace vdf {

class PinsFactory : public IPinsFactory
{
public:
	virtual ~PinsFactory() {}

public:
    virtual IVisualInputPinPtr createInputPin() const;
    virtual IVisualOutputPinPtr createOutputPin() const;
    virtual const QUuid id() const;
    virtual const QString name() const;
    virtual const QString description() const;
};
DEFINE_SMART_POINTERS(PinsFactory);
}

#endif
