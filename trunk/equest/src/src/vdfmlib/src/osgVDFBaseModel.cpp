#include "StdAfx.h"

#include <osgViewer/ViewerEventHandlers>
#include <vdfmlib/osgVDFBaseModel.h>
#include <osgui/KeyboardMapperHandler.h>
#include <osgui/Borderized.h>

#include <dfmlib/Connection.h>
#include <dfmlib/DFSourceNode.h>

#include <vector>
#include <algorithm>
#include <limits>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

osgVDFBaseModel::osgVDFBaseModel(osgViewer::View* view, osgWidget::point_type width, osgWidget::point_type height,
    unsigned int nodeMask, unsigned int flags, const dflm::MPtr & model)
    : osgWidget::WindowManager(view, width, height, nodeMask, flags),
    currentNodeDescriptor(0), logicModel(model),
    modelBoundingBox(0,0,0, width, height, 0), connectingStarted(false), connectingCurrentPin(nullptr), focusedNode(nullptr),
    minIntersectRatioToSelect(0.5), maxDistToDelConnection(50), toolbarVisible(false), visualConnectionNormalColor(osgWidget::Color(0,0,0,1)),
    visualConnectionHighlightColor(osgWidget::Color(1,1,0,1)), lastHighlightedConnection(nullptr), contextMenuOn(false), moveStarted(false),
    lastButton(0)
{
    //if(model != nullptr){
        init(view);
    //}
}

osgVDFBaseModel::~osgVDFBaseModel(void)
{
    /*ScopeLock lock(stateMutex);
    if(logicModel != nullptr){
        logicModel->unlock();
    }*/
}

void osgVDFBaseModel::setModel(const dflm::MPtr & model, const NodesPositions & nodesPositions)
{
    ScopeLock lock(stateMutex);

    if(model == this->logicModel){
        return;
    }

    UTILS_ASSERT((model != nullptr), "Nieprawidłowy model!");

    //logicModel->unlock();

    //clearVisualConnections();

    //clearVisualNodes();

    logicModel = model;

    //if(logicModel->trylock() == true){

        //TODO
        //iteruj po nowym modelu i dodaj elementy

        //rozmiesc elementy
    //}
}

//! \param lock Czy zablokować do edycji logiki i właściwości modelu - tylko wizualna zmiana layoutu możliwa
void osgVDFBaseModel::lockForModelEdition(bool lock)
{

}

//! \return Czy zablokowano do edycji logiki
bool osgVDFBaseModel::isLockedForModelEdition() const
{
    return false;
}

//! \param lock Czy zablokować do jakiejkolwiek edycji
void osgVDFBaseModel::lockForVisualEdition(bool lock)
{

}

//! \return Czy zablokowano do jakiejkolwiek edycji
bool osgVDFBaseModel::isLockedForVisualEdition() const
{
    return false;
}

void osgVDFBaseModel::clearVisualConnections()
{
    throw std::runtime_error("To implement!");
}

void osgVDFBaseModel::clearVisualNodes()
{
    throw std::runtime_error("To implement!");
}

const dflm::MPtr & osgVDFBaseModel::getModel() const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
    return logicModel;
}

void osgVDFBaseModel::init( osgViewer::View* view )
{
    //add event to create nodes
//#ifdef _DEBUG
//    view->addEventHandler(new osgViewer::StatsHandler());
//#endif

    view->addEventHandler(new UserSpaceClick(this));
    view->addEventHandler(new osgui::KeyboardMapperHandler(this));
    view->addEventHandler(new VisualSelectionManager(this));
    addEventCallback(new ModelResizeHandler(this));

    nodePushEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeClick, this, osgWidget::EVENT_MOUSE_PUSH);
    nodeDragEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeDrag, this, osgWidget::EVENT_MOUSE_DRAG);
    nodeReleaseEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeRelease, this, osgWidget::EVENT_MOUSE_RELEASE);
    nodeEnterEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeEnter, this, osgWidget::EVENT_MOUSE_ENTER);
    nodeLeaveEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeLeave, this, osgWidget::EVENT_MOUSE_LEAVE);

    pinPushEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinClick, this, osgWidget::EVENT_MOUSE_PUSH);
    pinDragEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinDrag, this, osgWidget::EVENT_MOUSE_DRAG);
    pinReleaseEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinRelease, this, osgWidget::EVENT_MOUSE_RELEASE);
    pinEnterEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinEnter, this, osgWidget::EVENT_MOUSE_ENTER);
    pinLeaveEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinLeave, this, osgWidget::EVENT_MOUSE_LEAVE);

    connectionStartUpdate = boost::bind(&osgVDFBaseModel::setVisualConnectionStart, this, _1, _2);
    connectionEndUpdate = boost::bind(&osgVDFBaseModel::setVisualConnectionEnd, this, _1, _2);

    selectionActionKeys.insert(osgGA::GUIEventAdapter::KEY_Control_L);
}

bool osgVDFBaseModel::innerIsPointFree( const osgWidget::XYCoord &posToStart ) const
{
    bool ret = false;
    osgWidget::WidgetList widgetList;

    if(!const_cast<osgVDFBaseModel*>(this)->pickAtXY(posToStart.x(), posToStart.y(), widgetList)){
        ret = true;
    }

    return ret;
}

bool osgVDFBaseModel::isPointFree(const osgWidget::XYCoord & posToStart) const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    return innerIsPointFree(posToStart);

}

void osgVDFBaseModel::addNode(const dflm::NPtr & node, osg::ref_ptr<osg::Image> image,
    const std::string &  name, const osgWidget::XYCoord & pos)
{
    ScopeLock lock(stateMutex);

	logicModel->addNode(node);

	//create visual node representation
	osg::ref_ptr<osgVDFBaseNode> vnode(createVisualNode(image, name));

    vnode->modelNode = node;
    vnode->visualModel = this;

    VPinsCompatibility tmpPinsCompatibility;

    //piny wejściowe
    for(auto it = node->beginIn(); it != node->endIn(); ++it){
        osgVDFBasePin * pin = createVisualPin((*it)->getName());
        pin->modelPin = *it;
        pin->visualModel = this;
        vnode->addInPin(pin, pin->modelPin->getName());

        pinsLogicalToGraph[*it] = pin;
        configureVisualPin(pin);
        pin->connectionsUpdater = connectionEndUpdate;
        pin->setVisualStatus(pin->getStaticVisualStatus());

        osgVDFBaseNode::Pins compatible;

        for(auto iT = compatiblePins.begin(); iT != compatiblePins.end(); ++iT){
            if(iT->first->getModelPin()->getType() == dflm::Pin::OUT
                && (*it)->isCompatible(iT->first->getModelPin())){
                    compatible.insert(iT->first);
                    iT->second.insert(pin);
            }
        }

        tmpPinsCompatibility[pin] = compatible;
    }

    //piny wyjsciowe
    for(auto it = node->beginOut(); it != node->endOut(); ++it){
        osgVDFBasePin * pin = createVisualPin((*it)->getName());
        pin->modelPin = *it;
        pin->visualModel = this;
        vnode->addOutPin(pin, pin->modelPin->getName());

        pinsLogicalToGraph[*it] = pin;
        configureVisualPin(pin);
        pin->connectionsUpdater = connectionStartUpdate;
        pin->setVisualStatus(pin->getStaticVisualStatus());

        osgVDFBaseNode::Pins compatible;

        for(auto iT = compatiblePins.begin(); iT != compatiblePins.end(); ++iT){
            if(iT->first->getModelPin()->getType() == dflm::Pin::IN
                && iT->first->getModelPin()->isCompatible(*it)){
                    compatible.insert(iT->first);
                    iT->second.insert(pin);
            }
        }

        tmpPinsCompatibility[pin] = compatible;
    }

    compatiblePins.insert(tmpPinsCompatibility.begin(), tmpPinsCompatibility.end());

	//create logical node -> visual node assosiation
	nodesLogicalToGraph[node] = vnode;
	visualNodes.insert(vnode);

    deselectedVNodes.insert(vnode);

	//configure node - mouse events
	configureVisualNode(vnode);

	//add this representation to WM
	addChild(vnode);

    vnode->setArea(vnode->getHeight() * vnode->getWidth());

	//hide connections
	hideConnections();

    positionNode(vnode, pos);

    vnode->setCollisionNodes(getCollisionNodes(vnode, visualNodes));

    if(vnode->getCollisionNodes().empty() == false){
        vnode->setVisualStatus(osgVDFBaseNode::COLLISION);

        for(auto it = vnode->getCollisionNodes().begin(); it != vnode->getCollisionNodes().end(); ++it){
            if((*it)->getVisualStatus() == osgVDFBaseNode::OK){
                (*it)->setVisualStatus(osgVDFBaseNode::COLLISION);
            }
        }
    }
}

void osgVDFBaseModel::configureVisualNode(osgVDFBaseNode * node)
{
	node->addEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG | osgWidget::EVENT_MASK_FOCUS | osgWidget::EVENT_MASK_MOUSE_MOVE);

	node->addCallback(nodePushEvent);
	node->addCallback(nodeDragEvent);
	node->addCallback(nodeReleaseEvent);
	node->addCallback(nodeEnterEvent);
	node->addCallback(nodeLeaveEvent);

	node->setVisualStatus(node->getVisualStatus());
}

void osgVDFBaseModel::configureVisualPin(osgVDFBasePin * pin)
{
	pin->addEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG | osgWidget::EVENT_MASK_MOUSE_MOVE);

	pin->addCallback(pinPushEvent);
	pin->addCallback(pinDragEvent);
	pin->addCallback(pinReleaseEvent);
	pin->addCallback(pinEnterEvent);
	pin->addCallback(pinLeaveEvent);

	pin->setVisualStatus(pin->getStaticVisualStatus());
}

void osgVDFBaseModel::disconnectNode(const dflm::NPtr & node)
{
    throw std::runtime_error("To implement!");
}

void osgVDFBaseModel::removeNode(const dflm::NPtr & node)
{
	removeNode(node, false);
}

void osgVDFBaseModel::removeNode(const dflm::NPtr & node, bool clearEvent)
{
    ScopeLock lock(stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

    osgVDFBaseNode * vnode = getVisualNode(node);

    if(deleteNodeAction){
        deleteNodeAction(vnode);
    }

	dflm::Model::Nodes destNodes;

	for(auto it = node->beginOut(); it != node->endOut(); ++it){
		for(auto iT = (*it)->begin(); iT != (*it)->end(); ++iT){
			destNodes.insert((*iT)->getDest()->getParent());
		}
	}

	logicModel->removeNode(node);

	//remove visual node representation
    if(innerIsNodeSelected(vnode) == true){
        selectedVNodes.erase(vnode);
        selectedNodes.erase(vnode->getModelNode());
        selectionPrevNodesStatus.erase(vnode);
    }else{
        //mark node as deselected
        deselectedVNodes.insert(vnode);
    }

	//get pins to delete
	osgVDFBaseNode::Pins pinsToRemove(vnode->getInPins());
	pinsToRemove.insert(vnode->getOutPins().begin(),
		vnode->getOutPins().end());

	//get connections to delete
	//get pins to update
	osgVDFBasePin::Connections connectionsToRemove;
	for(auto it = pinsToRemove.begin();
		it != pinsToRemove.end(); ++it){

			connectionsToRemove.insert((*it)->getConnections().begin(),
				(*it)->getConnections().end());
	}

	//delete all connections
	//delete connections mappings
	//refresh required pins
	for(auto it = connectionsToRemove.begin();
		it != connectionsToRemove.end(); ++it){
			it->second->removeConnection(it->first);
			it->second->setVisualStatus(it->second->getStaticVisualStatus());

			//remove connection from connection map
			auto iT = connectionsGraphToLogical.find(it->first);
			connectionsLogicalToGraph.erase(iT->second);
			connectionsGraphToLogical.erase(iT);

			//remove visual connection from WM
			removeChild(it->first);
	}

	//delete pins mappings
	//delete pins compatibility
	for(auto it = pinsToRemove.begin();
		it != pinsToRemove.end(); ++it){

		pinsLogicalToGraph.erase((*it)->getModelPin());
		compatiblePins.erase(*it);

		for(auto iT = compatiblePins.begin(); iT != compatiblePins.end(); ++iT){
			iT->second.erase(*it);
		}
	}

	//delete nodes mappings
	nodesLogicalToGraph.erase(node);
	visualNodes.erase(vnode);

	//refresh output pins of affected nodes
	for(auto it = destNodes.begin(); it != destNodes.end(); ++it){
		refreshOutputPins(nodesLogicalToGraph[*it]);
	}

	if(clearEvent){
		// wyczyszczenie _lastEvent oraz flag przycisków
		pointerMove(-FLT_MAX, -FLT_MAX);
		// wyczyszczenie _lastPush
		mouseReleasedLeft(0, 0);
	}

    VNodes collisionNodes = vnode->getCollisionNodes();
    vnode->setCollisionNodes(VNodes());

    //aktualizuj statusy kolizji pozostałych węzłów
    for(auto it = collisionNodes.begin(); it != collisionNodes.end(); ++it){
        if((*it)->getVisualStatus() == osgVDFBaseNode::COLLISION && (*it)->getCollisionNodes().empty() == true){
            (*it)->setVisualStatus(osgVDFBaseNode::OK);
        }
    }

	removeChild(vnode);
	hideConnections();
}

dflm::Model::Nodes osgVDFBaseModel::getNodesInArea(const osg::BoundingBox & area) const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

	dflm::Model::Nodes ret;

    VNodes vnodes(getVisualNodesInArea(area));

	for(auto it = vnodes.begin(); it != vnodes.end(); ++it){
		ret.insert((*it)->getModelNode());
	}

	return ret;
}

osgVDFBaseModel::VNodes osgVDFBaseModel::getVisualNodesInArea(const osg::BoundingBox & area) const
{
    osgVDFBaseModel::VNodes ret;

    for(auto it = visualNodes.begin(); it != visualNodes.end(); ++it){
        osg::BoundingBox nodeBB(osgui::Utils2D::generateBox(*it));
        if(area.intersects(nodeBB) == true){
            nodeBB = area.intersect(nodeBB);
            osgWidget::point_type intersectArea = (nodeBB.xMax() - nodeBB.xMin()) * (nodeBB.yMax() - nodeBB.yMin());
            osgWidget::point_type nodeArea = (*it)->getArea();
            if(nodeArea > 0 && intersectArea / nodeArea >= minIntersectRatioToSelect){
                ret.insert(*it);
            }
        }
    }

    return ret;
}

osgWidget::XYCoord osgVDFBaseModel::getFreePlaceForNode(const dflm::NPtr & node, const osgWidget::XYCoord & posToStart) const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

    osgVDFBaseNode * vnode = getVisualNode(node);

	return osgui::Utils2D::findFreeSpaceInNearby(posToStart,vnode->getWidth(), vnode->getHeight(), this);
}

void osgVDFBaseModel::selectNode(const dflm::NPtr & node)
{
    ScopeLock lock(stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

	if(isNodeSelected(node) == true){
		return;
	}

	osgVDFBaseNode * vnode = getVisualNode(node);

	bool tVis = isTooltipVisible();
	if(tVis == true){
		hideTooltip();
	}

	selectVisualNode(vnode);

	refreshFocused();

	if(tVis == true){
		showTooltip();
	}
}

void osgVDFBaseModel::selectVisualNode(osgVDFBaseNode * vnode)
{
    //set new node status - active
    vnode->setVisualStatus(osgVDFBaseNode::ACTIVE);
    //mark node as selected
    selectedVNodes.insert(vnode);
    selectedNodes.insert(vnode->getModelNode());
    deselectedVNodes.erase(vnode);

    movePrevNodesStatus.erase(vnode);

    vnode->setStrata(osgWidget::Window::STRATA_FOREGROUND);
}

void osgVDFBaseModel::selectNodes(const dflm::Model::Nodes & nodes)
{
    if(nodes.empty() == true){
        return;
    }

	bool tVis = isTooltipVisible();

    if(tVis == true){
		hideTooltip();
	}

	for(auto it = nodes.begin(); it != nodes.end(); ++it){
        osgVDFBaseNode * vnode = getVisualNode(*it);

        selectVisualNode(vnode);
	}

    refreshFocused();

	if(tVis == true){
		showTooltip();
	}
}

void osgVDFBaseModel::selectNodes(const osg::BoundingBox & rect)
{
    ScopeLock lock(stateMutex);
	selectNodes(getNodesInArea(rect));
}

void osgVDFBaseModel::selectAllNodes()
{
    ScopeLock lock(stateMutex);
	selectNodes(logicModel->getNodes());
}

bool osgVDFBaseModel::innerIsNodeSelected(const dflm::NPtr & node) const
{
    return selectedNodes.find(node) != selectedNodes.end();
}

bool osgVDFBaseModel::innerIsNodeSelected(osgVDFBaseNode * vnode) const
{
    return selectedVNodes.find(vnode) != selectedVNodes.end();
}

bool osgVDFBaseModel::isNodeSelected(const dflm::NPtr & node) const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

	return innerIsNodeSelected(node);
}

void osgVDFBaseModel::deselectVisualNode(osgVDFBaseNode * vnode, const VNodes & nodes)
{
    osgVDFBaseNode::VisualStatus status = osgVDFBaseNode::OK;

    if(vnode->getCollisionNodes().empty() == false){
        status = osgVDFBaseNode::COLLISION;
    }

    vnode->setVisualStatus(status);

    //mark node as deselected
    deselectedVNodes.insert(vnode);
    selectedVNodes.erase(vnode);
    selectedNodes.erase(vnode->getModelNode());
    vnode->setStrata(osgWidget::Window::STRATA_NONE);
}

void osgVDFBaseModel::deselectNode(const dflm::NPtr & node)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

	if(innerIsNodeSelected(node) == false){
		return;
	}

	bool tVis = isTooltipVisible();
	if(tVis == true){
		hideTooltip();
	}

    osgVDFBaseNode * vnode = getVisualNode(node);

	deselectVisualNode(vnode, visualNodes);

	//if(focusedNode == vnode){
	//	focusedNode = nullptr;
	//	if(selectedVNodes.empty() == false){
	//		focusedNode = *(selectedVNodes.begin());
	//	}else{
	//		focusedNode = *(visualNodes.begin());
	//	}
	//}

	refreshFocused();

	if(tVis == true){
		showTooltip();
	}
}

void osgVDFBaseModel::deselectVisualNodes(const VNodes & vnodes, const VNodes & refNodes)
{
    if(vnodes.empty() == true){
        return;
    }

    bool tVis = false;

	if(isTooltipVisible() == true){
		hideTooltip();
		tVis = true;
	}

	for(auto it = vnodes.begin(); it != vnodes.end(); ++it){

        if(innerIsNodeSelected(*it) == false){
            continue;
        }

        deselectVisualNode(*it, refNodes);

        /*if(focusedNode == vnode){
            focusedNode = nullptr;
            if(selectedVNodes.empty() == false){
                focusedNode = *(selectedVNodes.begin());
            }else{
                focusedNode = *(visualNodes.begin());
            }
        }*/
	}

    refreshFocused();

	if(tVis == true){
		showTooltip();
	}
}

void osgVDFBaseModel::deselectNodes(const dflm::Model::Nodes & nodes)
{
    deselectVisualNodes(getVNodesFromNodes(nodes), visualNodes);
}

void osgVDFBaseModel::deselectNodes(const osg::BoundingBox & rect)
{
    ScopeLock lock(stateMutex);
	deselectVisualNodes(getVisualNodesInArea(rect), visualNodes);
}

void osgVDFBaseModel::deselectAllVisualNodes()
{
    if(selectedVNodes.empty() == false){
        do {
            deselectVisualNode(*(selectedVNodes.begin()), visualNodes);
        }while(selectedVNodes.empty() == false);

        refreshFocused();
    }
}

void osgVDFBaseModel::deselectAllNodes()
{
    ScopeLock lock(stateMutex);

    deselectAllVisualNodes();
}

const dflm::Model::Nodes & osgVDFBaseModel::getSelectedNodes() const
{
	ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
    return selectedNodes;
}

dflm::Model::Nodes osgVDFBaseModel::filterNodes(const dflm::Model::Nodes & nodes) const
{
    dflm::Model::Nodes ret;

    std::vector<dflm::NPtr> intersection;
    intersection.reserve(std::max(nodes.size(), logicModel->getNodes().size()));

    auto it = std::set_intersection(logicModel->getNodes().begin(), logicModel->getNodes().end(), nodes.begin(), nodes.end(), intersection.begin());

    for(auto iT = intersection.begin(); iT != it; ++iT){
        ret.insert(*iT);
    }

    return ret;
}

osgVDFBaseModel::VNodes osgVDFBaseModel::getVNodesFromNodes(const dflm::Model::Nodes & nodes) const
{
    VNodes ret;

    for(auto it = nodes.begin(); it != nodes.end(); ++it){
        ret.insert(nodesLogicalToGraph.find(*it)->second);
    }

    return ret;
}

dflm::Model::Nodes osgVDFBaseModel::getNodesFromVNodes(const VNodes & vnodes) const
{
    dflm::Model::Nodes ret;

    for(auto it = vnodes.begin(); it != vnodes.end(); ++it){
        ret.insert((*it)->getModelNode());
    }

    return ret;
}

osg::BoundingBox osgVDFBaseModel::getNodesBoundingBox( const dflm::Model::Nodes & nodes ) const
{
	osg::BoundingBox ret;

    if(nodes.empty() == false){

        //przecięcie podanych węzłów i węzłów z modelu - tylko pokrywające się węzły wezmą udział w tworzeniu obszaru

        dflm::Model::Nodes correctNodes(filterNodes(nodes));

        VNodes vnodes(getVNodesFromNodes(correctNodes));

        ret = innerGetNodesBoundingBox(vnodes);
	}

	return ret;
}

osg::BoundingBox osgVDFBaseModel::innerGetNodesBoundingBox( const VNodes & vnodes) const
{
    osg::BoundingBox ret;

    if(vnodes.empty() == false){
        auto it = vnodes.begin();
        ret  = osgui::Utils2D::generateBox(*it);
        ++it;
        for( ; it != vnodes.end(); ++it){
            ret.expandBy(osgui::Utils2D::generateBox(*it));
        }
    }

    return ret;
}

bool osgVDFBaseModel::isNodeInCollision(const dflm::NPtr & node) const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

	osgVDFBaseNode * vnode = getVisualNode(node);

    return vnode->getCollisionNodes().empty() == false;
}

bool osgVDFBaseModel::isNodeInVisibleArea(const dflm::NPtr & node) const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

	osgVDFBaseNode * vnode = getVisualNode(node);

	return osg::BoundingBox(0,0,0,this->getWidth(), this->getHeight(), 0).intersects(osgui::Utils2D::generateBox(vnode));
}

bool osgVDFBaseModel::isNodeFullyInVisibleArea(const dflm::NPtr & node) const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

    osgVDFBaseNode * vnode = getVisualNode(node);

	if(isNodeInVisibleArea(node) == true){
		osg::BoundingBox vnodeBB(osgui::Utils2D::generateBox(vnode));
		return osg::BoundingBox(0,0,0,this->getWidth(), this->getHeight(), 0).intersect(vnodeBB).radius2() == vnodeBB.radius2();
	}

	return false;
}

void osgVDFBaseModel::hideConnections()
{
	osgWidget::point_type z = getMinNodesZ();
	for(auto it = connectionsGraphToLogical.begin(); it != connectionsGraphToLogical.end(); ++it){
		setVisualConnectionZ(it->first, z);
	}
}

osgWidget::XYCoord osgVDFBaseModel::innerGetNodePosition( const dflm::NPtr & node )
{
    osgVDFBaseNode * vnode = getVisualNode(node);

    return vnode->getAbsoluteOrigin();
}

osgWidget::XYCoord osgVDFBaseModel::getNodePosition(const dflm::NPtr & node)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

    return innerGetNodePosition(node);

}

osgWidget::XYCoord osgVDFBaseModel::getNodeSize(const dflm::NPtr & node)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

	osgVDFBaseNode* vnode = getVisualNode(node);

	return osgWidget::XYCoord(vnode->getWidth(), vnode->getHeight());
}

void osgVDFBaseModel::setNodeRelativePosition(const dflm::NPtr & node, const osgWidget::XYCoord & moveStep)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

	osgVDFBaseNode* vnode = getVisualNode(node);

	if(isTooltipVisible() == true){
		hideTooltip();
	}

    moveNode(vnode, moveStep);

    VNodes collisioNodes = vnode->getCollisionNodes();

    vnode->setCollisionNodes(getCollisionNodes(vnode, visualNodes));

    if(innerIsNodeSelected(vnode) == false){
        osgVDFBaseNode::VisualStatus status = osgVDFBaseNode::OK;
        if(vnode->getCollisionNodes().empty() == true){
            status = osgVDFBaseNode::COLLISION;
        }

        vnode->setVisualStatus(status);
    }

    for(auto it = collisioNodes.begin(); it != collisionNodes.end(); ++it){
        if((*it)->getVisualStatus() == osgVDFBaseNode::COLLISION && (*it)->getCollisionNodes().empty() == true){
            (*it)->setVisualStatus(osgVDFBaseNode::OK);
        }
    }

    for(auto it = vnode->getCollisionNodes().begin(); it != vnode->getCollisionNodes().end(); ++it){
        if((*it)->getVisualStatus() == osgVDFBaseNode::OK){
            (*it)->setVisualStatus(osgVDFBaseNode::COLLISION);
        }
    }

    //updateVisualNodeStatus(vnode, visualNodes);

}

osgVDFBaseModel::VNodes osgVDFBaseModel::getCollisionNodes(osgVDFBaseNode * vnode, const VNodes & nodes) const
{
    VNodes ret;

    osg::BoundingBox bb(osgui::Utils2D::generateBox(vnode));

    for(auto it = nodes.begin(); it != nodes.end(); ++it){
        if( *it != vnode && bb.intersects(osgui::Utils2D::generateBox(*it)) == true){
            ret.insert(*it);
        }
    }

    return ret;
}

void osgVDFBaseModel::updateVisualNodeStatus(osgVDFBaseNode * vnode, const VNodes & nodes)
{

}

void osgVDFBaseModel::moveNode( osgVDFBaseNode* vnode, const osgWidget::XYCoord &moveStep )
{
    vnode->addOrigin(moveStep.x(), moveStep.y());
    vnode->resizeAdd(0,0);
}

void osgVDFBaseModel::moveNodes( const VNodes & vnodes, const osgWidget::XYCoord &moveStep )
{
    for(auto it = vnodes.begin(); it != vnodes.end(); ++it){
        moveNode(*it, moveStep);
    }
}


void osgVDFBaseModel::setNodeAbsolutePosition(const dflm::NPtr & node, const osgWidget::XYCoord & nodePos)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

    if(nodesLogicalToGraph.find(node) == nodesLogicalToGraph.end()){
        throw std::runtime_error("Node not exist in model!");
    }

    osgVDFBaseNode* vnode = getVisualNode(node);

    if(isTooltipVisible() == true){
        hideTooltip();
    }

    positionNode(vnode, nodePos);

    VNodes collisionNodes = vnode->getCollisionNodes();

    vnode->setCollisionNodes(getCollisionNodes(vnode, visualNodes));

    if(innerIsNodeSelected(vnode) == false){
        osgVDFBaseNode::VisualStatus status = osgVDFBaseNode::OK;
        if(vnode->getCollisionNodes().empty() == false){
            status = osgVDFBaseNode::COLLISION;
        }

        vnode->setVisualStatus(status);
    }

    for(auto it = collisionNodes.begin(); it != collisionNodes.end(); ++it){
        if((*it)->getVisualStatus() == osgVDFBaseNode::COLLISION && (*it)->getCollisionNodes().empty() == true){
            (*it)->setVisualStatus(osgVDFBaseNode::OK);
        }
    }

    for(auto it = vnode->getCollisionNodes().begin(); it != vnode->getCollisionNodes().end(); ++it){
        if((*it)->getVisualStatus() == osgVDFBaseNode::OK){
            (*it)->setVisualStatus(osgVDFBaseNode::COLLISION);
        }
    }

    //updateVisualNodeStatus(vnode, visualNodes);

}

void osgVDFBaseModel::positionNode( osgVDFBaseNode* vnode, const osgWidget::XYCoord &nodePos )
{
    vnode->setOrigin(nodePos.x() - vnode->getWidth() / 2.0, nodePos.y() - vnode->getHeight() / 2.0);
    vnode->resizeAdd(0,0);
}

dflm::ConnPtr osgVDFBaseModel::connect(const dflm::PinPtr & src, const dflm::PinPtr & dest)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

	dflm::ConnPtr ret(logicModel->connect(src,dest));
	if(ret == nullptr){
		return ret;
	}

	osgVDFBasePin * vsrc = pinsLogicalToGraph[src];
	osgVDFBasePin * vdest = pinsLogicalToGraph[dest];

	//create visual connection
	osg::Geode* vconnection = createVisualConnection();
	setVisualConnectionZ(vconnection, getMinNodesZ());
	setVisualConnectionColor(vconnection, visualConnectionNormalColor);

	//update visual pins
	addConnectionToPin(vsrc, vconnection, vdest);
	addConnectionToPin(vdest, vconnection, vsrc);

	//update output pins of destination node
	refreshOutputPins(vdest->getVisualNode());

	//update mappings
	connectionsLogicalToGraph[ret] = vconnection;
	connectionsGraphToLogical[vconnection] = ret;

	//update view
	addChild(vconnection);

	return ret;
}

void osgVDFBaseModel::disconnect(const dflm::ConnPtr & connection)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
    dflm::PinPtr src = connection->getSrc();
    dflm::PinPtr dest = connection->getDest();
	logicModel->removeConnection(connection);

	osg::Geode * vconnection = connectionsLogicalToGraph[connection];

	//update connection pins
	osgVDFBasePin* vpin = pinsLogicalToGraph[src];
	removeConnectionFromPin(vpin,vconnection);
	vpin = pinsLogicalToGraph[dest];
	removeConnectionFromPin(vpin,vconnection);

	//update output pins of destination node
	refreshOutputPins(vpin->getVisualNode());

	//remove visual connection
	connectionsLogicalToGraph.erase(connection);
	connectionsGraphToLogical.erase(vconnection);
	removeChild(vconnection);
    if(lastHighlightedConnection == vconnection){
        lastHighlightedConnection = nullptr;
    }
}

osgWidget::point_type osgVDFBaseModel::getMinSelectedNodeAreaRatioToSelect() const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
	return minIntersectRatioToSelect;
}

void osgVDFBaseModel::setMinSelectedNodeAreaRatioToSelect(osgWidget::point_type areaRatio)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
	minIntersectRatioToSelect = areaRatio;
}

osgWidget::point_type osgVDFBaseModel::getMinDistToDelConnection() const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
	return maxDistToDelConnection;
}

void osgVDFBaseModel::setMinDistToDelConnection(osgWidget::point_type dist)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
	maxDistToDelConnection = dist;
}

const osgui::KeyboardMapper::Keys & osgVDFBaseModel::getSelectionActionKeys() const
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
	return selectionActionKeys;
}

void osgVDFBaseModel::setSelectionActionKeys(const osgui::KeyboardMapper::Keys & keys)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
	selectionActionKeys = keys;
}

void osgVDFBaseModel::deleteNodeSelf(osgVDFBaseNode * node)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);
	removeNode(node->getModelNode(), true);
}

void osgVDFBaseModel::refreshPinVisualState(osgVDFBasePin * pin)
{
	pin->setVisualStatus(pin->getStaticVisualStatus());
}

void osgVDFBaseModel::addConnectionToPin(osgVDFBasePin * pin, osg::Geode * connection, osgVDFBasePin * other){
	pin->addConnection(connection, other);
	refreshPinVisualState(pin);
}

void osgVDFBaseModel::removeConnectionFromPin(osgVDFBasePin * pin, osg::Geode * connection){
	pin->removeConnection(connection);
	refreshPinVisualState(pin);
}

void osgVDFBaseModel::refreshOutputPins(osgVDFBaseNode * node){
	for(osgVDFBaseNode::Pins::iterator it = node->getOutPins().begin(); it != node->getOutPins().end(); ++it){
		//refresh pin visual status
		refreshPinVisualState(*it);
	}
}

void osgVDFBaseModel::showDefaultToolbar(bool show)
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

	if(toolbarVisible == show){
		return;
	}

	toolbarVisible = show;

	if(toolbarVisible == true){
		if(toolbar == 0){
			toolbar = new osgui::Toolbar();
			this->addChild(toolbar);
			fillToolbarWithGroups(toolbar);
			for(std::set<osgVDFNodeTypeDescriptor*>::iterator it = nodeTypesMissing.begin();
				it != nodeTypesMissing.end(); ++it){

				graphAddNodeTypeToToolbar(*it);
			}

			std::set<osgVDFNodeTypeDescriptor*>().swap(nodeTypesMissing);
		}

		//m_pToolbar->setOrigin(0,this->getHeight() - m_pToolbar->getHeight());
		toolbar->setOrigin(0,0);
		toolbar->resize(this->getWidth(), toolbar->getHeight());
		toolbar->update();
		//TODO
		//if some collision with nodes - move nodes down to avoid collision
		toolbar->show();

	}else{
		toolbar->hide();
	}
}

void osgVDFBaseModel::registerNodeType( osgVDFNodeTypeDescriptor * descriptor )
{
    ScopeLock lock(const_cast<osgVDFBaseModel*>(this)->stateMutex);

	if(nodeTypesToControls.find(descriptor) != nodeTypesToControls.end()){
		throw std::runtime_error("Node type already registered!");
	}

	if(toolbar != 0){
		graphAddNodeTypeToToolbar(descriptor);
	}else{
		nodeTypesToControls[descriptor] = 0;
		nodeTypesMissing.insert(descriptor);
	}
}

void osgVDFBaseModel::graphAddNodeTypeToToolbar( osgVDFNodeTypeDescriptor* nodeTypeDescriptor )
{
	int tab = -1;
	switch(nodeTypeDescriptor->getNodeType()){
	case osgVDFBaseNode::ANALYZE:
		tab = 1;
		break;
	case osgVDFBaseNode::PROCESS:
		tab = 2;
		break;
	case osgVDFBaseNode::SOURCE:
		tab = 0;
		break;
	case osgVDFBaseNode::VISUALIZE:
		tab = 3;
		break;
	}

	if(tab == -1){
		return;
	}

	Button * newLabel = new Button(nodeTypeDescriptor->getButtonText(),"");
	newLabel->setToggleEnabled(true);
	newLabel->setToggled(false);

	newLabel->setNormalStyle("osgVDF.toolbar.button.normal");
	newLabel->setHoverStyle("osgVDF.toolbar.button.hover");
	newLabel->setToggleStyle("osgVDF.toolbar.button.toggle");
	newLabel->setPushedStyle("osgVDF.toolbar.button.pushed");


	newLabel->addCallback(new osgWidget::Callback(&osgVDFBaseModel::onNodeDescriptorPush, this, osgWidget::EVENT_MOUSE_PUSH, newLabel));
	newLabel->setEventMask(osgWidget::EVENT_MASK_MOUSE_CLICK);

	newLabel->setCanFill(false);
	controlsToNodeTypes[newLabel] = nodeTypeDescriptor;
	nodeTypesToControls[nodeTypeDescriptor] = newLabel;
	if(nodeTypeDescriptor->getButtonImage() != nullptr){
		newLabel->setColor(osgWidget::Color(1,1,1,1));
		newLabel->setSize(nodeTypeDescriptor->getButtonImage()->s(), nodeTypeDescriptor->getButtonImage()->t());
		newLabel->setImage(nodeTypeDescriptor->getButtonImage(),true, true);

		osgui::Tooltip * tooltip = new osgui::Tooltip();
        addChild(tooltip);
		tooltip->setText(nodeTypeDescriptor->getButtonText());
		tooltip->registerItem(newLabel);
		tooltips.insert(tooltip);
	}else{
		newLabel->setLabel(nodeTypeDescriptor->getButtonText());
		newLabel->setSize(0,0);
		newLabel->setFontSize(13);
	}

	toolbar->addTabElement(tab, newLabel);
}

osgVDFBaseNode* osgVDFBaseModel::getVisualNode(const dflm::NPtr & node) const{
	NodesMapping::const_iterator it = nodesLogicalToGraph.find(node);
	if(it != nodesLogicalToGraph.end()){
		return it->second;
	}

	return nullptr;
}

void osgVDFBaseModel::fillToolbarWithGroups(osgui::Toolbar * toolbar){
	if(toolbar == nullptr){
		return;
	}

	osgWidget::Label * label = new osgui::Borderized<osgWidget::Label>("sourceTab");
	toolbar->addTab(label);
    label->setLabel("Source");

	label = new osgui::Borderized<osgWidget::Label>("analysisTab");
	toolbar->addTab(label);
    label->setLabel("Analysis");

	label = new osgui::Borderized<osgWidget::Label>("processingTab");
	toolbar->addTab(label);
    label->setLabel("Processing");

	label = new osgui::Borderized<osgWidget::Label>("visualizeTab");
	toolbar->addTab(label);
    label->setLabel("Visualize");

    toolbar->resize();
}

osgWidget::point_type osgVDFBaseModel::getMinNodesZ(){
	osgWidget::point_type ret = 0;

	if(visualNodes.empty() == false){
		VNodes::iterator it = visualNodes.begin();
		ret = osgui::Utils2D::calcAbsZ(*it);
		++it;
		for( ; it != visualNodes.end(); ++it){
			ret = std::min(ret,osgui::Utils2D::calcAbsZ(*it));
		}
	}

	return ret;

}

osgWidget::point_type osgVDFBaseModel::getMaxNodesZ(){
	osgWidget::point_type ret = 0;

	if(visualNodes.empty() == false){
		VNodes::iterator it = visualNodes.begin();
		ret = osgui::Utils2D::calcAbsZ(*it);
		++it;
		for( ; it != visualNodes.end(); ++it){
			ret = std::max(ret,osgui::Utils2D::calcAbsZ(*it));
		}
	}

	return ret;

}

bool osgVDFBaseModel::onNodeDescriptorPush(osgWidget::Event& ev){
	currentNodeDescriptor = controlsToNodeTypes[ev.getWidget()];
	lastButton = (Button*)ev.getWidget();
	return false;
}

osgVDFBaseModel::VNodes osgVDFBaseModel::getSelectedNodesCollisions(){
	VNodes ret;

	std::vector<osgVDFBaseNode*> diff(nodesLogicalToGraph.size() - selectedVNodes.size());
	std::set_difference(visualNodes.begin(), visualNodes.end(), selectedVNodes.begin(), selectedVNodes.end(), diff.begin());

	for(VNodes::const_iterator it = selectedVNodes.begin(); it != selectedVNodes.end(); ++it){
		osg::BoundingBox selBB = osgui::Utils2D::generateBox(*it);
		for(std::vector<osgVDFBaseNode*>::iterator iT = diff.begin(); iT != diff.end(); ++iT){
			if(selBB.intersects(osgui::Utils2D::generateBox(*iT)) == true){
				ret.insert(*iT);
			}
		}
	}

	return ret;
}

void osgVDFBaseModel::saveCurrentVisualNodesStates()
{
    for(auto it = deselectedVNodes.begin(); it != deselectedVNodes.end(); ++it){
        movePrevNodesStatus[*it] = (*it)->getVisualStatus();
    }
}

void osgVDFBaseModel::restorePreviousVisualNodeStates()
{
    for(auto it = movePrevNodesStatus.begin(); it != movePrevNodesStatus.end(); ++it){
        it->first->setVisualStatus(it->second);
    }
}

void osgVDFBaseModel::updateCollisions(){
	//process collisions

	//get current collisions
	VNodes collisions = getSelectedNodesCollisions();

	//if collisions still exist
	if(collisions.empty() == false){

		//create difference between previous collision list and current collision list
		std::vector<osgVDFBaseNode*> difference(collisionNodes.size());
		std::vector<osgVDFBaseNode*>::iterator diffEnd = std::set_difference(collisionNodes.begin(),
			collisionNodes.end(), collisions.begin(), collisions.end(), difference.begin());

		//return nodes previous states if they are not in collision any more
		for(std::vector<osgVDFBaseNode*>::iterator it = difference.begin(); it != diffEnd; ++it){
			(*it)->setVisualStatus(movePrevNodesStatus[*it]);
			movePrevNodesStatus.erase(*it);
		}

		//fill new collision points
		for(VNodes::iterator it = collisions.begin(); it != collisions.end(); ++it){
			if(movePrevNodesStatus.find(*it) == movePrevNodesStatus.end()){
				movePrevNodesStatus[*it] = (*it)->getVisualStatus();
				(*it)->setVisualStatus(osgVDFBaseNode::COLLISION);
				collisionNodes.insert(*it);
			}
		}
	}else{
		for(VNodesPrevVisualStatus::iterator it = movePrevNodesStatus.begin();
			it != movePrevNodesStatus.end(); ++it){

				it->first->setVisualStatus(it->second);
		}
		VNodes().swap(collisionNodes);
		VNodesPrevVisualStatus().swap(movePrevNodesStatus);
	}
}

bool osgVDFBaseModel::onNodeClick(osgWidget::Event& ev)
{
    stateMutex.lock();
	try{
	    if(ev.getWindowManager()->isLeftMouseButtonDown() == false){
		    return false;
	    }

	    osgVDFBaseNode * vnode = dynamic_cast<osgVDFBaseNode*>(ev.getWindow());

	    focusedNode = vnode;

	    moveStarted = true;
	    bool selActionKeysPressed = areKeysPressed(selectionActionKeys);
	    if(innerIsNodeSelected(vnode) == true){
		    if(selActionKeysPressed == true){
			    moveStarted = false;
			    deselectVisualNode(vnode, deselectedVNodes);
		    }
	    }else{
		    if(selActionKeysPressed == true){
			    selectVisualNode(vnode);
		    }else{
			    deselectAllVisualNodes();
			    selectVisualNode(vnode);
		    }
	    }

	    if(moveStarted == true){

            if(isTooltipVisible() == true){
                hideTooltip();
            }

		    selectionBoundingBox = innerGetNodesBoundingBox(selectedVNodes);

            moveStartedWithViewCollision = !(osgui::Utils2D::boxFullyEnclosedInBox(selectionBoundingBox, modelBoundingBox));

		    //remember initial position in case of collision and revert position of selection
		    initialMoveCoords.x() = selectionBoundingBox.xMin();
		    initialMoveCoords.y() = selectionBoundingBox.yMin();
	    }
    }catch(...){
        moveStarted = false;
        stateMutex.unlock();
    }

	return false;
}

bool osgVDFBaseModel::onNodeDrag(osgWidget::Event& ev)
{
    try{
	    if(moveStarted == true){
		    osgWidget::XYCoord move(ev.x, ev.y);
		    osg::BoundingBox selBB(selectionBoundingBox);
		    selBB = osgui::Utils2D::translateBoundingBox(selBB, move);

            bool movePossible = selBB.intersect(modelBoundingBox).radius2() == selBB.radius2() && (toolbarVisible == false || (osgui::Utils2D::generateBox(toolbar).intersects(selBB) == false));

            if(moveStartedWithViewCollision == true || movePossible == true){
                moveNodes(selectedVNodes, move);
                selectionBoundingBox = selBB;

                for(auto it = selectedVNodes.begin(); it != selectedVNodes.end(); ++it){
                    (*it)->setCollisionNodes(getCollisionNodes(*it, deselectedVNodes));
                }

                for(auto it = deselectedVNodes.begin(); it != deselectedVNodes.end(); ++it){
                    if((*it)->getVisualStatus() == osgVDFBaseNode::COLLISION && (*it)->getCollisionNodes().empty() == true){
                        (*it)->setVisualStatus(osgVDFBaseNode::OK);
                    }else if((*it)->getVisualStatus() == osgVDFBaseNode::OK && (*it)->getCollisionNodes().empty() == false){
                        (*it)->setVisualStatus(osgVDFBaseNode::COLLISION);
                    }
                }
            }

            if(movePossible == true){
                moveStartedWithViewCollision = false;
            }
	    }
    }catch(...){
        moveStarted = false;
        stateMutex.unlock();
    }

	return false;
}

bool osgVDFBaseModel::onNodeRelease(osgWidget::Event& ev)
{
    try{
	    if(moveStarted == true){

            bool moveOk = true;

            for(auto it = selectedVNodes.begin(); it != selectedVNodes.end(); ++it){
                osgVDFBaseNode::VNodesDifference intersect(std::min((*it)->getCollisionNodes().size(), deselectedVNodes.size()));

                auto iT = std::set_intersection((*it)->getCollisionNodes().begin(), (*it)->getCollisionNodes().end(),
                    deselectedVNodes.begin(), deselectedVNodes.end(), intersect.begin());

                if(iT != intersect.begin()){
                    moveOk = false;
                    break;
                }
            }

		    if(moveOk == false){
			    //return to original position

                initialMoveCoords.x() -= selectionBoundingBox.xMin();
                initialMoveCoords.y() -= selectionBoundingBox.yMin();

			    moveNodes(selectedVNodes, initialMoveCoords);

                for(auto it = selectedVNodes.begin(); it != selectedVNodes.end(); ++it){
                    (*it)->setCollisionNodes(getCollisionNodes(*it, deselectedVNodes));
                }

                for(auto it = deselectedVNodes.begin(); it != deselectedVNodes.end(); ++it){
                    if((*it)->getVisualStatus() == osgVDFBaseNode::COLLISION && (*it)->getCollisionNodes().empty() == true){
                        (*it)->setVisualStatus(osgVDFBaseNode::OK);
                    }else if((*it)->getVisualStatus() == osgVDFBaseNode::OK && (*it)->getCollisionNodes().empty() == false){
                        (*it)->setVisualStatus(osgVDFBaseNode::COLLISION);
                    }
                }
		    }else{
                for(auto it = selectedVNodes.begin(); it != selectedVNodes.end(); ++it){
                    (*it)->setCollisionNodes(getCollisionNodes(*it, deselectedVNodes));
                }

                for(auto it = deselectedVNodes.begin(); it != deselectedVNodes.end(); ++it){
                    if((*it)->getVisualStatus() == osgVDFBaseNode::COLLISION && (*it)->getCollisionNodes().empty() == true){
                        (*it)->setVisualStatus(osgVDFBaseNode::OK);
                    }else if((*it)->getVisualStatus() == osgVDFBaseNode::OK && (*it)->getCollisionNodes().empty() == false){
                        (*it)->setVisualStatus(osgVDFBaseNode::COLLISION);
                    }
                }
            }
		    moveStarted = false;
            stateMutex.unlock();
	    }
    }catch(...){
        moveStarted = false;
        stateMutex.unlock();
    }

	return false;
}

bool osgVDFBaseModel::onPinClick(osgWidget::Event& ev)
{
    stateMutex.lock();
    try{
	    //event pin
	    if(ev.getWindowManager()->isLeftMouseButtonDown() == false){
		    return false;
	    }

	    connectingStartPin = dynamic_cast<osgVDFBasePin*>(ev.getWidget());

	    if(connectingStarted == false && (connectingStartPin->getModelPin()->getType() == dflm::Pin::OUT ||
		    connectingStartPin->getModelPin()->empty() == true)){
			    //mark that connecting has started
			    connectingStarted = true;

			    if(isTooltipVisible() == true){
				    hideTooltip();
			    }

			    //get mouse pose to track it later with onPinDrag or onPinMove action
			    connectingCurrentPose.x() = ev.x;
			    connectingCurrentPose.y() = ev.y;

			    //highlight its compatible pins
			    osgVDFBaseNode::Pins & cPins = compatiblePins[connectingStartPin];
			    savePinsStatus(cPins);
			    highlightCompatiblePins(connectingStartPin, cPins);

			    //create graphical connection representation

			    visualConnection = createVisualConnection();
                setVisualConnectionStart(visualConnection, connectingStartPin->getCenterPosition());
                setVisualConnectionEnd(visualConnection, connectingStartPin->getCenterPosition());
			    setVisualConnectionZ(visualConnection, 0);
			    setVisualConnectionColor(visualConnection, visualConnectionHighlightColor);

			    if(connectingStartPin->getModelPin()->getType() == dflm::Pin::IN){
				    connectionContinue = connectionStartUpdate;
			    }else if(connectingStartPin->getModelPin()->getType() == dflm::Pin::OUT){
				    connectionContinue = connectionEndUpdate;
			    }

			    conectingPrevStartPinStatus = connectingStartPin->getVisualStatus();
			    connectingStartPin->setVisualStatus(osgVDFBasePin::CONNECTED);

			    addChild(visualConnection);
			    //setFocused(m_pFocusedNode);
			    refreshFocused();
	    }
    }catch(...){
        connectingStarted = false;
        stateMutex.unlock();
    }

	return false;
}

bool osgVDFBaseModel::onPinDrag(osgWidget::Event& ev)
{
    try{
	    if(connectingStarted == true){
		    //update graphical connection shape and position
		    connectingCurrentPose.x() += ev.x;
		    connectingCurrentPose.y() += ev.y;

		    //update graphical connection
		    connectionContinue(visualConnection, connectingCurrentPose);

		    //check if we are over some pin!!
		    //if yes highlight it properly and save its pointer
		    //else reset destination pin, reset its state
		    osg::Vec3f pos3D(connectingCurrentPose.x(), connectingCurrentPose.y(),0);
		    bool isPin = false;
		    for(auto it = compatiblePins.begin();
			    it != compatiblePins.end(); ++it){

				    //onEnter
				    if(it->first != connectingStartPin && osgui::Utils2D::generateBox(it->first).contains(pos3D) == true){
					    isPin = true;
					    if(connectingCurrentPin != it->first){
						    if(connectingCurrentPin != nullptr){
							    //reset status
							    connectingCurrentPin->setVisualStatus(conectingPrevCurrentPinStatus);
						    }

						    connectingCurrentPin = it->first;
						    conectingPrevCurrentPinStatus = connectingCurrentPin->getVisualStatus();
						    osgVDFBasePin::VisualStatus pinNewStatus = osgVDFBasePin::CONNECTED;

						    if(connectingPrevPinsStatus.find(connectingCurrentPin) == connectingPrevPinsStatus.end() ||
							    connectingCurrentPin->getDynamicVisualStatus(connectingStartPin,logicModel) == osgVDFBasePin::INCOMPATIBLE){
								    pinNewStatus = osgVDFBasePin::INCOMPATIBLE;
						    }

						    connectingCurrentPin->setVisualStatus(pinNewStatus);
					    }

					    break;
				    }
		    }

		    //on leave current pin
		    if(isPin == false && connectingCurrentPin != nullptr){
			    connectingCurrentPin->setVisualStatus(conectingPrevCurrentPinStatus);
			    connectingCurrentPin = nullptr;
		    }
	    }
    }catch(...){
        connectingStarted = false;
        stateMutex.unlock();
    }

	return false;
}

bool osgVDFBaseModel::onPinRelease(osgWidget::Event& ev)
{
    try{
	    if(connectingStarted == true){
		    restorePinsStatus();

		    if(connectingCurrentPin != nullptr){

                connectionContinue(visualConnection, connectingCurrentPin->getCenterPosition());

                if(connectingStartPin->getModelPin()->getType() == dflm::Pin::IN){
                    std::swap(connectingStartPin, connectingCurrentPin);
                    std::swap(conectingPrevStartPinStatus, conectingPrevCurrentPinStatus);
                }

                if(logicModel->canConnect(connectingStartPin->getModelPin(), connectingCurrentPin->getModelPin()) == true){

                    connectingStartPin->setVisualStatus(osgVDFBasePin::CONNECTED);
                    connectingCurrentPin->setVisualStatus(osgVDFBasePin::CONNECTED);

                    dflm::ConnPtr conn = logicModel->connect(connectingStartPin->getModelPin(), connectingCurrentPin->getModelPin());

                    //aktuzlizuj piny, dodaj połączenie do map
                    addConnectionToPin(connectingStartPin, visualConnection, connectingCurrentPin);
                    addConnectionToPin(connectingCurrentPin, visualConnection, connectingStartPin);

                    //update mappings
                    connectionsLogicalToGraph[conn] = visualConnection;
                    connectionsGraphToLogical[visualConnection] = conn;

                    setVisualConnectionColor(visualConnection, visualConnectionNormalColor);

                    refreshOutputPins(connectingCurrentPin->getVisualNode());
                }else{
                    removeChild(visualConnection);
                    connectingStartPin->setVisualStatus(conectingPrevStartPinStatus);
                    connectingCurrentPin->setVisualStatus(conectingPrevCurrentPinStatus);
                }

		    }else{
                removeChild(visualConnection);
			    connectingStartPin->setVisualStatus(conectingPrevStartPinStatus);
		    }

		    //setFocused(m_pFocusedNode);
		    refreshFocused();

		    //reset connecting
		    visualConnection = nullptr;
		    connectingStartPin = connectingCurrentPin = nullptr;
		    connectionContinue = osgVDFBasePin::VisualConnectionsUpdater();

            connectingStarted = false;
            stateMutex.unlock();
	    }
    }catch(...){
        connectingStarted = false;
        stateMutex.unlock();
    }

	return false;
}

void osgVDFBaseModel::savePinsStatus(const osgVDFBaseNode::Pins & pins){
	for(osgVDFBaseNode::Pins::const_iterator it = pins.begin(); it != pins.end(); ++it){
		connectingPrevPinsStatus[*it] = (*it)->getVisualStatus();
	}
}

void osgVDFBaseModel::highlightCompatiblePins(osgVDFBasePin * startPin, const osgVDFBaseNode::Pins & pins){
	for(osgVDFBaseNode::Pins::const_iterator it = pins.begin(); it != pins.end(); ++it){
		osgVDFBasePin::VisualStatus pinStatus = (*it)->getDynamicVisualStatus(startPin->getModelPin(), logicModel);
		if(pinStatus == osgVDFBasePin::ACTIVE){
			(*it)->setVisualStatus(pinStatus);
		}
	}
}

void osgVDFBaseModel::restorePinsStatus(){
	for(VPinPrevVisualStatus::iterator it = connectingPrevPinsStatus.begin(); it != connectingPrevPinsStatus.end(); ++it){
		it->first->setVisualStatus(it->second);
	}

	VPinPrevVisualStatus().swap(connectingPrevPinsStatus);
}

bool osgVDFBaseModel::onPinEnter(osgWidget::Event& ev)
{
    osgVDFBaseModel::ScopeLock lock(stateMutex);

	osgVDFBasePin * pin = dynamic_cast<osgVDFBasePin *>(ev.getWidget());

	std::vector<std::string> pinErrors = getPinErrors(pin->getModelPin());

	if(pinErrors.empty() == false){
		hideTooltip();

		std::vector<std::string>::iterator it = pinErrors.begin();
		std::string text(*it);

		++it;

		for( ; it != pinErrors.end(); ++it){
			text += "\n" + *it;
		}

		setTooltipText(text);

		osgWidget::XYCoord pos = pin->getParent()->getAbsoluteOrigin() + pin->getOrigin();
		pos.x() += pin->getWidth();
		pos.y() += pin->getHeight();

		showTooltip(pos);
		refreshFocused();
	}

	return false;
}

bool osgVDFBaseModel::onPinLeave(osgWidget::Event& ev)
{
	if(isTooltipVisible() == true){

		hideTooltip();
		refreshFocused();
	}

	return false;
}

bool osgVDFBaseModel::onNodeEnter(osgWidget::Event& ev)
{
    osgVDFBaseModel::ScopeLock lock(stateMutex);

	osgVDFBaseNode * node = dynamic_cast<osgVDFBaseNode *>(ev.getWindow());
	std::vector<std::string> nodeErrors = getNodeErrors(node->getModelNode());
	if(nodeErrors.empty() == false){
		hideTooltip();

		std::vector<std::string>::iterator it = nodeErrors.begin();
		std::string text(*it);

		++it;

		for( ; it != nodeErrors.end(); ++it){
			text += "\n" + *it;
		}

		setTooltipText(text);

		osgWidget::XYCoord pos = node->getAbsoluteOrigin();
		pos.x() += node->getWidth();
		pos.y() += node->getHeight();
		showTooltip(pos);
		//refreshFocused();
	}

	return false;
}

bool osgVDFBaseModel::onNodeLeave(osgWidget::Event& ev){
	return onPinLeave(ev);
}

std::vector<std::string> osgVDFBaseModel::getPinErrors(const dflm::PinPtr & pin){
	std::vector<std::string> ret;

	if(pin->getType() == dflm::Pin::IN){
		if(pin->isRequired() == true && pin->empty() == true){
			ret.push_back("Pin wymagany - musi być podłączony!");
		}
	}else if(pin->getType() == dflm::Pin::OUT && pin->empty() == false && pin->isComplete() == false){
		std::string message("Pin wyjsciowy jest połączony i zależny od pinów wejściowych:");
		auto dependant = pin->getDependantPins();
		for(auto it = dependant.begin(); it != dependant.end(); ++it){
			if((*it).lock()->empty() == true){
				message += " " + (*it).lock()->getName();
			}
		}

		message += ". Podłącz te piny!";
		ret.push_back(message);
	}

	return ret;
}

std::vector<std::string> osgVDFBaseModel::getNodeErrors(const dflm::NPtr & node){
	std::vector<std::string> ret;

	//if(dynamic_cast<chart::DFSourceNode*>(node.get()) == 0){
	if(utils::dynamic_pointer_cast<dflm::DFSourceNode>(node) == 0){
		if(node->beginIn() != node->endIn()){
			if(dflm::Node::anyInPinConnected(node) == false){
				ret.push_back("Wezel musi mieć podłączony minimum jeden pin wejściowy");
			}

			//const dflm::Node::Pins & pinsSet = node->getInPins();
			for(auto it = node->beginIn(); it != node->endIn(); ++it){
				if(getPinErrors(*it).empty() == false){
					ret.push_back("Problem z pinami wejściowymi - sprawdz piny wejściowe");
					break;
				}
			}
		}

		//if(node->getOutPins().empty() == false){
		//	const dflm::Node::Pins & pinsSet = node->getOutPins();
			for(auto it = node->beginOut(); it != node->endOut(); ++it){
				if(getPinErrors(*it).empty() == false){
					ret.push_back("Problem z pinami wyjsciowymi - sprawdz piny wyjsciowe");
					break;
				}
			}
		//}
	}

	return ret;
}

osgVDFBaseModel::VisualSelectionManager::VisualSelectionManager(osgVDFBaseModel * m) : model(m), visualSelectionStarted(false){

}

osgVDFBaseModel::VisualSelectionManager::~VisualSelectionManager(){

}

bool osgVDFBaseModel::VisualSelectionManager::handle(const osgGA::GUIEventAdapter& gea,
	osgGA::GUIActionAdapter&      gaa,
	osg::Object*                  obj,
	osg::NodeVisitor*             nv
	){

		//do this if the descriptor is set
		//check if mouse was pushed - left button!!

		if(visualSelectionStarted == false && model->isPointFree(osgWidget::XYCoord(gea.getX(), gea.getY())) == false){
			return false;
		}

		model->focusedNode = nullptr;

		osgGA::GUIEventAdapter::EventType e = gea.getEventType();
		switch(e){
		case osgGA::GUIEventAdapter::PUSH:
			visualSelectionStarted = gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ? true : false;
			if(visualSelectionStarted == true){
				//reset previous selection
				if(model->areKeysPressed(model->selectionActionKeys) == false){
					model->deselectAllNodes();
				}

				//prepare first frame position
				model->selectionStartPose = osgWidget::XYCoord(gea.getX(), gea.getY());
				model->visualSelection = model->createVisualSelection(model->selectionStartPose);
				model->addChild(model->visualSelection);
			}
			break;

		case osgGA::GUIEventAdapter::DRAG:
			if(visualSelectionStarted == true){
				//continue with frame
				model->continueVisulaSelection(model->visualSelection, osgWidget::XYCoord(gea.getX(), gea.getY()));

				osg::BoundingBox locBB(std::min(gea.getX(), model->selectionStartPose.x()),
					std::min(gea.getY(), model->selectionStartPose.y()), 0,
					std::max(gea.getX(), model->selectionStartPose.x()),
					std::max(gea.getY(), model->selectionStartPose.y()), 0);

				osgVDFBaseModel::VNodes currentlySelectedNodes =  model->getVisualNodesInArea(locBB);

				VNodes cs;
				for(auto it = currentlySelectedNodes.begin(); it != currentlySelectedNodes.end(); ++it){
					cs.insert(*it);
					model->selectVisualNode(*it);
				}

				std::vector<osgVDFBaseNode*> diff(model->selectedVNodes.size());
				auto endDiff = std::set_difference(model->selectedVNodes.begin(), model->selectedVNodes.end(),
					cs.begin(), cs.end(), diff.begin());

				//deselect missing
				for(auto it = diff.begin(); it != endDiff; ++it){
					model->deselectVisualNode(*it, model->visualNodes);
				}
			}

			break;

		case osgGA::GUIEventAdapter::RELEASE:
			if(visualSelectionStarted == true){
				visualSelectionStarted = false;

				//m_pModel->pointerMove(-FLT_MAX, -FLT_MAX);
				// wyczyszczenie _lastPush
				//m_pModel->mouseReleasedLeft(0, 0);

				//remove selection frame
				model->removeChild(model->visualSelection);
				model->visualSelection = nullptr;
			}
			break;
		}

		return false;
}

osgVDFBaseModel::ModelResizeHandler::ModelResizeHandler(osgVDFBaseModel * m) : model(m){

}

osgVDFBaseModel::ModelResizeHandler::~ModelResizeHandler(){

}

bool osgVDFBaseModel::ModelResizeHandler::handle(const osgGA::GUIEventAdapter& gea,
	osgGA::GUIActionAdapter&      gaa,
	osg::Object*                  obj,
	osg::NodeVisitor*             nv
	){

	if(gea.getEventType() == osgGA::GUIEventAdapter::RESIZE){
		model->modelBoundingBox.xMax() = gea.getWindowWidth();
		model->modelBoundingBox.yMax() = gea.getWindowHeight();

		if(model->toolbar != 0){
			//resize toolbar to fit width, keep height
			model->toolbar->resize(gea.getWindowWidth(), model->toolbar->getHeight());
		}
	}

	return false;
}


osgVDFBaseModel::UserSpaceClick::UserSpaceClick(osgVDFBaseModel * m) : model(m){

}

osgVDFBaseModel::UserSpaceClick::~UserSpaceClick(){

}

const osgVDFBaseNode* osgVDFBaseModel::UserSpaceClick::getTopNodeInPoint(const osgWidget::XYCoord & pos) const
{
    const osgVDFBaseNode* ret = nullptr;
    for(auto it = model->visualNodes.begin(); it != model->visualNodes.end(); ++it){
        if(osgui::Utils2D::generateBox(*it).contains(osg::Vec3f(pos.x(), pos.y(), 0)) == true ){
            if(ret == nullptr || (ret->getZ() < (*it)->getZ())){
                ret = *it;
            }
        }
    }

    return ret;
}

bool osgVDFBaseModel::UserSpaceClick::handle(const osgGA::GUIEventAdapter& gea,
	osgGA::GUIActionAdapter&      gaa,
	osg::Object*                  obj,
	osg::NodeVisitor*             nv
	){

    if(model->contextMenuOn == true && gea.getEventType() == osgGA::GUIEventAdapter::PUSH && gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON){
        model->contextMenu->hideMenu();
    }

    if(model->clickNodeAction && gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && gea.getEventType() == osgGA::GUIEventAdapter::PUSH){
        //pobierz węzły pod tą pozycją
        osgWidget::XYCoord pos(gea.getX(), gea.getY());
        const osgVDFBaseNode* node = getTopNodeInPoint(pos);

        if(node != nullptr){
            model->clickNodeAction(node, pos);
        }
    }else if(model->doubleClickNodeAction && gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && gea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK){
        osgWidget::XYCoord pos(gea.getX(), gea.getY());
        const osgVDFBaseNode* node = getTopNodeInPoint(pos);

        if(node != nullptr){
            model->doubleClickNodeAction(node, pos);
        }
    }else if(model->contextNodeAction && gea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON && gea.getEventType() == osgGA::GUIEventAdapter::PUSH){
        osgWidget::XYCoord pos(gea.getX(), gea.getY());
        const osgVDFBaseNode* node = getTopNodeInPoint(pos);

        if(node != nullptr){
            model->contextNodeAction(node, pos);
        }
    }

	//do this if the descriptor is set
	//check if mouse was pushed - left button!!
	if(model->toolbarVisible == true && model->lastButton != 0 && model->lastButton->isToggled() == true && model->currentNodeDescriptor != 0 &&
		gea.getEventType() == osgGA::GUIEventAdapter::PUSH && gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
		model->isPointFree(osgWidget::XYCoord(gea.getX(), gea.getY())) == true){

		static int nodeCounter = 0;

		if(model->toolbarVisible == true && model->modelBoundingBox.contains(osg::Vec3f(gea.getX(), gea.getY(), 0)) == false){
			//jeśli klik w toolbar to nic nie robimy!!
			return false;
		}

		dflm::NPtr node = model->currentNodeDescriptor->createNode();
		std::stringstream ss;
		ss << nodeCounter++;


		//to ensure unique names for nodes!! important for connections unique naming!!
		//node->setNodeName(ss.str() + node->getName());
		model->addNode(node, 0, node->getName(), osgWidget::XYCoord(gea.getX(), gea.getY()));
		model->focusedNode = model->getVisualNode(node);
		//osgWidget::XYCoord nSize = model->getNodeSize(node);
		//model->setNodeAbsolutePosition(node, osgWidget::XYCoord(gea.getX() - nSize.x() / 2.0, gea.getY() - nSize.y() / 2.0));
	}else if(model->contextMenuOn == false && gea.getEventType() == osgGA::GUIEventAdapter::PUSH && gea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON){
		//collect connections in nearby to delete
		osgWidget::XYCoord pos(gea.getX(), gea.getY());
		//go through all points in all connections and identify the closest one to delete
		ConnectionsActionsMapping toDelete;

		for(osgVDFBaseModel::RevConnectionsMapping::iterator it = model->connectionsGraphToLogical.begin();
			it != model->connectionsGraphToLogical.end(); ++it){

			osg::Geometry * geom = dynamic_cast<osg::Geometry*>(it->first->getDrawable(0));
			osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			bool inserted = false;
			float minDist = std::numeric_limits<float>::max();
			unsigned int idx = 0;
			for(unsigned int i = 0; i < vertexData->size(); ++i){
				float distLoc = std::pow(pos.x() - (*vertexData)[i][0],2) + std::pow(pos.y() - (*vertexData)[i][1],2);
				if(distLoc <= model->maxDistToDelConnection){
					//add to delete map
					//toDelete[it->second->getConnectionName()] = it->first;
                    toDelete[it->second->getConnectionName()] = std::make_pair(boost::bind(&osgVDFBaseModel::highlightConnection, model.get(), it->first.get()), boost::bind(&osgVDFBaseModel::disconnect, model.get(), it->second));
					inserted = true;
					break;
				}else if(distLoc < minDist){
					idx = i;
					minDist = distLoc;
				}
			}

			if(inserted == false && vertexData->size() > 1){
				double lineDist = std::numeric_limits<double>::max();
				double segDist = std::numeric_limits<double>::max();
				//use closest found element to interpolate
				if(idx == 0){
					DistanceFromLine(gea.getX(), gea.getY(), (*vertexData)[idx][0], (*vertexData)[idx][1], (*vertexData)[1][0], (*vertexData)[1][1], segDist, lineDist);
				}else if(idx == vertexData->size()-1){
					DistanceFromLine(gea.getX(), gea.getY(), (*vertexData)[idx][0], (*vertexData)[idx][1], (*vertexData)[idx-1][0], (*vertexData)[idx-1][1], segDist, lineDist);
				}else{
					double tmpSegDist = std::numeric_limits<double>::max();
					DistanceFromLine(gea.getX(), gea.getY(), (*vertexData)[idx][0], (*vertexData)[idx][1], (*vertexData)[idx-1][0], (*vertexData)[idx-1][1], tmpSegDist, lineDist);
					DistanceFromLine(gea.getX(), gea.getY(), (*vertexData)[idx][0], (*vertexData)[idx][1], (*vertexData)[idx+1][0], (*vertexData)[idx+1][1], segDist, lineDist);
					segDist = std::min(segDist, tmpSegDist);
				}

				if(segDist <= model->maxDistToDelConnection){
					//toDelete[it->second->getConnectionName()] = it->first;
                    toDelete[it->second->getConnectionName()] = std::make_pair(boost::bind(&osgVDFBaseModel::highlightConnection, model.get(), it->first.get()), boost::bind(&osgVDFBaseModel::disconnect, model.get(), it->second));
				}
			}
		}

		if(toDelete.empty() == false){
			//show menu context

            //model->contextMenu->clearMenu();
            if(model->contextMenu != nullptr){
                model->contextMenu->hideMenu();
                model->removeChild(model->contextMenu);
                model->contextMenu = nullptr;
            }

			model->showConnectionsDeleteContextMenu(toDelete, boost::bind(&osgVDFBaseModel::clearHighlightedConnection, model.get()), osgWidget::XYCoord(gea.getX(), gea.getY()));
		}
	}

	return false;
}

void osgVDFBaseModel::UserSpaceClick::DistanceFromLine(double cx, double cy, double ax, double ay ,
					  double bx, double by, double &distanceSegment,
					  double &distanceLine)
{

	//
	// find the distance from the point (cx,cy) to the line
	// determined by the points (ax,ay) and (bx,by)
	//
	// distanceSegment = distance from the point to the line segment
	// distanceLine = distance from the point to the line (assuming
	//					infinite extent in both directions
	//

/*

Subject 1.02: How do I find the distance from a point to a line?


    Let the point be C (Cx,Cy) and the line be AB (Ax,Ay) to (Bx,By).
    Let P be the point of perpendicular projection of C on AB.  The parameter
    r, which indicates P's position along AB, is computed by the dot product
    of AC and AB divided by the square of the length of AB:

    (1)     AC dot AB
        r = ---------
            ||AB||^2

    r has the following meaning:

        r=0      P = A
        r=1      P = B
        r<0      P is on the backward extension of AB
        r>1      P is on the forward extension of AB
        0<r<1    P is interior to AB

    The length of a line segment in d dimensions, AB is computed by:

        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 + ... + (Bd-Ad)^2)

    so in 2D:

        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 )

    and the dot product of two vectors in d dimensions, U dot V is computed:

        D = (Ux * Vx) + (Uy * Vy) + ... + (Ud * Vd)

    so in 2D:

        D = (Ux * Vx) + (Uy * Vy)

    So (1) expands to:

            (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
        r = -------------------------------
                          L^2

    The point P can then be found:

        Px = Ax + r(Bx-Ax)
        Py = Ay + r(By-Ay)

    And the distance from A to P = r*L.

    Use another parameter s to indicate the location along PC, with the
    following meaning:
           s<0      C is left of AB
           s>0      C is right of AB
           s=0      C is on AB

    Compute s as follows:

            (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
        s = -----------------------------
                        L^2


    Then the distance from C to P = |s|*L.

*/


	double r_numerator = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
	double r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
	double r = r_numerator / r_denomenator;

    double s =  ((ay-cy)*(bx-ax)-(ax-cx)*(by-ay) ) / r_denomenator;

	distanceLine = fabs(s)*sqrt(r_denomenator);

	if ( (r >= 0) && (r <= 1) )
	{
		distanceSegment = distanceLine;
	}
	else
	{
		double dist1 = (cx-ax)*(cx-ax) + (cy-ay)*(cy-ay);
		double dist2 = (cx-bx)*(cx-bx) + (cy-by)*(cy-by);
		if (dist1 < dist2)
		{
			distanceSegment = sqrt(dist1);
		}
		else
		{
			distanceSegment = sqrt(dist2);
		}
	}

	return;
}

void osgVDFBaseModel::contextMenuActionWrapper(const ConnectionAction & action, const std::string& name, bool state)
{
    action();
}

bool osgVDFBaseModel::onLeaveContextMenu(osgWidget::Event& ev)
{
    (*((ConnectionAction*)ev.getData()))();
    return true;
}

void osgVDFBaseModel::showConnectionsDeleteContextMenu(const osgVDFBaseModel::ConnectionsActionsMapping & toDelete,
    const ConnectionAction & leaveContextMenu, const osgWidget::XYCoord & pos){

    contextMenu = new osgui::ContextMenu();

    contextMenu->addEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE);
    contextMenu->addCallback(new osgWidget::Callback(&osgVDFBaseModel::onLeaveContextMenu, this, osgWidget::EVENT_MOUSE_LEAVE, (void*)&leaveContextMenu));

    contextMenu->setMenuOnCloseCallback(std::string(), boost::bind(&osgVDFBaseModel::onCloseContextMenu, this, _1));

    addChild(contextMenu);



    for(ConnectionsActionsMapping::const_iterator it = toDelete.begin(); it != toDelete.end(); ++it){
        osgui::ContextMenu::OnClickCallback cc = boost::bind(&osgVDFBaseModel::contextMenuActionWrapper, it->second.second, _1, _2);
        osgui::ContextMenu::OnHoverCallback hc = boost::bind(&osgVDFBaseModel::contextMenuActionWrapper, it->second.first, _1, _2);

		contextMenu->addMenuItem(it->first, false, cc, hc);
	}



    setFocused(contextMenu);
	hideConnections();
	contextMenu->showMenu(pos);
    contextMenuOn = true;
}

void osgVDFBaseModel::highlightConnection(osg::Geode * connection)
{
    clearHighlightedConnection();
    setVisualConnectionColor(connection, visualConnectionHighlightColor);
    lastHighlightedConnection = connection;
}

void osgVDFBaseModel::clearHighlightedConnection()
{
    if(lastHighlightedConnection != nullptr){
        setVisualConnectionColor(lastHighlightedConnection, visualConnectionNormalColor);
        lastHighlightedConnection = nullptr;
    }
}

void osgVDFBaseModel::removeConnection(const dflm::ConnPtr & connection, const std::string & connectionName, bool checked){
	if(lastHighlightedConnection == connectionsLogicalToGraph[connection]){
		lastHighlightedConnection = nullptr;
	}

	disconnect(connection);

}

void osgVDFBaseModel::onCloseContextMenu(osgui::ContextMenu * menu){
	contextMenuOn = false;

    setFocused(nullptr);

	if(lastHighlightedConnection != nullptr){
		setVisualConnectionColor(lastHighlightedConnection, visualConnectionNormalColor);
		lastHighlightedConnection = nullptr;
	}
}

void osgVDFBaseModel::refreshFocused(osgWidget::Window * toFocus){
	if(focusedNode != nullptr){
		focusedNode->setStrata(osgWidget::Window::STRATA_NONE);
		setFocused(focusedNode);
	}else if(selectedVNodes.empty() == false){
		setFocused(*(selectedVNodes.begin()));
	}else if(visualNodes.empty() == false){
		setFocused(*(visualNodes.begin()));
	}else{
		setFocused(toFocus);
	}

	hideConnections();
}

osgWidget::Color osgVDFBaseModel::getVisualConnectionNormalColor() const
{
    return visualConnectionNormalColor;
}

void osgVDFBaseModel::setVisualConnectionNormalColor( const osgWidget::Color & color)
{
    visualConnectionNormalColor = color;
}

osgWidget::Color osgVDFBaseModel::getVisualConnectionHighlightColor() const
{
    return visualConnectionHighlightColor;
}

void osgVDFBaseModel::setVisualConnectionHighlightColor( const osgWidget::Color & color)
{
    visualConnectionHighlightColor = color;
}

//--------------------------------------- NODES ACTIONS -----------------------------------------------------------

void osgVDFBaseModel::setNodeClickAction(const NodeAction & clickAction)
{
    this->clickNodeAction = clickAction;
}

const osgVDFBaseModel::NodeAction & osgVDFBaseModel::getNodeClickAction() const
{
    return clickNodeAction;
}

void osgVDFBaseModel::setNodeDoubleClickAction(const NodeAction & doubleClickAction)
{
    this->doubleClickNodeAction = doubleClickAction;
}

const osgVDFBaseModel::NodeAction & osgVDFBaseModel::getNodeDoubleClickAction() const
{
    return doubleClickNodeAction;
}

void osgVDFBaseModel::setNodeDeleteAction(const NodeDeleteAction & deleteAction)
{
    this->deleteNodeAction = deleteAction;
}

const osgVDFBaseModel::NodeDeleteAction & osgVDFBaseModel::getNodeDeleteAction() const
{
    return deleteNodeAction;
}

void osgVDFBaseModel::setNodeContextAction(const NodeAction & contextAction)
{
    this->contextNodeAction = contextAction;
}

const osgVDFBaseModel::NodeAction & osgVDFBaseModel::getNodeContextAction() const
{
    return contextNodeAction;
}

}
