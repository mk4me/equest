/********************************************************************
	created:	2013/11/13
	created:	13:11:2013   16:46
	filename: 	PointsState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__POINTSSTATE_H__
#define HEADER_GUARD_DICOM__POINTSSTATE_H__

#include <coreui/AbstractState.h>
#include "PointsLayer.h"
#include "AbstractEditState.h"

namespace dicom {

class LayeredStateMachine;

class PointsState : public AbstractEditState
{
    Q_OBJECT
public:
    PointsState(LayeredStateMachine* machine, bool curved, bool openLine, annotations::annotationsIdx adnotationIdx);
	virtual ~PointsState() {}
    friend class AddLayerCommand;
public:
    virtual bool mouseReleaseEvent( QGraphicsSceneMouseEvent* e );
    virtual bool mousePressEvent( QGraphicsSceneMouseEvent* e );

private Q_SLOTS:
    void clear();
    void resetLayer();
    void addLayer();
    void addLayer( int adnIdx );

    virtual void begin( coreUI::AbstractStateConstPtr lastState );
    virtual void end();
private:
    bool curved;
    bool openLine;
    annotations::annotationsIdx adnotationIdx;
};
DEFINE_SMART_POINTERS(PointsState);

}
#endif
