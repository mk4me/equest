/********************************************************************
	created:	2013/02/07
	created:	7:2:2013   13:20
	filename: 	DarkNodeStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKNODESTRATEGY_H__
#define HEADER_GUARD_NEWVDF__DARKNODESTRATEGY_H__

#include <plugins/newVdf/INodeStrategy.h>
#include <QtCore/QRect>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>

namespace vdf {

class DarkNodeStrategy : public INodeStrategy
{
public:
	DarkNodeStrategy();

	void setContainerStyle(INodeStrategy::State state);

	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr ) {}
	virtual void setNode(utils::weak_ptr<IVisualNode> item);
	virtual const QRectF& getRect();
	virtual QWidget* getWidget();
	virtual void update();
	virtual QPointF getPinPosition( int no, bool input );
	virtual void setState( State state );

private:
	utils::weak_ptr<IVisualNode> node;
	QLabel* label;
	QWidget* container;
	QLabel* lowerFrame;
	QRectF rect;
	int labelHeight;
    bool invalid;
    bool selected;
};

DEFINE_SMART_POINTERS(DarkNodeStrategy);
}

#endif
