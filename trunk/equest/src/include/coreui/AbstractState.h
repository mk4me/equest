/********************************************************************
	created:	2013/11/13
	created:	13:11:2013   9:18
	filename: 	AbstractState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__ABSTRACTSTATE_H__
#define HEADER_GUARD_COREUI__ABSTRACTSTATE_H__


#include <QtWidgets/QWidget>
#include <QtGui/QWheelEvent>
#include <utils/SmartPtr.h>
#include <coreui/Export.h>
#include <QtWidgets/QGraphicsScene>

namespace coreUI {

class AbstractStateMachine;
class AbstractState;
DEFINE_SMART_POINTERS(AbstractState);

class COREUI_EXPORT AbstractState
{
public:
    AbstractState(AbstractStateMachine* machine) :
      stateMachine(machine) {}
    virtual ~AbstractState() {}

public:
    virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e) { return false; }
    virtual bool mouseReleaseEvent(QGraphicsSceneMouseEvent* e) { return false; }
    virtual bool focusInEvent(QFocusEvent * event) { return false; }
    virtual bool focusOutEvent(QFocusEvent * event) { return false; }
    virtual bool mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e) { return false; }
    virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e) { return false; }
	virtual bool keyPressEvent(QKeyEvent *event) { return false; }
	virtual bool keyReleaseEvent(QKeyEvent *event) { return false; }
#ifndef QT_NO_WHEELEVENT
    virtual bool wheelEvent(QGraphicsSceneWheelEvent* e) { return false; }
#endif
    virtual void begin(AbstractStateConstPtr lastState) {}
    virtual void end() {}

    // TODO : oprogramowac w AbstractStateMachine
    virtual void selectionChanged(const QList<QGraphicsItem*>& list) {}

protected:
    AbstractStateMachine* stateMachine;
};
}

#endif
