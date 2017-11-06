#include "SimpleNodeStrategy.h"
#include <plugins/newVdf/IVisualNode.h>
#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/IVisualSinkNode.h>

using namespace vdf;

//// TODO: tymczasowe
//QSizeF calculateNodeSize(int pins);
//QSizeF calculateNodeSize(int inputPins, int outputPins);
//const int pinHeight = 10;
//const int pin2 = pinHeight / 2;
//const int nodeWidth = 100;
//const int margin = 5;
//const int margin2 = margin * 2;

SimpleNodeStrategy::SimpleNodeStrategy() :
	height(-1)
{
}

void SimpleNodeStrategy::setNode( IVisualNodeWeakPtr item )
{
	node = item;
	name = node.lock()->getName().toStdString();
	auto src = utils::dynamic_pointer_cast<IVisualSourceNode>(node.lock());
	auto snk = utils::dynamic_pointer_cast<IVisualSinkNode>(node.lock());
	height = src ? src->getNumOutputPins() : 0;
	height = (std::max)(height, snk ? snk->getNumInputPins() : 0);
}


const QRectF& SimpleNodeStrategy::getRect()
{
	rect.setRect(0, 0, 100, (std::max)(10 + height * 20, 40));
	return rect;
}

QPointF SimpleNodeStrategy::getPinPosition( int no, bool input )
{
	return QPointF(input ? -7 : 91, 5 + no * 20);
}

QWidget* SimpleNodeStrategy::getWidget()
{
	return nullptr;
}

void SimpleNodeStrategy::update()
{
    auto ptr = node.lock();
    if (ptr) {
	    name = ptr->getName().toStdString();
    }
}

void SimpleNodeStrategy::setState( State state )
{
	setContainerStyle(state);
}

void SimpleNodeStrategy::setContainerStyle(INodeStrategy::State state)
{
	QString s;
	switch(state) {
	case INodeStrategy::Normal:
		break;
	case INodeStrategy::Selected:
		break;
	default:
		UTILS_ASSERT(false);
	}
}

void vdf::SimpleNodeStrategy::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr */ )
{
	
}
