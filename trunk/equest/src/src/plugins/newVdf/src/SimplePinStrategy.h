/********************************************************************
	created:	2013/02/07
	created:	7:2:2013   13:17
	filename: 	DarkPinStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKPINSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__DARKPINSTRATEGY_H__

#include <plugins/newVdf/IPinStrategy.h>
#include <plugins/newVdf/IVisualPin.h>

namespace vdf {

class SimplePinStrategy : public IPinStrategy
{
public:
	SimplePinStrategy();
	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr );
	virtual void setPin( IVisualPinWeakPtr item );
	virtual const QRectF& getRect();

	virtual void setState( State s );

    virtual QPointF getConnectionEndShift();

private:
	QRectF rect;
	IVisualPinWeakPtr pin;
	QColor stateColor;
};
DEFINE_SMART_POINTERS(SimplePinStrategy);
}

#endif
