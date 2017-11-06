/********************************************************************
	created:	2013/11/13
	created:	13:11:2013   9:18
	filename: 	AbstractStateMachine.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__ABSTRACTSTATEMACHINE_H__
#define HEADER_GUARD_COREUI__ABSTRACTSTATEMACHINE_H__

#include <QtWidgets/QWidget>
#include <QtGui/QWheelEvent>
#include <coreui/Export.h>
#include <coreui/AbstractState.h>
#include <loglib/Exceptions.h>

namespace coreUI {
    
//! Prosta maszyna stanów do obsługi UI
class COREUI_EXPORT AbstractStateMachine : public QObject
{
    Q_OBJECT;
public:
    AbstractStateMachine();
    virtual ~AbstractStateMachine();

public:
    void addState(AbstractStatePtr state);
    void setState(AbstractStatePtr state);
    int getNumStates() const;
    AbstractStatePtr getState(int idx);

    AbstractStatePtr getCurrentState();
    template <class TPtr>
    TPtr getState();

public:
    bool mousePressEvent(QGraphicsSceneMouseEvent* e);
    bool mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    bool mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e);
    bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
	bool keyPressEvent(QKeyEvent *event);
	bool keyReleaseEvent(QKeyEvent *event);
    bool focusInEvent(QFocusEvent * event);
    bool focusOutEvent(QFocusEvent * event);

#ifndef QT_NO_WHEELEVENT
    bool wheelEvent(QGraphicsSceneWheelEvent* e);
#endif


private:
    class Impl;
    utils::unique_ptr<Impl> impl;
};
DEFINE_SMART_POINTERS(AbstractStateMachine);



template <class TPtr>
TPtr coreUI::AbstractStateMachine::getState()
{
    for (int i = 0; i < getNumStates(); i++) {
        auto cast = utils::dynamic_pointer_cast<TPtr::element_type>(getState(i));
        if (cast) {
            return cast;
        }
    }

    throw loglib::runtime_error("coreUI::AbstractStateMachine::getState() - Unable to find state");
}

}

#endif
