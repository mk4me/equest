/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:32
	filename: 	StyleItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLEITEM_H__
#define HEADER_GUARD_NEWVDF__STYLEITEM_H__

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualNode.h>
#include <plugins/newVdf/IVisualStrategy.h>

namespace vdf {
//! 
class StyleItem : public QGraphicsObject
{
    Q_OBJECT
public:
    //! 
    //! \param desc 
    //! \param parent 
    StyleItem(QGraphicsObject *parent = 0);
	//! 
	virtual ~StyleItem() {}

private:
    void _StyleItem();

public:
	/*QSizeF getSize() const;
	void setSize(const QSizeF& val);
	void setSize(qreal width, qreal height);*/
	void setStrategy(IVisualStrategyPtr strategy);
	IVisualStrategyPtr getStrategy();
    
public:
    //! 
    virtual QRectF boundingRect() const;
    //! 
    //! \param painter 
    //! \param option 
    //! \param widget 
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );
    virtual QPainterPath shape() const;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    
signals:
    void transformChanged();
	void hoveredChanged(bool hover);

private slots:
    void emitTransformChange();

private:
	QGraphicsProxyWidget* getOrCreateProxy();
private:
    bool isHovered;
	IVisualStrategyPtr strategy;
	QGraphicsProxyWidget* proxy;
};

}

#endif

