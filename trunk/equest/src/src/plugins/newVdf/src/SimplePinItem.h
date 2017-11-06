/********************************************************************
	created:	2012/12/14
	created:	14:12:2012   22:11
	filename: 	SimpleItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLEITEM_H__
#define HEADER_GUARD_NEWVDF__SIMPLEITEM_H__

#include <QtGui/QGraphicsItem>
#include <QtCore/QSize>
#include <plugins/newVdf/IVisualNode.h>

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

class SimpleSourceNode : public IVisualSourceNode
{
public:
    SimpleSourceNode();
    virtual ~SimpleSourceNode() {}

public:
    virtual void addOutputPin( IVisualOutputPin * pin );
    virtual void removeOutputPin( IVisualOutputPin* pin );
    virtual void clearOutputPins();
    virtual void setName( const QString & name );
    virtual void setConfigButton( QAbstractButton * button );
    virtual void setCloseButton( QAbstractButton * button );
    virtual QGraphicsItem * visualItem();
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();

private:
    SimpleItem* simpleItem;
    std::vector<IVisualOutputPin *> pins;
};

class SimpleSinkNode : public IVisualSinkNode
{
public:
    SimpleSinkNode();
    virtual ~SimpleSinkNode() {}

public:
    virtual void addInputPin( IVisualInputPin* pin );
    virtual void removeInputPin( IVisualInputPin* pin );
    virtual void clearInputPins();
    virtual void setName( const QString & name );
    virtual void setConfigButton( QAbstractButton * button );
    virtual void setCloseButton( QAbstractButton * button );
    virtual QGraphicsItem * visualItem();
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();
private:
    SimpleItem* simpleItem;
    std::vector<IVisualInputPin *> pins;
};

class SimpleProcessingNode : public IVisualProcessingNode
{
public:
    SimpleProcessingNode();
    virtual ~SimpleProcessingNode() {}

public:
    virtual void addInputPin( IVisualInputPin* pin );
    virtual void removeInputPin( IVisualInputPin* pin );
    virtual void clearInputPins();
    virtual void addOutputPin( IVisualOutputPin * pin );
    virtual void removeOutputPin( IVisualOutputPin* pin );
    virtual void clearOutputPins();

    virtual void setName( const QString & name );
    virtual void setConfigButton( QAbstractButton * button );
    virtual void setCloseButton( QAbstractButton * button );
    virtual QGraphicsItem * visualItem();
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();

private:
    SimpleItem* simpleItem;
    std::vector<IVisualInputPin *> inputPins;
    std::vector<IVisualOutputPin *> outputPins;
};

class SimpleOutputPinNode : public IVisualOutputPin
{
public:
    SimpleOutputPinNode();
    virtual ~SimpleOutputPinNode();

public:
    virtual void markIncomplete();
    virtual void markNormal();
    virtual void markCompatible();
    virtual void markIncompatible();
    virtual void markConnective();
    virtual void markUnconnective();
    virtual void markConnected();
    virtual const QPoint connectionPosition() const;
    virtual QGraphicsItem * visualItem();
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();

private:
    SimplePinItem* item;
};

class SimpleInputPinNode : public IVisualInputPin
{
public:
    SimpleInputPinNode();
    virtual ~SimpleInputPinNode();

public:
    virtual void markRequired();
    virtual void markMissing();
    virtual void markNormal();
    virtual void markCompatible();
    virtual void markIncompatible();
    virtual void markConnective();
    virtual void markUnconnective();
    virtual void markConnected();
    virtual const QPoint connectionPosition() const;
    virtual QGraphicsItem * visualItem();
    virtual void addSelection();
    virtual void removeSelection();
    virtual void addHover();
    virtual void removeHover();
    virtual void addCollision();
    virtual void removeCollision();

private:
    SimplePinItem* item;
};


#endif
