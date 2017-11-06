/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:18
	filename: 	INodeStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__INODESTRATEGY_H__
#define HEADER_GUARD_NEWVDF__INODESTRATEGY_H__

#include <plugins/newVdf/IVisualStrategy.h>

namespace vdf {

class IVisualNode;
class INodeStrategy : public IVisualStrategy
{
public:
	enum State {
		Normal,
		Selected,
        Invalid,
        Valid
	};
	virtual void setState(State state) = 0;
	virtual void setNode(utils::weak_ptr<IVisualNode> item) = 0;
	virtual QPointF getPinPosition(int no, bool input) = 0;
};
DEFINE_SMART_POINTERS(INodeStrategy);

}

#endif
