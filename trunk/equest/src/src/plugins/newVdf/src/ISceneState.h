/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   11:57
	filename: 	ISceneState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__ISCENESTATE_H__
#define HEADER_GUARD_NEWVDF__ISCENESTATE_H__

#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsView>
#include <QtGui/QWheelEvent>

#include <utils/SmartPtr.h>
#include "SceneModel.h"

class QGraphicsScene;
class IVisualItem;
class QMouseEvent;

namespace vdf {

class SceneStateMachine;

class ISceneState
{
public:
    typedef utils::shared_ptr<ISceneState> Ptr;
    typedef utils::shared_ptr<const ISceneState> ConstPtr;
public:
    ISceneState(SceneStateMachine* machine) :
      stateMachine(machine) {}
    virtual ~ISceneState() {}

public:
    virtual void selectionChanged(const QList<QGraphicsItem*>& list) {}
    virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e) { return false; }
    virtual bool mouseReleaseEvent(QGraphicsSceneMouseEvent* e) { return false; }
    virtual bool mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e) { return false; }
    virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e) { return false; }
	virtual bool keyPressEvent(QKeyEvent *event) { return false; }
	virtual bool keyReleaseEvent(QKeyEvent *event) { return false; }
#ifndef QT_NO_WHEELEVENT
    virtual bool wheelEvent(QGraphicsSceneWheelEvent* e) { return false; }
#endif
    virtual void begin(ConstPtr lastState) {}
    virtual void end() {}

protected:
    SceneStateMachine* stateMachine;
};
DEFINE_SMART_POINTERS(ISceneState);
}

#endif
