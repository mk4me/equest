/********************************************************************
	created:	2013/12/12
	created:	12:12:2013   13:05
	filename: 	MoveState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__MOVESTATE_H__
#define HEADER_GUARD_DICOM__MOVESTATE_H__


#include <coreui/AbstractState.h>
#include <plugins/dicom/ILayeredImage.h>
#include "NormalState.h"

namespace dicom {

class LayeredStateMachine;

class MoveState : public NormalState
{
public:
    MoveState(LayeredStateMachine* machine);
	virtual ~MoveState() {}

public:
    virtual void selectionChanged(const QList<QGraphicsItem*>& list);
    virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e);
    virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    virtual bool mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    virtual void begin(coreUI::AbstractStateConstPtr lastState);
    virtual void end();
    
private: 
    std::pair<QGraphicsItem*, QPointF> position;
    QCursor moveCursor;
};
DEFINE_SMART_POINTERS(MoveState);

}
#endif
