#include "NewVdfPCH.h"
#include "StyleItem.h"
#include <plugins/newVdf/IConnectionStrategy.h>
#include <QtWidgets/QGraphicsProxyWidget>

using namespace vdf;

void StyleItem::_StyleItem()
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
	connect(this, SIGNAL(parentChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(xChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(yChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(zChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(rotationChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(scaleChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(widthChanged()), this, SLOT(emitTransformChange()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(emitTransformChange()));

	proxy = nullptr;
}

StyleItem::StyleItem( QGraphicsObject *parent /*= 0*/ ) : 
    QGraphicsObject(parent),
    isHovered(false)
{
    _StyleItem();

};

QRectF StyleItem::boundingRect() const
{
    return strategy->getRect();
}
 
void StyleItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0 */ )
{
	UTILS_ASSERT(strategy);
	if (strategy->getWidget()) {
		//QGraphicsObject::paint(painter, option, widget);
	} else {
		strategy->paint(painter, option, widget);
	}
}

void StyleItem::hoverEnterEvent( QGraphicsSceneHoverEvent *event )
{
	emit hoveredChanged(true);
    isHovered = true;
    update();
}

void StyleItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
	emit hoveredChanged(false);
    isHovered = false;
    update();
}

void StyleItem::emitTransformChange()
{
    emit transformChanged();
    // TODO - niestety zmiana transformacji nie jest propagowana na childy
    // dlatego tez to brzydkie wyslanie sygnalu (na potrzeby pinow i aktualizacji polaczenia)
    // najlepiej byloby to jakos inaczej rozwiazac
    QList<QGraphicsItem *> children = childItems();
    for (auto it = children.begin(); it != children.end(); ++it) {
        StyleItem* si = dynamic_cast<StyleItem*>(*it);
        if (si) {
            si->emitTransformChange();
        }
    }
}

void StyleItem::setStrategy( IVisualStrategyPtr strategy )
{
	QWidget* widget = strategy->getWidget();
	if (widget) {
		auto proxy = getOrCreateProxy();
		proxy->setWidget(widget);
		proxy->update();
	}
	this->strategy = strategy;
}

IVisualStrategyPtr StyleItem::getStrategy()
{
	return strategy;
}

QGraphicsProxyWidget* StyleItem::getOrCreateProxy()
{
	if (!proxy) {
		proxy =  new QGraphicsProxyWidget(this);
	}
	return proxy;
}


QPainterPath vdf::StyleItem::shape() const
{
    IConnectionStrategyPtr connection = utils::dynamic_pointer_cast<IConnectionStrategy>(this->strategy);
    if (connection) {
        return connection->shape();
    }
    return QGraphicsItem::shape();
}
