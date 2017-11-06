#include "NewVdfPCH.h"
#include "SimpleItem.h"
#include <QtGui/QPainter>

namespace vdf
{

QSizeF calculateNodeSize(int pins)
{
    return QSizeF(nodeWidth + margin2, pins * pinHeight + margin2);
}

QSizeF calculateNodeSize(int inputPins, int outputPins)
{
    return calculateNodeSize((std::max)(inputPins, outputPins));
}

void SimpleItem::_SimpleItem()
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
	color = QColor(180, 180, 180, 255);

    connect(this, SIGNAL(parentChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(xChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(yChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(zChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(rotationChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(scaleChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(widthChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(emitTransformChange()));
}

SimpleItem::SimpleItem( QGraphicsObject *parent /*= 0*/ ) : 
    QGraphicsObject(parent),
    isHovered(false),
    rect(0, 0, 5, 5)
{
    _SimpleItem();
};

SimpleItem::SimpleItem( qreal width, qreal height, QGraphicsObject *parent /*= 0*/ ): 
    QGraphicsObject(parent),
    isHovered(false),
    rect(0, 0, width, height)
{
    _SimpleItem();
}

QRectF SimpleItem::boundingRect() const
{
    return rect;
}
 
void SimpleItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0 */ )
{
	// TODO : przeniesc do simple pin item
    QColor temp(
		isHovered ? color.red() + 20 : color.red(),
		isHovered ? color.green() + 20 : color.green(),
		isHovered ? color.blue() + 20 : color.blue());
	temp.setBlue(isSelected() ? 0 : temp.blue());

    painter->setBrush(QBrush(temp));
    painter->drawRect(boundingRect());
}

void SimpleItem::hoverEnterEvent( QGraphicsSceneHoverEvent *event )
{
    isHovered = true;
    update();
}

void SimpleItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    isHovered = false;
    update();
}

void SimpleItem::setSize( const QSizeF& val )
{
    rect.setWidth(val.width()); 
    rect.setHeight(val.height());
}

void SimpleItem::setSize( qreal width, qreal height )
{
    rect.setWidth(width); 
    rect.setHeight(height);
}

QSizeF SimpleItem::getSize() const
{
    return QSizeF(rect.width(), rect.height());
}

void SimpleItem::emitTransformChange()
{
    emit transformChanged();
    // TODO - niestety zmiana transformacji nie jest propagowana na childy
    // dlatego tez to brzydkie wyslanie sygnalu (na potrzeby pinow i aktualizacji polaczenia)
    // najlepiej byloby to jakos inaczej rozwiazac
    QList<QGraphicsItem *> children = childItems();
    for (auto it = children.begin(); it != children.end(); ++it) {
        SimpleItem* si = dynamic_cast<SimpleItem*>(*it);
        if (si) {
            si->emitTransformChange();
        }
    }
}


SimplePinItem::SimplePinItem(QGraphicsObject *parent) :
    SimpleItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

SimpleTextItem::SimpleTextItem( QGraphicsObject *parent /*= 0*/ ) : 
    SimpleItem(parent),
    text("Node")
{

}

SimpleTextItem::SimpleTextItem( qreal width, qreal height, QGraphicsObject *parent /*= 0*/ ) : 
    SimpleItem(width, height, parent),
    text("Node")
{

}

QRectF SimpleTextItem::boundingRect() const
{
    QRectF rect = SimpleItem::boundingRect();
    rect.setHeight(rect.height() + 20);
    return rect;
}

void SimpleTextItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0 */ )
{
    SimpleItem::paint(painter, option, widget);
    QRectF rect = SimpleItem::boundingRect();
    painter->setPen(Qt::black);
    painter->drawText(rect.x() + 15, rect.y() + rect.height() + 15, text);
}

}
