/********************************************************************
	created:	2013/03/14
	created:	14:3:2013   14:25
	filename: 	SimpleConnectionStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLECONNECTIONSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__SIMPLECONNECTIONSTRATEGY_H__

#include <plugins/newVdf/IConnectionStrategy.h>
#include <plugins/newVdf/IVisualConnection.h>
#include <plugins/newVdf/IVisualPin.h>


namespace vdf {

class IVisualOutputPin;
class IVisualInputPin;
class SimpleConnectionStrategy : public IConnectionStrategy
{
public:
	SimpleConnectionStrategy();
	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr );
	virtual const QRectF& getRect();
    QPainterPath shape() const;
    virtual void setConnection( utils::weak_ptr<IVisualConnection> connection );

private:
    bool hasPins();

private:
	IVisualPinWeakPtr beginPin;
	IVisualPinWeakPtr endPin;
    IVisualConnectionWeakPtr connection;
	QRectF rect;
    QPainterPath path;
};
DEFINE_SMART_POINTERS(SimpleConnectionStrategy);
}

#endif
