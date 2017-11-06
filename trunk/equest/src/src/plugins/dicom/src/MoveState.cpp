#include "DicomPCH.h"
#include "MoveState.h"
#include "PointsState.h"
#include <coreui/AbstractStateMachine.h>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include "LayeredStateMachine.h"
#include <coreui/MoveCommand.h>
#include "LayeredSerie.h"

using namespace dicom;

class DicomMoveCommand : public coreUI::MoveCommand
{
public:
	DicomMoveCommand(LayeredStateMachine* machine,
		QGraphicsItem* item, const QPointF& newP, const QPointF& oldP)
		: coreUI::MoveCommand(item, newP, oldP), wasEdited(false),
		machine(machine)
	{

	}

public:
	//! Przesuwa element
	virtual void doIt()
	{
		wasEdited = machine->getSerie()->isEdited();

		coreUI::MoveCommand::doIt();
		
		if(newPosition() != oldPosition()){
			machine->getSerie()->markAsEdited(true);
		}
	}

	//! Cofa przesuniêcie
	virtual void undoIt()
	{
		coreUI::MoveCommand::undoIt();
		machine->getSerie()->markAsEdited(wasEdited);
	}

private:
	bool wasEdited;
	LayeredStateMachine* machine;
};

dicom::MoveState::MoveState( LayeredStateMachine* machine) :
    NormalState(machine),
    moveCursor(QPixmap(":/dicom/move.png"), 0, 0)
{

}

void MoveState::selectionChanged(const QList<QGraphicsItem*>& list)
{
    /*if (item2layer.empty()) {
    begin();
    }
    for (auto it = list.begin(); it != list.end(); ++it) {
    QGraphicsItem* item = extractItem(*it);
    auto layerIt = item2layer.find(item);
    if (layerIt != item2layer.end()) {
    (layerIt->second)->setSelected(true);
    }
    machine->getSerie()->getLayersModel()->refresh();
    }*/
    machine->setState(machine->getNormalState());
}

bool MoveState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    position.first = nullptr;
    if (e->button() == Qt::LeftButton) {
        auto item = extractItem(e);
        if (item) {
            position = std::make_pair(item, item->pos());
        }
    }

    // celowo zwracany jest false, nawet gdy obs³u¿yliœmy event (domyœlna obs³uga jest w tym przypadku porz¹dana)
    return false;
}

bool MoveState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    if (e->button() == Qt::LeftButton) {
        if (position.first != nullptr) {
            QGraphicsItem* item = position.first;
            QPointF oldP = position.second;
            QPointF newP = item->pos();
            machine->getCommandStack()->addCommand(utils::make_shared<DicomMoveCommand>(machine,item, newP, oldP));
        }
    }
    return false;
}



void MoveState::end()
{
    NormalState::end();
    machine->changeCursor(Qt::ArrowCursor);
    position.first = nullptr;
}

void dicom::MoveState::begin(coreUI::AbstractStateConstPtr lastState)
{
    NormalState::begin(lastState);
    machine->changeCursor(moveCursor);
}

bool dicom::MoveState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    return false;
}
