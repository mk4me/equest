#include "DicomPCH.h"
#include "NormalState.h"
#include "PointsState.h"
#include <coreui/AbstractStateMachine.h>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include "LayeredStateMachine.h"
#include <coreui/MoveCommand.h>
#include "LayeredSerie.h"
#include <QtWidgets/QMenu>

using namespace dicom;

dicom::NormalState::NormalState( LayeredStateMachine* machine) :
    coreUI::AbstractState(machine),
    machine(machine)
{

}

void NormalState::selectionChanged(const QList<QGraphicsItem*>& list)
{
    if (item2layer.empty()) {
        createItem2LayerMap();
    }
    for (auto it = list.begin(); it != list.end(); ++it) {
        QGraphicsItem* item = extractItem(*it);
        auto layerIt = item2layer.find(item);
        if (layerIt != item2layer.end()) {
            (layerIt->second)->setSelected(true);
        }
        machine->getSerie()->getLayersModel()->refreshSelected();
    }
}

bool NormalState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
//    position.first = nullptr;
    if (e->button() == Qt::LeftButton) {
       // auto item = extractItem(e);
       // if (item) {
       //     position = std::make_pair(item, item->pos());
       // }
    } else if (e->button() == Qt::RightButton) {
        QMenu menu;
        int numSelected = getNumSelected();
        if (numSelected == 0) {
            QAction* boneAction = menu.addAction(tr("Tag bone"));
            connect(boneAction, SIGNAL(triggered()), this, SLOT(addBone()));

            QAction* skinAction = menu.addAction(tr("Tag skin"));
            connect(skinAction, SIGNAL(triggered()), this, SLOT(addSkin()));

            //QAction* tendonAction = menu.addAction(tr("Tag tendon"));
            //connect(tendonAction, SIGNAL(triggered()), this, SLOT(addTendon()));

            QAction* jointAction = menu.addAction(tr("Tag joint"));
            connect(jointAction, SIGNAL(triggered()), this, SLOT(addJoint()));

            QAction* inflamatoryAction = menu.addAction(tr("Tag region of inflammatory synovitis"));
            connect(inflamatoryAction, SIGNAL(triggered()), this, SLOT(addInflamatory()));

            //QAction* noiseAction = menu.addAction(tr("Tag noise"));
            //connect(noiseAction, SIGNAL(triggered()), this, SLOT(addNoise()));

        } else if (numSelected == 1) {
            QAction* moveAction = menu.addAction(tr("Move"));
            connect(moveAction, SIGNAL(triggered()), this, SLOT(move()));

            QAction* editAction = menu.addAction(tr("Edit"));
            connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));

            QAction* removeAction = menu.addAction(tr("Remove"));
            connect(removeAction, SIGNAL(triggered()), this, SLOT(removeLayer()));
        }

		bool enable = machine->getSerie()->editionEnable();
		auto actions = menu.actions();
		for(auto it = actions.begin(); it != actions.end(); ++it){
			(*it)->setEnabled(enable);
		}

        menu.exec(e->screenPos());
    }
    // celowo zwracany jest false, nawet gdy obs³u¿yliœmy event (domyœlna obs³uga jest w tym przypadku porz¹dana)
    return false;
}

bool NormalState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    /*if (e->button() == Qt::LeftButton) {
        if (position.first != nullptr) {
            QGraphicsItem* item = position.first;
            QPointF oldP = position.second;
            QPointF newP = item->pos();
            machine->getCommandStack()->addCommand(utils::make_shared<coreUI::MoveCommand>(item, newP, oldP));
        }
    }*/
    return false;
}

bool NormalState::keyReleaseEvent( QKeyEvent *event )
{
    return false;
}


void NormalState::end()
{
//    position.first = nullptr;
//    item2layer.clear();
}

QGraphicsItem* NormalState::extractItem( QGraphicsSceneMouseEvent* e )
{
    auto items = machine->getGraphicsScene()->items(e->scenePos());
    return extractItem(items.isEmpty() ? nullptr : items.first());
}


QGraphicsItem* dicom::NormalState::extractItem( QGraphicsItem* itm )
{
    if (itm) {
        QGraphicsItemGroup* grp = dynamic_cast<QGraphicsItemGroup*>(itm->parentItem());
        if (grp) {
            return grp;
        }
    }

    return itm;
}

void dicom::NormalState::begin(coreUI::AbstractStateConstPtr lastState)
{
    createItem2LayerMap();

}

bool dicom::NormalState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
	if (item2layer.empty()) {
		createItem2LayerMap();
	}
	for (auto& pair : item2layer) {
		(pair.second)->setSelected(false);
	}
	auto items = machine->getGraphicsScene()->items(e->scenePos());
	if (!items.isEmpty()) {
		
		auto it = item2layer.find(extractItem(items.first()));
		if (it != item2layer.end()) {
			(it->second)->setSelected(true);
		}
	}
    return true;
}

void dicom::NormalState::createItem2LayerMap()
{
    ILayeredImagePtr image = machine->getSerie()->getImage();
    int tags = image->getNumTags();
    for (int tagIdx = 0; tagIdx < tags; ++tagIdx) {
        auto tag = image->getTag(tagIdx);
        int count = image->getNumGraphicLayerItems(tag);
        for (int i = 0; i < count; ++i) {
            auto vl = image->getLayerGraphicItem(tag, i);
            if (vl) {
                item2layer[vl->getItem()] = vl;
            }
        }
    }
}

int dicom::NormalState::getNumSelected()
{
    int selected = 0;
    ILayeredImagePtr image = machine->getSerie()->getImage();
    for(auto tag : image->getTags()) {
        int count = image->getNumGraphicLayerItems(tag);
        for (int i = 0; i < count; ++i) {
            auto vl = image->getLayerGraphicItem(tag, i);
            if (vl->getSelected()) {
                ++selected;
            }
        }
    }
    return selected;
}

dicom::ILayerGraphicItemPtr dicom::NormalState::getFirstSelected()
{
    int selected = 0;
    ILayeredImagePtr image = machine->getSerie()->getImage();
    for(auto tag : image->getTags()) {
        int count = image->getNumGraphicLayerItems(tag);
        for (int i = 0; i < count; ++i) {
            auto vl = image->getLayerGraphicItem(tag, i);
            if (vl->getSelected()) {
                return vl;
            }
        }
    }
    return ILayerGraphicItemPtr();
}

void dicom::NormalState::move()
{
    machine->setState(machine->getMoveState());
}

void dicom::NormalState::edit()
{
    PointsLayerPtr points = boost::dynamic_pointer_cast<PointsLayer>(getFirstSelected());
    if (points) {
        machine->getEditState()->setLayerToEdit(points);
        machine->setState(machine->getEditState());
    }
}

void dicom::NormalState::removeLayer()
{
    PointsLayerPtr points = boost::dynamic_pointer_cast<PointsLayer>(getFirstSelected());
    if (points) {
        auto image = machine->getSerie()->getImage();
        auto tags = image->getTags();
        for (auto tag : tags) {
            int numItems = image->getNumLayerItems(tag);

            for (int i = 0; i < numItems; ++i) {
                if (image->getLayerItem(tag, i) == points) {
                    machine->getSerie()->removeLayer(tag, i);
                    return;
                }
            }

        }
    }
}

void dicom::NormalState::addBone()
{
    machine->setState(machine->getBoneState());
}

void dicom::NormalState::addSkin()
{
    machine->setState(machine->getSkinState());
}

void dicom::NormalState::addTendon()
{
    machine->setState(machine->getTendonState());
}

void dicom::NormalState::addJoint()
{
    machine->setState(machine->getJointState());
}

void dicom::NormalState::addInflamatory()
{
    machine->setState(machine->getInflamatoryState());
}

void dicom::NormalState::addNoise()
{
    machine->setState(machine->getNoiseState());
}

bool dicom::NormalState::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e)
{
	int numSelected = getNumSelected();
	if (numSelected == 1) {
		edit();
	}
	return false;
}


