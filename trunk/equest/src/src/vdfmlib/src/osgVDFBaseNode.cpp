#include "StdAfx.h"
#include <vdfmlib/osgVDFBaseNode.h>
#include <vdfmlib/osgVDFBaseModel.h>
#include <osgui/Utils2D.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

osgVDFBaseNode::osgVDFBaseNode(const std::string & nodeName) : osgWidget::Window(nodeName),
    visualModel(nullptr), visualStatus(OK), area(0)
{
    /*UTILS_ASSERT((modelNode != nullptr, "Nieprawidlowy wezel logiczny"));
    UTILS_ASSERT((model != nullptr, "Nieprawidlowy model wizualny"));*/
}

osgVDFBaseNode::~osgVDFBaseNode(void)
{

}

osgWidget::point_type osgVDFBaseNode::getArea() const
{
    return area;
}

void osgVDFBaseNode::setArea(float area)
{
    UTILS_ASSERT((area > 0), "Nieprawidlowa powierzchnia węzła");
    this->area = area;
}

const dflm::NPtr & osgVDFBaseNode::getModelNode() const
{
	return modelNode;
}

osgVDFBaseModel * osgVDFBaseNode::getVisualModel() const
{
	return visualModel;
}

void osgVDFBaseNode::deleteNode()
{
	if(visualModel == nullptr){
		throw std::runtime_error("Visual node tries to delete itself without visual model!");
	}

	visualModel->deleteNodeSelf(this);
}

void osgVDFBaseNode::loggZCoordinates() const
{
	std::cout << "Logging node:\t" << this->getName() << std::endl;
	std::cout << "Z coordinates of all elements:" << std::endl;
	std::cout << "node (this window):\t" << osgui::Utils2D::calcAbsZ(this) << std::endl;
}

void osgVDFBaseNode::addInPin(osgVDFBasePin * inPin, const std::string & pinName)
{
	UTILS_ASSERT((inPin != nullptr), "Bledny vizualny pin!");

    if(inPin->visualParentNode != nullptr){
        throw std::runtime_error("Visual input pin already assigned to a node");
    }

	inPin->visualParentNode = this;
    inPins.insert(inPin);
	graphAddInPin(inPin,pinName);
}

void osgVDFBaseNode::addOutPin(osgVDFBasePin * outPin, const std::string & pinName)
{
    UTILS_ASSERT((outPin != nullptr), "Bledny vizualny pin!");

    if(outPin->visualParentNode != nullptr){
        throw std::runtime_error("Visual output pin already assigned to a node");
    }

    outPin->visualParentNode = this;
    outPins.insert(outPin);
	graphAddOutPin(outPin,pinName);
}

const osgVDFBaseNode::Pins & osgVDFBaseNode::getInPins() const
{
	return inPins;
}
const osgVDFBaseNode::Pins & osgVDFBaseNode::getOutPins() const
{
	return outPins;
}

void osgVDFBaseNode::setVisualStatus(osgVDFBaseNode::VisualStatus nodeVisualStatus)
{
	if(nodeVisualStatus != visualStatus){
		visualStatus = nodeVisualStatus;
		graphSetStatus(visualStatus);
	}
}

osgVDFBaseNode::VisualStatus osgVDFBaseNode::getVisualStatus() const
{
	return visualStatus;
}

void osgVDFBaseNode::setCollisionNodes(const VNodes & vnodes)
{
    if(collisionNodes.empty() == true){
        collisionNodes = vnodes;
        for(auto it = collisionNodes.begin(); it != collisionNodes.end(); ++it){
            (*it)->collisionNodes.insert(this);
        }
    }else if(vnodes.empty() == true){
        for(auto it = collisionNodes.begin(); it != collisionNodes.end(); ++it){
            (*it)->collisionNodes.erase(this);
        }

        VNodes().swap(collisionNodes);
    }else{
        VNodesDifference difference(std::max(collisionNodes.size(), vnodes.size()));
        auto it = std::set_difference(collisionNodes.begin(), collisionNodes.end(), vnodes.begin(), vnodes.end(), difference.begin());

        for(auto iT = difference.begin(); iT != it; ++iT){
            (*iT)->collisionNodes.erase(this);
        }

        it = std::set_difference(vnodes.begin(), vnodes.end(), collisionNodes.begin(), collisionNodes.end(), difference.begin());

        for(auto iT = difference.begin(); iT != it; ++iT){
            (*iT)->collisionNodes.insert(this);
        }

        collisionNodes = vnodes;
    }
}

const osgVDFBaseNode::VNodes & osgVDFBaseNode::getCollisionNodes() const
{
    return collisionNodes;
}

}
