/********************************************************************
	created:	2013/01/28
	created:	28:1:2013   13:13
	filename: 	SimpleMergedNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SimpleMergedNode__SIMPLEMERGEDNODE_H__
#define HEADER_GUARD_SimpleMergedNode__SIMPLEMERGEDNODE_H__

#include "StyleOutputPinNode.h"

namespace vdf {
	class MergedOutputPin : public StyleOutputPinNode
	{
	public:
		MergedOutputPin(IVisualOutputPinPtr outputPin);

	public:
		virtual df::IOutputPin* getModelPin() const;

	private:
		IVisualOutputPinPtr output;
		
	};
}
//#include <QtWidgets/QGraphicsItem>
//#include <QtCore/QSize>
//#include <plugins/newVdf/IVisualNode.h>
//
//class SimpleTextItem;
//class IVisualInputPin;
//class IVisualOutputPin;
//
//namespace vdf {
//
//class SimpleMergedNode : public IVisualProcessingNode
//{
//public:
//    SimpleProcessingNode();
//    virtual ~SimpleProcessingNode() {}
//
//public:
//    virtual void addInputPin( IVisualInputPin* pin );
//    virtual void removeInputPin( IVisualInputPin* pin );
//    virtual void clearInputPins();
//    virtual void addOutputPin( IVisualOutputPin * pin );
//    virtual void removeOutputPin( IVisualOutputPin* pin );
//    virtual void clearOutputPins();
//
//    virtual void setName( const QString & name );
//    virtual void setConfigButton( QAbstractButton * button );
//    virtual void setCloseButton( QAbstractButton * button );
//    virtual QGraphicsItem * visualItem() const;
//    virtual void addSelection();
//    virtual void removeSelection();
//    virtual void addHover();
//    virtual void removeHover();
//    virtual void addCollision();
//    virtual void removeCollision();
//
//private:
//    SimpleTextItem* simpleItem;
//    std::vector<IVisualInputPin *> inputPins;
//    std::vector<IVisualOutputPin *> outputPins;
//};
//}

#endif
