/********************************************************************
	created:	2013/01/23
	created:	23:1:2013   9:06
	filename: 	NormalState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__NORMALSTATE_H__
#define HEADER_GUARD_NEWVDF__NORMALSTATE_H__

#include "ISceneState.h"

class QGraphicsScene;
class IVisualItem;
class QMouseEvent;

namespace vdf {

class SimplePinItem;
class SceneStateMachine;

//!
class NormalState : public ISceneState
{
public:
    NormalState(SceneStateMachine* scene) : ISceneState(scene) {}
    virtual ~NormalState() {}
public:
    virtual void selectionChanged(const QList<QGraphicsItem*>& list);

	virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e);
	virtual bool mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

	virtual bool keyReleaseEvent( QKeyEvent *event );

    virtual void end();

private: 
	std::map<IVisualNodePtr, QPointF> positions;
};
DEFINE_SMART_POINTERS(NormalState);
}

#endif
