/********************************************************************
	created:	2013/02/07
	created:	7:2:2013   13:54
	filename: 	IVisualStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__IVISUALSTRATEGY_H__

#include <utils/SmartPtr.h>
#include <QtCore/QRect>

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace vdf {

class IVisualItem;
class IVisualStrategy
{
public:
	virtual ~IVisualStrategy() {}
	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr ) = 0;
	virtual QWidget* getWidget() { return nullptr; }
	virtual void update() {}
	virtual const QRectF& getRect() = 0;
};
DEFINE_SMART_POINTERS(IVisualStrategy);
}

#endif
