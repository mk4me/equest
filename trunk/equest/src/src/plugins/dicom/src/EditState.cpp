#include "DicomPCH.h"
#include "EditState.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QGraphicsLineItem>
#include <QtWidgets/QMenu>
#include <coreui/AbstractStateMachine.h>
#include <coreui/MoveCommand.h>
#include <coreui/MultiCommand.h>
#include "LayeredStateMachine.h"
#include "BackgroundLayer.h"
#include "LayeredSerie.h"
#include "PointsLayer.h"
#include <plugins/dicom/Annotations.h>


using namespace dicom;


dicom::EditState::EditState( LayeredStateMachine* machine ) :
    AbstractEditState(machine),
    delPointIdx(-1)
{

}



bool dicom::EditState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    if (e->button() == Qt::RightButton) {
        utils::shared_ptr<QMenu> menu = utils::make_shared<QMenu>();
        rightClickMenu = menu;

        QAction* doneAction = menu->addAction(tr("Done"));
        connect(doneAction, SIGNAL(triggered()), this, SLOT(done()));
		QList<QGraphicsItem *> itemsAtPoint = machine->getGraphicsScene()->items(e->scenePos());
		auto item =  itemsAtPoint.isEmpty() ? nullptr : itemsAtPoint.first();
        if (layer->hasPoint(item)) {
            delPointIdx = layer->getPointIdx(item);
            QAction* deleteAction = menu->addAction(tr("Delete point"));
            connect(deleteAction, SIGNAL(triggered()), this, SLOT(deletePoint()));
        }

		bool enable = machine->getSerie()->editionEnable();
		auto actions = menu->actions();
		for(auto it = actions.begin(); it != actions.end(); ++it){
			(*it)->setEnabled(enable);
		}

        menu->exec(e->screenPos());
    } else if (e->button() == Qt::LeftButton) {
		auto item = extractItem(e);

        if (layer->hasPoint(item)) {
            possibleMove = true;
            positionsToCheck.clear();
            int count = layer->getNumPoint();
            for (int i = 0; i < count; ++i) {
                auto pointItem = layer->getPoint(i);
                positionsToCheck.push_back(std::make_pair(pointItem, pointItem->scenePos()));
            }
            return false;
        } else {
            auto pos = e->scenePos();
            auto offs = layer->getItem()->pos();
            machine->getCommandStack()->addCommand(utils::make_shared<AddPointCommand>(layer, pos - offs, true));
        }
    }

    return true;
}

void dicom::EditState::begin( coreUI::AbstractStateConstPtr lastState )
{
	AbstractEditState::begin(lastState);
    layer = getLayerToEdit();
    layer->setSelected(false);
    layer->setEditable(true);
}

void dicom::EditState::end()
{
	AbstractEditState::end();
    //done();
    layer->setEditable(false);
}

bool dicom::EditState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    possibleMove = false;

    std::vector<utils::ICommandPtr> commands;
    for (auto it = positionsToCheck.begin(); it != positionsToCheck.end(); ++it) {
        QGraphicsItem* item = it->first;
        QPointF oldP = it->second;
        QPointF newP = item->pos();
        if (newP != oldP) {
            commands.push_back(utils::ICommandPtr(new MovePathCommand(layer, item, newP, oldP)));
        }
    }
    if (!commands.empty()) {
        machine->getCommandStack()->addCommand(utils::ICommandPtr(new coreUI::MultiCommand(commands)));
    }
    return false;
}


void dicom::EditState::done()
{
    machine->setState(machine->getNormalState());
}



bool dicom::EditState::keyPressEvent( QKeyEvent* event )
{
    if (event->key() == Qt::Key_Delete) {
        delPointIdx = getSelectedPointIdx();
        deletePoint();
    }

    return false;
}

int dicom::EditState::getSelectedPointIdx() const
{
    int count = layer->getNumPoint();
    int idx = -1;
    for (int i = 0; i < count; i++) {
        if (layer->getPoint(i)->isSelected()) {
            if (idx == -1) {
                idx = i;
            } else {
                return -1;
            }
        }
    }

    return idx;
}

void dicom::EditState::deletePoint()
{
    if (delPointIdx != -1) {
        machine->getCommandStack()->addCommand(utils::make_shared<RemovePointCommand>(layer, delPointIdx));
        delPointIdx = -1;
    } else {
        UTILS_ASSERT(false);
    }
}

