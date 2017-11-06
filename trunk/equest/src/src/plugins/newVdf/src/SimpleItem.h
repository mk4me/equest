/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   11:22
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
class SimpleItem : public QGraphicsObject
{
    Q_OBJECT
public:
    //! 
    //! \param desc 
    //! \param parent 
    SimpleItem(QGraphicsObject *parent = 0);
    //! 
    //! \param width 
    //! \param height 
    //! \param parent 
    SimpleItem(qreal width, qreal height, QGraphicsObject *parent = 0);
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
	QColor getColor() const { return color; }
	void setColor(QColor val) { color = val; }
    
signals:
    void transformChanged();

private slots:
    void emitTransformChange();
private:
    bool isHovered;
    QRectF rect;
	QColor color;
};

class SimpleTextItem : public SimpleItem
{
    Q_OBJECT;
public:
    SimpleTextItem(QGraphicsObject *parent = 0);
    SimpleTextItem(qreal width, qreal height, QGraphicsObject *parent = 0);
    virtual ~SimpleTextItem() {}

public:
    //! 
    virtual QRectF boundingRect() const;
    //! 
    //! \param painter 
    //! \param option 
    //! \param widget 
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

public slots:
    QString getText() const { return text; }
    void setText(QString val) { text = val; }
private:
    QString text;
};

class SimplePinItem : public SimpleItem
{
public:
    SimplePinItem(QGraphicsObject *parent = 0);
    virtual ~SimplePinItem() {}
};

// TODO: tymczasowe
QSizeF calculateNodeSize(int pins);
QSizeF calculateNodeSize(int inputPins, int outputPins);
const int pinHeight = 10;
const int pin2 = pinHeight / 2;
const int nodeWidth = 100;
const int margin = 5;
const int margin2 = margin * 2;
}

#endif

