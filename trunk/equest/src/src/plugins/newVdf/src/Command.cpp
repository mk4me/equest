#include "NewVdfPCH.h"
#include "Command.h"
#include "VdfScene.h"

using namespace vdf;

void AddToSceneCommand::doIt()
{
    sceneModel->addNodeWithPins(nodeWithPins, position);
	/*auto node = nodeWithPins.get<0>();
	auto vis = node->visualItem();
	vis->setPos(position);

	sceneModel->addItem(node);

	SceneBuilder::Pins pins = nodeWithPins.get<1>();
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		sceneModel->addItem(*it);
	}
	
	pins = nodeWithPins.get<2>();
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		sceneModel->addItem(*it);
	}*/
}

void AddToSceneCommand::undoIt()
{
	sceneModel->removeItem(nodeWithPins.get<0>());
}

AddToSceneCommand::AddToSceneCommand(SceneModelPtr scene, const SceneBuilder::VisualNodeWithPins& node, const QPointF& scenePos) :
	sceneModel(scene),
	nodeWithPins(node),
	position(scenePos)
{

}

QString AddToSceneCommand::name()
{
	return QString(typeid(this).name());
}

AddConnectionCommand::AddConnectionCommand( SceneModelPtr scene, IVisualOutputPinPtr p1, IVisualInputPinPtr p2 ) :
	sceneModel(scene),
	outputPin(p1),
	inputPin(p2)
{

}

void AddConnectionCommand::doIt()
{
    connection = sceneModel->addConnection(outputPin, inputPin);
}

void AddConnectionCommand::undoIt()
{
    sceneModel->removeConnection(connection);
}

//MultiCommand::MultiCommand( const std::vector<utils::ICommandPtr>& c ) :
//	commands(c) 
//{
//
//}
//
//void MultiCommand::doIt()
//{
//	for (auto it = commands.begin(); it != commands.end(); ++it) {
//		(*it)->doIt();
//	}
//}
//
//void MultiCommand::undoIt()
//{
//	for (int i = commands.size() - 1; i >= 0; --i) {
//		commands[i]->undoIt();
//	}
//}

//MoveCommand::MoveCommand( QGraphicsItem* item, const QPointF& newP, const QPointF& oldP ) :
//	item(item),
//	newPosition(newP),
//	oldPosition(oldP)
//{
//
//}
//
//void MoveCommand::doIt()
//{
//	item->setPos(newPosition);
//}
//
//void MoveCommand::undoIt()
//{
//	item->setPos(oldPosition);
//}

RemoveNodeCommand::RemoveNodeCommand(SceneModelPtr scene, IVisualNodePtr toRemove ) : 
	item(toRemove),
	sceneModel(scene)
{

}

void RemoveNodeCommand::doIt()
{
	removedConnections.clear();
	IVisualSourceNodePtr source = utils::dynamic_pointer_cast<IVisualSourceNode>(item);
	if (source) {
		int count = source->getNumOutputPins();
		for (int i = 0; i < count; i++) {
			IVisualOutputPinPtr pin = source->getOutputPin(i);
			removeConnectionFromPin(pin);
			removedPins.push_back(pin);
		}
	}

	IVisualSinkNodePtr sink = utils::dynamic_pointer_cast<IVisualSinkNode>(item);
	if (sink) {
		int count = sink->getNumInputPins();
		for (int i = 0; i < count; i++) {
			IVisualInputPinPtr pin = sink->getInputPin(i);
			removeConnectionFromPin(pin);
			removedPins.push_back(pin);
		}
	}

	sceneModel->removeItem(item);
}

void RemoveNodeCommand::undoIt()
{
	sceneModel->addItem(item);
	for (auto it = removedPins.begin(); it != removedPins.end(); ++it) {
		sceneModel->addItem(*it);
	}
	removedPins.clear();
	for (auto it = removedConnections.begin(); it != removedConnections.end(); ++it) {
		sceneModel->addConnection((*it)->getOutputPin(), (*it)->getInputPin());
	}
	removedConnections.clear();
}

void RemoveNodeCommand::removeConnection( IVisualConnectionPtr connection ) 
{
    if (connection) {
        sceneModel->removeConnection(connection);
        removedConnections.push_back(connection);
    }
}

void RemoveNodeCommand::removeConnectionFromPin( IVisualInputPinPtr pin )
{
	IVisualConnectionPtr connection = pin->getConnection().lock();
    removeConnection(connection);

}

void vdf::RemoveNodeCommand::removeConnectionFromPin( IVisualOutputPinPtr pin )
{
    int count = pin->getNumConnections();
    for (int i = 0; i < count; ++i) {
        IVisualConnectionPtr connection = pin->getConnection(i).lock();
        removeConnection(connection);
    }
    
}

vdf::RemoveSelectedCommand::RemoveSelectedCommand( SceneModelPtr scene, const QList<QGraphicsItem*> selectedItems ) :
	items(selectedItems),
    sceneModel(scene)
{

}

void vdf::RemoveSelectedCommand::doIt()
{
	auto nodes = sceneModel->getVisualItems<IVisualNodePtr>(items);
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto command = utils::ICommandPtr(new RemoveNodeCommand(sceneModel, *it));
        command->doIt();
		commands.push_back(command);
	}
    auto connections = sceneModel->getVisualItems<IVisualConnectionPtr>(items);
    for (auto it = connections.begin(); it != connections.end(); ++it) {
        auto command = utils::ICommandPtr(new RemoveConnectionCommand(sceneModel, *it));
        command->doIt();
        commands.push_back(command);
    }
}

void vdf::RemoveSelectedCommand::undoIt()
{
    for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
        (*it)->undoIt();
    }
    commands.clear();
}

void vdf::RemoveConnectionCommand::doIt()
{
    sceneModel->removeConnection(item);
}

void vdf::RemoveConnectionCommand::undoIt()
{
    sceneModel->addConnection(item->getOutputPin(), item->getInputPin());
}

vdf::RemoveConnectionCommand::RemoveConnectionCommand( SceneModelPtr scene, IVisualConnectionPtr toRemove ) :
    item(toRemove),
    sceneModel(scene)
{

}




