#include "StdAfx.h"
#include <vdfmlib/osgVDFBasePin.h>
#include <vdfmlib/osgVDFBaseModel.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

osgVDFBasePin::osgVDFBasePin (const std::string& name) : osgWidget::Widget("\\pin\\"+name),
    centerChanged(false), visualParentNode(nullptr), visualModel(nullptr)
    {

    /*UTILS_ASSERT((pin != nullptr, "Nieprawidlowy pin logiczny"));
    UTILS_ASSERT((model != nullptr, "Nieprawidlowy model logiczny"));*/
}

osgVDFBasePin::osgVDFBasePin (const osgVDFBasePin& pin, const osg::CopyOp& co) : Widget(pin),
	modelPin(pin.modelPin)
{

}


osgVDFBasePin::~osgVDFBasePin(void)
{
}

const dflm::PinPtr & osgVDFBasePin::getModelPin() const
{
	return modelPin;
}

osgVDFBaseNode* osgVDFBasePin::getVisualNode() const
{
	return visualParentNode;
}

void osgVDFBasePin::addConnection(osg::Geode * connection, osgVDFBasePin * other)
{
	if(connections.find(connection) != connections.end()) {
		throw std::runtime_error("Visual connection already exist in pin!");
	}

    connections[connection] = other;
}

void osgVDFBasePin::removeConnection(osg::Geode * connection)
{
	auto it = connections.find(connection);
    if(it == connections.end()){
        throw std::runtime_error("Connection not exist in pin!");
    }

    connections.erase(it);
}

const osgVDFBasePin::Connections & osgVDFBasePin::getConnections() const
{
	return connections;
}

osgVDFBasePin::VisualStatus osgVDFBasePin::getStaticVisualStatus() const
{
	VisualStatus ret = OK;

	switch(modelPin->getStaticStatus()){
	case dflm::Pin::REQUIRED:
		ret = osgVDFBasePin::REQUIRED;
		break;

	case dflm::Pin::INCOMPLETE:
		ret = osgVDFBasePin::INCOMPLETE;
		break;

	case dflm::Pin::CONNECTED:
		ret = osgVDFBasePin::CONNECTED;
		break;
	}

	return ret;
}

osgVDFBasePin::VisualStatus osgVDFBasePin::getDynamicVisualStatus(osgVDFBasePin * refPin, const dflm::MPtr & model) const
{
	return getDynamicVisualStatus(refPin->getModelPin(), model);
}

osgVDFBasePin::VisualStatus osgVDFBasePin::getDynamicVisualStatus(const dflm::PinPtr & refPin, const  dflm::MPtr & model) const
{
	VisualStatus ret = ACTIVE;

	switch(modelPin->getDynamicStatus(refPin, model)){
	case dflm::Pin::COMPATIBLE:
		if(modelPin->getType() == dflm::Pin::IN && modelPin->empty() == false){
			ret = osgVDFBasePin::INCOMPATIBLE;
		}
		break;

	case dflm::Pin::COMPATIBLE_CYCLE:
	case dflm::Pin::COMPATIBLE_FULL:
	case dflm::Pin::INCOMPATIBLE:
		ret = osgVDFBasePin::INCOMPATIBLE;
		break;

	}
	
	return ret;
}

void osgVDFBasePin::setVisualStatus(VisualStatus pinVisualStatus)
{
	if(visualStatus != pinVisualStatus){
		visualStatus = pinVisualStatus;
		graphSetStatus(pinVisualStatus);
	}
}

osgVDFBasePin::VisualStatus osgVDFBasePin::getVisualStatus() const
{
	return visualStatus;
}

const osgWidget::XYCoord & osgVDFBasePin::getCenterPosition()
{
	if(centerChanged == true){
		centerPosition = getParent()->getAbsoluteOrigin() + getOrigin();
		centerPosition.x() += getWidth() / 2;
		centerPosition.y() += getHeight() / 2;
		centerChanged = false;
	}

	return centerPosition;
}

osgWidget::XYCoord osgVDFBasePin::getCenterPosition() const
{
	osgWidget::XYCoord ret = getParent()->getAbsoluteOrigin() + getOrigin();
	ret.x() += getWidth() / 2;
	ret.x() += getHeight() / 2;
	return ret;
}

void osgVDFBasePin::positioned()
{
	centerChanged = true;
    
	for(auto it = connections.begin(); it != connections.end(); ++it){
        connectionsUpdater(it->first, getCenterPosition());
    }
}

void osgVDFBasePin::managed(osgWidget::WindowManager* wm)
{
	graphSetStatus(visualStatus);
}

}
