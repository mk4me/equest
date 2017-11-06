#include "DicomPCH.h"
#include "PointsState.h"
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
#include "LayeredImageVisualizer.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "AbstractEditState.h"


using namespace dicom;


class ClearCommand : public utils::ICommand
{
public:
    ClearCommand(PointsLayerPtr layer) : 
      layer(layer)
    {
    }

    virtual void doIt()
    {
        layer->getItem()->setVisible(false);
    }

    virtual void undoIt()
    {
        layer->getItem()->setVisible(true);
    }

    virtual QString name()
    {
        return QString(typeid(this).name());
    }

private:
    PointsLayerPtr layer;
};

namespace dicom {
class AddLayerCommand : public utils::ICommand
{
public:
    AddLayerCommand(LayeredStateMachine* machine, ILayeredImagePtr image, PointsState* pointsState, PointsLayerPtr layer) : 
      layer(layer),
      image(image),
      machine(machine),
      pointsState(pointsState),
	  wasEdited(false)
    {

    }

public:
    virtual void doIt()
    {
		wasEdited = machine->getSerie()->isEdited();

        layer->setEditable(false);
        image->addLayer(layer, machine->getSerie()->visualizer->getCurrentLayerUserName());
		machine->getSerie()->markAsEdited(true);
        machine->getSerie()->refresh();
        //machine->getSerie()->save();
    }
    
    virtual void undoIt() {
        image->removeLayer(layer);

		machine->getSerie()->markAsEdited(wasEdited);
        machine->getSerie()->refresh();
        //machine->getSerie()->save();
        layer->setEditable(true);
        pointsState->layer = layer;
    }
    
    virtual QString name() { return QString(typeid(this).name()); }

private:
    PointsLayerPtr layer;
    ILayeredImagePtr image;
    LayeredStateMachine* machine;
    PointsState* pointsState;
	bool wasEdited;
};

}


dicom::PointsState::PointsState( LayeredStateMachine* machine, bool curved, bool openLine, annotations::annotationsIdx adnotationIdx ) :
    AbstractEditState(machine),
    curved(curved),
    openLine(openLine),
    adnotationIdx(adnotationIdx)
{

}


bool dicom::PointsState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    if (e->button() == Qt::RightButton && layer->getNumPoint() > 0) {

        //machine->setState(machine->getNormalState());
        //return true;

        utils::shared_ptr<QMenu> menu = utils::make_shared<QMenu>();
        rightClickMenu = menu;
        QAction* clearAction = menu->addAction(tr("Remove"));
        connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

        QAction* addAction = menu->addAction(tr("Save"));
        connect(addAction, SIGNAL(triggered()), this, SLOT(addLayer()));

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
            machine->getCommandStack()->addCommand(utils::make_shared<AddPointCommand>(layer, pos, false)); // dodaj bez best fit (czyli na koniec)
        }
    }

    return true;
}


void dicom::PointsState::clear()
{
    machine->getCommandStack()->addCommand(utils::make_shared<ClearCommand>(layer));
    machine->setState(machine->getNormalState());
}

void dicom::PointsState::addLayer()
{
    addLayer(adnotationIdx);
    resetLayer();
    machine->setState(machine->getNormalState());
}

void dicom::PointsState::addLayer( int adnIdx )
{
    // warunek isVisible wynika z tego, ze clear nie usuwa punktow tylko robi warstwe niewidoczna
	auto item = layer->getItem();
    if (layer->getNumPoint() > 0 && item != nullptr && item->isVisible()) {
        auto img = machine->getSerie()->getImage();

        auto command = utils::make_shared<AddLayerCommand>(machine, img, this, layer);
        machine->getCommandStack()->addCommand(command);
    }
}

void dicom::PointsState::begin( coreUI::AbstractStateConstPtr lastState )
{
	AbstractEditState::begin(lastState);
    if (this->curved) {
        QCursor c(QPixmap(":/dicom/add_curve.png"), 0, 0);
        machine->changeCursor(c);
    } else {
        QCursor c(QPixmap(":/dicom/add_poly.png"), 0, 0);
        machine->changeCursor(c);
    }
    resetLayer();
    ILayeredImagePtr img = machine->getSerie()->getImage();
    for(auto tag : img->getTags()) {
        for (int i = img->getNumGraphicLayerItems(tag) - 1; i >= 0; --i) {
            img->getLayerGraphicItem(tag, i)->setSelected(false);
        }
    }
    machine->getGraphicsScene()->addItem(layer->getItem());
}

void dicom::PointsState::end()
{
	AbstractEditState::end();
    if (layer->getNumPoint() > 2) {
        addLayer(adnotationIdx);
    } else {
        for (int i = layer->getNumPoint() - 1; i >= 0; --i) {
            machine->getCommandStack()->addCommand(utils::make_shared<RemovePointCommand>(layer, i));
        }
    }
    resetLayer();
    machine->changeCursor(Qt::ArrowCursor);
}

bool dicom::PointsState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
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


void dicom::PointsState::resetLayer()
{
    layer = boost::make_shared<PointsLayer>(adnotationIdx);
    layer->setPointsDrawer(DrawersBuilder::createDrawer(adnotationIdx));
}
