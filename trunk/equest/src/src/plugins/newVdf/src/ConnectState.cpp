#include "NewVdfPCH.h"
#include "ConnectState.h"
#include "SimpleItem.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include "SceneStateMachine.h"
#include "VdfScene.h"
#include "QtWidgets/QGraphicsSceneEvent"

using namespace vdf;

void ConnectState::begin( ISceneStateConstPtr lastState )
{
    UTILS_ASSERT (firstPin);
    
	auto sceneModel = stateMachine->getScene()->getSceneModel();
	connections = sceneModel->getPossibleConections(firstPin);

	for (auto it = connections.possible.begin(); it != connections.possible.end(); ++it) {
		(*it)->markConnective();
	}
	for (auto it = connections.impossible.begin(); it != connections.impossible.end(); ++it) {
		(*it)->markUnconnective();
	}
    auto pos = firstPin->getConnectionPosHint();
    auto factories = stateMachine->getSceneModel()->getBuilder().getFactories();
    connection = factories->getCurrentConnectionsFactory()->createConnection();
    pin1 = factories->getCurrentPinsFactory()->createInputPin();
    pin2 = factories->getCurrentPinsFactory()->createOutputPin();
    connection->setInputPin(pin1);
    connection->setOutputPin(pin2);
    stateMachine->getScene()->addItem(connection->visualItem());
    pin1->visualItem()->setPos(pos);
    pin2->visualItem()->setPos(pos);

}

ConnectState::ConnectState( SceneStateMachine* scene ) : 
    ISceneState(scene)
{

}

void ConnectState::selectionChanged( const QList<QGraphicsItem*>& list )
{
}

bool ConnectState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    auto pos1 = firstPin->getConnectionPosHint();
    auto pos2 = e->scenePos();

    if (firstPin->isType(IVisualItem::InputPin)) {
        pin2->visualItem()->setPos(e->scenePos());
    } else {
        pin1->visualItem()->setPos(e->scenePos());
    }
    
    connection->visualItem()->update();
    return true;
}

void ConnectState::end()
{
    stateMachine->getScene()->removeItem(connection->visualItem());
    connection = IVisualConnectionPtr();
    pin1 = IVisualInputPinPtr();
    pin2 = IVisualOutputPinPtr();
	for (auto it = connections.possible.begin(); it != connections.possible.end(); ++it) {
		(*it)->markNormal();
	}
	for (auto it = connections.impossible.begin(); it != connections.impossible.end(); ++it) {
		(*it)->markNormal();
	}
}

bool ConnectState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	if (e->button() == Qt::RightButton) {
		stateMachine->setState(stateMachine->getNormalState());
		return true;
	} else if (e->button() == Qt::LeftButton) {
        auto model = stateMachine->getScene()->getSceneModel();
        IVisualPinPtr pin1 = firstPin;

        auto items = stateMachine->getScene()->items(e->scenePos());
        for (auto it = items.begin(); it != items.end(); ++it) {
            IVisualPinPtr pin2 = utils::dynamic_pointer_cast<IVisualPin>(model->tryGetVisualItem(*it));
            
            if (pin1 && pin2) {
                if (model->connectionPossible(pin1, pin2)) {
                    PinResolver p(pin1, pin2);
                    stateMachine->getCommandStack()->addCommand(utils::ICommandPtr(new AddConnectionCommand(model, p, p)));
                    break;
                }
            }
        }

        stateMachine->setState(stateMachine->getNormalState());
        return true;
    }

	return false;
}
