/********************************************************************
	created:	2012/12/14
	created:	14:12:2012   22:11
	filename: 	SimpleItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLEITEM_H__
#define HEADER_GUARD_NEWVDF__SIMPLEITEM_H__

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualNode.h>

namespace vdf {
//! 
class SimpleItem : public QGraphicsItem
{
public:
    //! 
    //! \param desc 
    //! \param parent 
    SimpleItem(QGraphicsItem *parent = 0);
    //! 
    //! \param width 
    //! \param height 
    //! \param parent 
    SimpleItem(qreal width, qreal height, QGraphicsItem *parent = 0);
	//! 
	virtual ~SimpleItem() {}

private:
    void _SimpleItem();

public:
    QSizeF getSize() const;
    void setSize(const QSizeF& val);
    void setSize(qreal width, qreal height);
    
public:
    //! 
    virtual QRectF boundingRect() const;
    //! 
    //! \param painter 
    //! \param option 
    //! \param widget 
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);


private:
    bool isHovered;
    QRectF rect;
};

class SimplePinItem : public SimpleItem
{
public:
    SimplePinItem(QGraphicsItem *parent = 0);
    virtual ~SimplePinItem() {}
};

}

#endif
