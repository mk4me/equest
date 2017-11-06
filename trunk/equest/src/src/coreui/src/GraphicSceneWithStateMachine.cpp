#include "CoreUiPCH.h"
#include <coreui/GraphicSceneWithStateMachine.h>

using namespace coreUI;

class GraphicSceneWithStateMachine::Impl 
{
public:
    AbstractStateMachinePtr machine;
};

coreUI::GraphicSceneWithStateMachine::GraphicSceneWithStateMachine( AbstractStateMachinePtr machine ) :
    impl(new Impl)
{
    impl->machine = machine;
}

coreUI::GraphicSceneWithStateMachine::~GraphicSceneWithStateMachine() 
{
}

void coreUI::GraphicSceneWithStateMachine::keyPressEvent(QKeyEvent *event)
{
    if (!impl->machine->keyPressEvent(event)) {
        QGraphicsScene::keyPressEvent(event);
    }
}

void coreUI::GraphicSceneWithStateMachine::keyReleaseEvent(QKeyEvent *event)
{
    if (!impl->machine->keyReleaseEvent(event)) {
        QGraphicsScene::keyReleaseEvent(event);
    }
}


void coreUI::GraphicSceneWithStateMachine::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    if (!impl->machine->mousePressEvent(event)) {
        QGraphicsScene::mousePressEvent(event);
    }
}

void coreUI::GraphicSceneWithStateMachine::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    if (!impl->machine->mouseMoveEvent(event)) {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void coreUI::GraphicSceneWithStateMachine::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    if (!impl->machine->mouseReleaseEvent(event)) {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}

void coreUI::GraphicSceneWithStateMachine::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    if (!impl->machine->mouseDoubleClickEvent(event)) {
        QGraphicsScene::mouseDoubleClickEvent(event);
    }
}

#ifndef QT_NO_WHEELEVENT
void coreUI::GraphicSceneWithStateMachine::wheelEvent( QGraphicsSceneWheelEvent *event )
{
    if (!impl->machine->wheelEvent(event)) {
        QGraphicsScene::wheelEvent(event);
    }
}

void coreUI::GraphicSceneWithStateMachine::focusInEvent( QFocusEvent * event )
{
    if (!impl->machine->focusInEvent(event)) {
        QGraphicsScene::focusInEvent(event);
    }
}

void coreUI::GraphicSceneWithStateMachine::focusOutEvent( QFocusEvent * event )
{
    if (!impl->machine->focusOutEvent(event)) {
        QGraphicsScene::focusOutEvent(event);
    }
}

#endif
