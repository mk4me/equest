/********************************************************************
	created:	2013/01/23
	created:	23:1:2013   11:17
	filename: 	ConnectState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__CONNECTSTATE_H__
#define HEADER_GUARD_NEWVDF__CONNECTSTATE_H__

#include "ISceneState.h"

namespace vdf {

//!
class ConnectState : public ISceneState
{
public:
    ConnectState(SceneStateMachine* scene);
    virtual ~ConnectState() {}

public:
    virtual void begin( ISceneStateConstPtr lastState );
    virtual void end();
    virtual void selectionChanged(const QList<QGraphicsItem*>& list);
    virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e);

public:
    void setFirstPin(vdf::IVisualPinPtr val) { firstPin = val; }

private:
    IVisualPinPtr firstPin;
    IVisualConnectionPtr connection;
    IVisualInputPinPtr pin1;
    IVisualOutputPinPtr pin2;

	SceneModel::Connections connections;
};
DEFINE_SMART_POINTERS(ConnectState);
}

#endif
