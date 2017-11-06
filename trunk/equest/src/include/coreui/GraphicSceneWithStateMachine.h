/********************************************************************
	created:	2013/11/13
	created:	13:11:2013   12:10
	filename: 	GraphicSceneWithStateMachine.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__GRAPHICSCENEWITHSTATEMACHINE_H__
#define HEADER_GUARD_COREUI__GRAPHICSCENEWITHSTATEMACHINE_H__

#include <QtWidgets/QGraphicsScene>
#include <utils/SmartPtr.h>
#include <coreui/Export.h>
#include <coreui/AbstractStateMachine.h>


namespace coreUI {

class COREUI_EXPORT GraphicSceneWithStateMachine : public QGraphicsScene
{
    Q_OBJECT

public:
    GraphicSceneWithStateMachine(AbstractStateMachinePtr machine);
	virtual ~GraphicSceneWithStateMachine();

public:
    //! 
    //! \param event 
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );
    //! 
    //! \param event 
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    //! 
    //! \param event 
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );
    //! 
    //! \param event 
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    //! 
    //! \param event 
    virtual void wheelEvent( QGraphicsSceneWheelEvent *event );
    //! 
    //! \param event 
    virtual void focusInEvent(QFocusEvent * event);
    //! 
    //! \param event 
    virtual void focusOutEvent(QFocusEvent * event);

    //! 
    ////! \param event 
    //virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
    ////! 
    ////! \param event 
    //virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    ////! 
    ////! \param event 
    //virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    ////! 
    ////! \param event 
    //virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    //! 
    //! \param event 
    virtual void keyPressEvent(QKeyEvent *event);
    //! 
    //! \param event 
    virtual void keyReleaseEvent(QKeyEvent *event);

private:
    class Impl;
    utils::unique_ptr<Impl> impl;
};
DEFINE_SMART_POINTERS(GraphicSceneWithStateMachine);

}
#endif
