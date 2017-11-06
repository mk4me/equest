/********************************************************************
	created:	2013/11/29
	created:	29:11:2013   10:12
	filename: 	EditState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__EDITSTATE_H__
#define HEADER_GUARD_DICOM__EDITSTATE_H__

#include <coreui/AbstractState.h>
#include "PointsLayer.h"
#include <utils/ICommand.h>
#include <coreui/MoveCommand.h>
#include "AbstractEditState.h"

namespace dicom {


class EditState : public AbstractEditState
{
    Q_OBJECT
public:
    EditState(LayeredStateMachine* machine);
	virtual ~EditState() {}
    friend class AddLayerCommand;
public:
    virtual bool mouseReleaseEvent( QGraphicsSceneMouseEvent* e );
    virtual bool mousePressEvent( QGraphicsSceneMouseEvent* e );

	

    virtual bool keyPressEvent( QKeyEvent* event );

    virtual void begin( coreUI::AbstractStateConstPtr lastState );
    virtual void end();

    dicom::PointsLayerPtr getLayerToEdit() const { return layerToEdit; }
    void setLayerToEdit(dicom::PointsLayerPtr val) { layerToEdit = val; }


    int getSelectedPointIdx() const;

private Q_SLOTS:
    void done();
    void deletePoint();

private:
    PointsLayerPtr layerToEdit;
    int delPointIdx;
};
DEFINE_SMART_POINTERS(EditState);

}
#endif
