#include <dfmlib/Model.h>
#include <dfmlib/Pin.h>
#include <dfmlib/Connection.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Model::Model(void)
{
}

Model::~Model(void)
{
}

bool Model::isModelChangeAllowed() const
{
    return true;
}

void Model::setNodeName(const NPtr & node, const std::string & name)
{
    UTILS_ASSERT((node != nullptr), "Błędny węzeł do edycji!");

    ScopedLock lock(editMutex);

    if(node->model.lock() != shared_from_this()){
        throw std::runtime_error("Node belongs to other model!");
    }

    node->setName(name);

    notify();
}

//! \param pin Pin któremu nazwę zmieniamy
//! \param name Nowa nazwa pinu
void Model::setPinName(const PinPtr & pin, const std::string & name)
{
    UTILS_ASSERT((pin != nullptr), "Błędny pin do edycji!");

    ScopedLock lock(editMutex);

    if(pin->getParent()->model.lock().get() != this){
        throw std::runtime_error("Pin belongs to different model!");
    }

    pin->setName(name);

    notify();
}

const Model::Nodes & Model::getNodes() const
{
    ScopedLock lock(editMutex);
    return nodes;
}

const Model::Connections & Model::getConnections() const
{
    ScopedLock lock(editMutex);
    return connections;
}

const Model::RequiringConnection & Model::getRequiringConnections() const
{
    ScopedLock lock(editMutex);
    return pinsRequiringConnections;
}

const Model::Nodes & Model::getLeafNodes() const
{
    ScopedLock lock(editMutex);
    return leafNodes;
}

bool Model::isNodeSupported(const NPtr & node) const
{
    return true;
}

bool Model::additionalModelValidation() const
{
    return true;
}

bool Model::isModelValid() const
{
    ScopedLock lock(editMutex);
    return getRequiringConnections().empty() == true && additionalModelValidation() == true;
}

void Model::addNode(const NPtr & node)
{
	UTILS_ASSERT((node != nullptr), "Bledny wezel");

    if(node == nullptr){
        throw std::runtime_error("Invalid node!");
    }

    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    if(node->model.lock() != nullptr){
        throw std::runtime_error("Node belongs to other model!");
    }

    if(isNodeSupported(node) == false){
        throw std::runtime_error("Node type not supported by model!");
    }

    if(nodes.find(node) != nodes.end()){
        throw std::runtime_error("Node already added to model!");
    }

    if(node->isInitialized() == false){
        node->initialize();
    }

    node->model = shared_from_this();
    nodes.insert(node);

    afterNodeAdd(node);

    if(node->beginIn() != node->endIn()){
        //dodaj do listy wymaganych podłączenia

        PinsSet requiredPins;
        //auto pins = node->getInPins();
        for(auto it = node->beginIn(); it != node->endIn(); ++it){
            if((*it)->isRequired() == true){
                requiredPins.insert(*it);
            }
        }

        if(requiredPins.empty() == false){
            //zainicjuj brakujące piny
            pinsRequiringConnections[node].insert(requiredPins.begin(), requiredPins.end());
        }else{
            //oznacz węzeł jako niepodłaczony, bo żadne wejście nie ma połączenia
            pinsRequiringConnections[node] = PinsSet();
        }

        //oznacz jako liść
        addLeaf(node);
    }

    notify();
}

void Model::afterNodeAdd(const NPtr & node)
{

}

void Model::disconnectNode(const NPtr & node)
{
    UTILS_ASSERT((node != nullptr), "Błędny węzeł");

    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    if(node->model.lock() == nullptr){
        throw std::runtime_error("Node is not assigned to any model!");
    }

    if(node->model.lock().get() != this){
        throw std::runtime_error("Node belongs to other model!");
    }

    quickDisconnectNode(node);

    notify();
}

void Model::quickDisconnectNode(const NPtr & node)
{
    //get all node connections from all pins and remove them
    Connections allNodeConnections;

    for(auto it = node->beginIn(); it != node->endIn(); ++it){
        //const Connections & connections = (*it)->getConnections();
        allNodeConnections.insert((*it)->begin(),(*it)->end());
    }

    for(auto it = node->beginOut(); it != node->endOut(); ++it){
        //const Connections & connections = (*it)->getConnections();
        allNodeConnections.insert((*it)->begin(),(*it)->end());
    }

    //remove all node connections
    for(auto it = allNodeConnections.begin(); it != allNodeConnections.end(); ++it){
        quickRemoveConnection(connections.find(*it));
    }
}

void Model::removeNode(const NPtr & node)
{
    UTILS_ASSERT((node != nullptr), "Bledny wezel");

    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    if(node->model.lock() == nullptr){
        throw std::runtime_error("Node is not assigned to any model!");
    }

    if(node->model.lock().get() != this){
        throw std::runtime_error("Node belongs to other model!");
    }

    quickRemoveNode(nodes.find(node));

    notify();
}

void Model::quickRemoveNode(const Nodes::iterator & nodeIt)
{
    NPtr node(*nodeIt);

    removeLeaf(node);
    beforeNodeRemove(node);

    quickDisconnectNode(node);
    node->model.reset();
    nodes.erase(nodeIt);
    pinsRequiringConnections.erase(node);
}

void Model::beforeNodeRemove(const NPtr & node)
{

}

void Model::clearNodes()
{
    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    while(nodes.begin() != nodes.end()) {
        quickRemoveNode(nodes.begin());
    }

    UTILS_ASSERT((connections.size() == 0), "Nie wszystkie połączenia zostały usunięte jak się spodziewano przy usuwaniu wszystkich węzłów");

    Nodes().swap(nodes);
    Connections().swap(connections);
    RequiringConnection().swap(pinsRequiringConnections);

    notify();
}

bool Model::additionalConnectRules(const CPinPtr & src, const CPinPtr & dest) const
{
    return true;
}

bool Model::canConnect(const CPinPtr & src, const CPinPtr & dest) const
{
    ScopedLock lock(editMutex);

	bool ret = true;

	if(src->getType() != Pin::OUT || dest->getType() != Pin::IN || src->getParent() == nullptr || dest->getParent() == nullptr ||
        src->getParent()->model.lock().get() != this || dest->getParent()->model.lock().get() != this ||
        dest->isCompatible(src) == false || findConnection(src, dest) != nullptr){
			ret = false;
	}

    if(ret == true && additionalConnectRules(src, dest) == false){
        ret = false;
    }

	return ret;
}

ConnPtr Model::connect(const PinPtr & src, const PinPtr & dest)
{
    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

	if(canConnect(src, dest) != true){
		throw std::runtime_error("Connection can not be created - breaks basic connectivity rules!");
	}

    ConnPtr ret(new Connection(src, dest));
    connections.insert(ret);
    src->addConnection(ret);
    dest->addConnection(ret);

    //jeśli źródło jest zależne od jakichś pinów wejściowych w obrębie ich węzła i są one niepodłączone, to trzeba je dodać jako wymagane
    PinsSet requiredPins;
    for(auto it = src->getDependantPins().begin(); it != src->getDependantPins().end(); ++it){
        if((*it).lock()->empty() == true){
            //requiredPins.insert(*it);
            // rev - visual robil to niejawnie czy jak?
            requiredPins.insert(it->lock());
        }
    }

    if(requiredPins.empty() == false){
        pinsRequiringConnections[src->getParent()].insert(requiredPins.begin(), requiredPins.end());
    }


    //jeśli cel jest na liście wymagających podpięcia usuń go
    auto it = pinsRequiringConnections.find(dest->getParent());
    if(it != pinsRequiringConnections.end()){
        it->second.erase(dest);

        //sprawdź czy węzeł poprawnie podłączony
        if(it->second.empty() == true){
            pinsRequiringConnections.erase(it);
        }
    }

    afterConnect(ret);

    if(leafNodes.find(src->getParent()) != leafNodes.end()){
        removeLeaf(src->getParent());
    }

    notify();

    return ret;
}

void Model::afterConnect(const ConnPtr & connection)
{

}

void Model::removeConnection(const ConnPtr & connection)
{
    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    auto it = connections.find(connection);
	if(it == connections.end()){
		throw std::runtime_error("Connection dos not exist in model!");
	}

    quickRemoveConnection(it);

    notify();
}

void Model::clearConnections()
{
    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    while(connections.begin() != connections.end()){
        quickRemoveConnection(connections.begin());
    }

    Connections().swap(connections);

    notify();
}

void Model::quickRemoveConnection(const Connections::iterator & connIt)
{
    ConnPtr connection(*connIt);

    beforeRemoveConnection(connection);

    //policz ile połączeń dochodzi do węzła docelowego usuwanego połączenia (wystarczy że będzie ich więcej niż 1)
    //sprawdź czy docelowy pin nie jest wymagany dla jakiegoś połączonego piny wyjściowego węzła
    PinPtr destPin(connection->getDest());
    NPtr destNode(destPin->getParent());
    NPtr srcNode(connection->getSrc()->getParent());
    unsigned int countIn = 0;
    bool otherDepends = false;
    for(auto it = destNode->beginIn(); it != destNode->endIn(); ++it){
        if((*it)->empty() == false){
            if( otherDepends == false &&
                (*it)->getDependantPins().find(destPin) != (*it)->getDependantPins().end()){
                    otherDepends = true;
            }
            countIn += (*it)->size();
            if(countIn > 1){
                break;
            }
        }
    }

    if(countIn == 1){
        //to ostatnie połączenie, więc zostanie niepodłączony
        pinsRequiringConnections[destNode] = PinsSet();
    }

    if(otherDepends == true || destPin->isRequired() == true){
        //ten pin był albo wymagany albo zależą od niego inne piny wyjściowe węzła
        pinsRequiringConnections[destNode].insert(destPin);
    }

    connection->getSrc()->removeConnection(connection);
    connection->getDest()->removeConnection(connection);
    connection->setDest(PinPtr());
    connection->setSrc(PinPtr());
    connections.erase(connIt);

    //sprawdź czy nie powstał nowy liść
    if(srcNode->beginIn() != srcNode->endIn()){
        bool leaf = true;
        for(auto it = srcNode->beginOut(); it != srcNode->endOut(); ++it){
            if((*it)->empty() == false){
                leaf = false;
                break;
            }
        }

        if(leaf == true){
            addLeaf(srcNode);
        }
    }
}

void Model::beforeRemoveConnection(const ConnPtr & connection)
{

}

bool Model::createCycle(const CPinPtr & src, const CPinPtr & dest) const
{
    return !getCycle(src, dest).empty();
}

Model::CyclePath Model::getCycle(const CPinPtr & src, const CPinPtr & dest) const
{
    ScopedLock lock(editMutex);
	//if source is connected no loops possible
	//also if leaf node is connected

	//connections in path
	CyclePath connInPath;

	NPtr destNode = dest->getParent();
	if(Node::anyOutPinConnected(destNode) == false){
		return connInPath;
	}

	PathEntry pe = getFirstNodeOutputConnection(destNode);

	//stores node compleately checked - up to leafs
	Nodes checkedNodes;

    initCycleCheckedNodes(checkedNodes);

	//nodes in path
	Nodes nodesInPath;

	nodesInPath.insert(src->getParent());
	nodesInPath.insert(destNode);

	//structure for traversing
	std::vector<PathEntry> path;

	//find first output connection
	path.push_back(pe);
	//add it co connections
	connInPath.push_back(pe.node->getOutPin(pe.pinIndex)->getConnection(pe.connectionIndex));

	//check cycles in structure starting from this connection
	while(path.empty() == false){
		NPtr nextNode = connInPath.back()->getDest()->getParent();

		//check for a cycle
		if(nodesInPath.find(nextNode) != nodesInPath.end()){
			//we have found a cycle
			break;
		}

		if(Node::anyOutPinConnected(nextNode) == false){
			checkedNodes.insert(nextNode);
			connInPath.pop_back();
		}

		//check if node fully verified
		if(checkedNodes.find(nextNode) != checkedNodes.end()){
			//if yes we can skip it, move back and continue procedure
			while(path.empty() == false && (pe = getNextNodeOutputConnection(path.back())) == path.back()){
				//move back
				checkedNodes.insert(path.back().node);
				nodesInPath.erase(path.back().node);
				if(connInPath.empty() == false){
					connInPath.pop_back();
				}
				path.pop_back();
			}

			if(path.empty() == false){
				path.back() = pe;
				connInPath.push_back(pe.node->getOutPin(pe.pinIndex)->getConnection(pe.connectionIndex));
			}

			continue;
		}

		//go to the next node!!
		//update nodes in path
		nodesInPath.insert(nextNode);

		path.push_back(getFirstNodeOutputConnection(nextNode));
		connInPath.push_back(path.back().node->getOutPin(path.back().pinIndex)->getConnection(path.back().connectionIndex));
	}

	return connInPath;
}

Model::PathEntry Model::getFirstNodeOutputConnection(const NPtr & node)
{
    PathEntry pathElement;

    for(unsigned int i = 0; i <= node->sizeOut(); ++i){
        if(node->getOutPin(i)->empty() == false){
            pathElement.node = node;
            pathElement.pinIndex = i;
            pathElement.connectionIndex = 0;
            return pathElement;
        }
    }

    return pathElement;
}

Model::PathEntry Model::getNextNodeOutputConnection(const PathEntry & pathElement)
{
    PathEntry pe = pathElement;

    if(++pe.connectionIndex < static_cast<int>(pathElement.node->getOutPin(pathElement.pinIndex)->size())){
        return pe;
    }

    pe.pinIndex++;

    for( ; pe.pinIndex < static_cast<int>(pe.node->sizeOut()); pe.pinIndex++){
        if(pe.node->getOutPin(pe.pinIndex)->empty() == false){
            pe.connectionIndex = 0;
            return pe;
        }
    }

    return pathElement;
}

bool Model::PathEntry::operator==(const PathEntry & pe) const
{
    if(this->node == pe.node && this->pinIndex == pe.pinIndex && this->connectionIndex == pe.connectionIndex){
        return true;
    }

    return false;
}

void Model::initCycleCheckedNodes(Model::Nodes & nodes) const
{
    nodes.insert(leafNodes.begin(), leafNodes.end());
}

ConnPtr Model::findConnection(CPinPtr src, CPinPtr dest) const
{
	ConnPtr ret;

	if(src->size() > dest->size()){
		src.swap(dest);
	}

	for(auto it = src->begin(); it != src->end(); ++it){
		//connection exist and is registered by the model
		if((*it)->getOther(src) == dest && std::find(connections.begin(), connections.end(), *it) != connections.end()){
			ret = *it;
			break;
		}
	}

	return ret;
}

void Model::addLeaf(const NPtr & node)
{
    //check if node is not a leafe already
    if(leafNodes.find(node) == leafNodes.end()){
        leafNodes.insert(node);
        afterLeafAdd(node);
    }
}

void Model::removeLeaf(const NPtr & node)
{
    auto it = leafNodes.find(node);
    if(it != leafNodes.end()){
        beforeLeafRemove(node);
        //DFNode::getDFNode(node)->onLeafProcessedCallback.clear();
        leafNodes.erase(node);
    }
}

void Model::afterLeafAdd(const NPtr & node)
{

}

void Model::beforeLeafRemove(const NPtr & node)
{

}

}
