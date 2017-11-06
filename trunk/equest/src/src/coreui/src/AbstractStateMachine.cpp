#include "CoreUiPCH.h"
#include <coreui/AbstractStateMachine.h>
#include <utils/Debug.h>

using namespace coreUI;

class AbstractStateMachine::Impl
{
public:
    AbstractStatePtr currentState;
    std::vector<AbstractStatePtr> states;
};

AbstractStateMachine::AbstractStateMachine() : 
    impl(new AbstractStateMachine::Impl)
 {
 }


coreUI::AbstractStateMachine::~AbstractStateMachine()
{

}

void coreUI::AbstractStateMachine::addState( AbstractStatePtr state )
{
    if (!impl->currentState) {
        impl->currentState = state;
    }

    impl->states.push_back(state);
}

int coreUI::AbstractStateMachine::getNumStates() const
{
    return impl->states.size();
}

coreUI::AbstractStatePtr coreUI::AbstractStateMachine::getState( int idx )
{
    return impl->states[idx];
}

coreUI::AbstractStatePtr coreUI::AbstractStateMachine::getCurrentState()
{
    return impl->currentState;
}



    
void AbstractStateMachine::setState( AbstractStatePtr state )
{
    UTILS_ASSERT(impl->currentState);
    impl->currentState->end();
    impl->currentState = state;
    state->begin(impl->currentState);
}

bool coreUI::AbstractStateMachine::focusInEvent( QFocusEvent * event )
{
    return impl->currentState->focusInEvent(event);
}

bool coreUI::AbstractStateMachine::focusOutEvent( QFocusEvent * event )
{
    return impl->currentState->focusOutEvent(event);
}

bool AbstractStateMachine::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    return impl->currentState->mousePressEvent(e);
}

bool AbstractStateMachine::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    return impl->currentState->mouseReleaseEvent(e);
}

bool AbstractStateMachine::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* e )
{
    return impl->currentState->mouseDoubleClickEvent(e);
}

bool AbstractStateMachine::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    return impl->currentState->mouseMoveEvent(e);
}

bool AbstractStateMachine::keyPressEvent(QKeyEvent *event)
{
	return impl->currentState->keyPressEvent(event);
}

bool AbstractStateMachine::keyReleaseEvent(QKeyEvent *event)
{
	return impl->currentState->keyReleaseEvent(event);
}

#ifndef QT_NO_WHEELEVENT
bool AbstractStateMachine::wheelEvent( QGraphicsSceneWheelEvent* e )
{
    return impl->currentState->wheelEvent(e);
}

#endif

