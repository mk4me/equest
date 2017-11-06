/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:18
	filename: 	IPinStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IPINSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__IPINSTRATEGY_H__

#include <plugins/newVdf/IVisualStrategy.h>

namespace vdf {

class IVisualPin;
class IPinStrategy : public IVisualStrategy
{
public:
	enum State {
		Normal,
		Connected,
		Connective,
		Unconnective
	};
	virtual void setPin(utils::weak_ptr<IVisualPin> item) = 0;
	virtual void setState(State s) = 0;
    virtual QPointF getConnectionEndShift() = 0;
};
DEFINE_SMART_POINTERS(IPinStrategy);

}

#endif
