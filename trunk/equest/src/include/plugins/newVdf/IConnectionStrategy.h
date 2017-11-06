/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:17
	filename: 	IConnectionStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__ICONNECTIONSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__ICONNECTIONSTRATEGY_H__

#include <plugins/newVdf/IVisualStrategy.h>
#include <QtGui/QPainterPath>

namespace vdf {

class IVisualPin;
class IVisualConnection;
class IConnectionStrategy : public IVisualStrategy
{
public:
    virtual void setConnection(utils::weak_ptr<IVisualConnection> connection) = 0;
    virtual QPainterPath shape() const = 0;
};
DEFINE_SMART_POINTERS(IConnectionStrategy);

}
#endif
