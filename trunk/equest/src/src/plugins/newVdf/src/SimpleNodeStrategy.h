/********************************************************************
	created:	2013/03/14
	created:	14:3:2013   20:57
	filename: 	SimpleNodeStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLENODESTRATEGY_H__
#define HEADER_GUARD_NEWVDF__SIMPLENODESTRATEGY_H__

#include <plugins/newVdf/INodeStrategy.h>
#include <QtCore/QRect>
#include <string>

namespace vdf {

class SimpleNodeStrategy : public INodeStrategy
{
public:
	SimpleNodeStrategy();

	void setContainerStyle(INodeStrategy::State state);

	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr );
	virtual void setNode(utils::weak_ptr<IVisualNode> item);
	virtual const QRectF& getRect();
	virtual QWidget* getWidget();
	virtual void update();
	virtual QPointF getPinPosition( int no, bool input );
	virtual void setState( State state );

private:
	utils::weak_ptr<IVisualNode> node;
	QRectF rect;
	int height;
	std::string name;
};
DEFINE_SMART_POINTERS(SimpleNodeStrategy);
}

#endif
