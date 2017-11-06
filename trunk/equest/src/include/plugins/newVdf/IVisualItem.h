/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   10:15
    filename: IVisualItem.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALITEM_H__
#define HEADER_GUARD___IVISUALITEM_H__

#include <QtWidgets/QGraphicsItem>
#include <plugins/newVdf/IVisualStrategy.h>

namespace vdf {

class IVisualItem
{
public:
	enum Type 
	{
		Pin,
		InputPin,
		OutputPin,
		Node,
		SinkNode,
		SourceNode,
		ProcessingNode,
		Connection,
		Custom
	};

public:
	virtual ~IVisualItem() {}

public:
	virtual QGraphicsItem * visualItem() const = 0;
	virtual void setVisualStrategy(IVisualStrategyPtr strategy) = 0;
	virtual Type getType() const = 0;
	virtual bool isType(Type t) const = 0;
};
DEFINE_SMART_POINTERS(IVisualItem);

template<IVisualItem::Type T>
struct _Z { static qreal value() { return 0.0f; } };

template<> struct _Z<IVisualItem::Node>
{ static qreal value() { return 0.0f; } };

template<> struct _Z<IVisualItem::Connection>
{ static qreal value() { return 1.0f; } };

template<> struct _Z<IVisualItem::Pin>
{ static qreal value() { return 2.0f; } };


template <IVisualItem::Type T, bool focus>
struct Z
{
    static qreal value() { return (focus ? 10.0f : 0.0f) + _Z<T>::value(); };
};

}

#endif	//	HEADER_GUARD___IVISUALITEM_H__
