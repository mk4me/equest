#include "NewVdfPCH.h"
#include "CanvasStyleEditor.h"
#include "ConnectionsFactory.h"
#include "NodesFactory.h"
#include "PinsFactory.h"
#include "DarkConnectionsFactory.h"
#include "DarkBackgroundStrategy.h"
#include "DarkNodesFactory.h"
#include "DarkPinsFactory.h"
#include "SimpleItem.h"
#include "BackgroundsFactory.h"

using namespace vdf;

CanvasStyleEditor::CanvasStyleEditor()
{
    registerConnectionsFactory(IConnectionsFactoryPtr(new ConnectionsFactory));
    registerNodesFactory(INodesFactoryPtr(new NodesFactory));
	registerPinsFactory(IPinsFactoryPtr(new PinsFactory));

	registerConnectionsFactory(IConnectionsFactoryPtr(new DarkConnectionsFactory));
	registerNodesFactory(INodesFactoryPtr(new DarkNodesFactory));
	registerPinsFactory(IPinsFactoryPtr(new DarkPinsFactory));

	registerBackgroundsFactory(IBackgroundsFactoryPtr(new BackgroundsFactory<DarkBackgroundStrategy>("Dark", "Dark desc", QUuid())));
}

CanvasStyleEditor::~CanvasStyleEditor()
{

}

void CanvasStyleEditor::registerNodesFactory( INodesFactoryPtr nFactory )
{
    currentNodesFactory = nFactory;
    nodesFactories.push_back(nFactory);
}

void CanvasStyleEditor::registerPinsFactory( IPinsFactoryPtr pFactory )
{
    currentPinsFactory = pFactory;
    pinsFactories.push_back(pFactory);
}

void CanvasStyleEditor::registerConnectionsFactory( IConnectionsFactoryPtr pFactory )
{
    currentConnectionsFactory = pFactory;
    connectionsFactories.push_back(pFactory);
}

std::string CanvasStyleEditor::serialize() const
{
    return std::string();
}

void CanvasStyleEditor::deserialize( const std::string & src )
{

}

IConnectionsFactoryPtr CanvasStyleEditor::getConnectionFactory( int index )
{
	return connectionsFactories[index];
}

int CanvasStyleEditor::getNumConnectionFactories() const
{
	return static_cast<int>(connectionsFactories.size());
}

IPinsFactoryPtr CanvasStyleEditor::getPinFactory( int index )
{
	return pinsFactories[index];
}

int CanvasStyleEditor::getNumPinFactories() const
{
	return static_cast<int>(pinsFactories.size());
}

INodesFactoryPtr CanvasStyleEditor::getNodeFactory( int index )
{
	return nodesFactories[index];
}

int CanvasStyleEditor::getNumNodeFactories() const
{
	return static_cast<int>(nodesFactories.size());
}

CanvasStyleEditor::ConnectionsFactoriesConstRange CanvasStyleEditor::getConnectionsRange() const
{
	return boost::make_iterator_range(connectionsFactories.cbegin(), connectionsFactories.cend());
}

CanvasStyleEditor::ConnectionsFactoriesRange CanvasStyleEditor::getConnectionsRange()
{
	return boost::make_iterator_range(connectionsFactories.begin(), connectionsFactories.end());
}

CanvasStyleEditor::PinsFactoriesConstRange CanvasStyleEditor::getPinsRange() const
{
	return boost::make_iterator_range(pinsFactories.cbegin(), pinsFactories.cend());
}

CanvasStyleEditor::PinsFactoriesRange CanvasStyleEditor::getPinsRange()
{
	return boost::make_iterator_range(pinsFactories.begin(), pinsFactories.end());
}

CanvasStyleEditor::NodesFactoriesConstRange CanvasStyleEditor::getNodesRange() const
{
	return boost::make_iterator_range(nodesFactories.cbegin(), nodesFactories.cend());
}

CanvasStyleEditor::NodesFactoriesRange CanvasStyleEditor::getNodesRange()
{
	return boost::make_iterator_range(nodesFactories.begin(), nodesFactories.end());
}

void CanvasStyleEditor::setCurrentConnectionsFactory( IConnectionsFactoryPtr val )
{
	currentConnectionsFactory = val;
}

void CanvasStyleEditor::setCurrentNodesFactory( INodesFactoryPtr val )
{
	currentNodesFactory = val;
}

void CanvasStyleEditor::setCurrentPinsFactory( IPinsFactoryPtr val )
{
	currentPinsFactory = val;
}

IPinsFactoryConstPtr CanvasStyleEditor::getCurrentPinsFactory() const
{
	return currentPinsFactory;
}

IConnectionsFactoryConstPtr CanvasStyleEditor::getCurrentConnectionsFactory() const
{
	return currentConnectionsFactory;
}

INodesFactoryConstPtr CanvasStyleEditor::getCurrentNodesFactory() const
{
	return currentNodesFactory;
}

void CanvasStyleEditor::registerBackgroundsFactory( IBackgroundsFactoryPtr pFactory )
{
	currentBackgroundFactory = pFactory;
	backgroundFactories.push_back(pFactory);
}

IBackgroundsFactoryPtr CanvasStyleEditor::getCurrentBackgroundsFactory() const
{
	return currentBackgroundFactory;
}

void CanvasStyleEditor::setCurrentBackgroundsFactory( IBackgroundsFactoryPtr val )
{
	currentBackgroundFactory = val;
}

CanvasStyleEditor::BackgroundsFactoriesRange CanvasStyleEditor::getBackgroundsRange()
{
	return boost::make_iterator_range(backgroundFactories.begin(), backgroundFactories.end());
}

CanvasStyleEditor::BackgroundsFactoriesConstRange CanvasStyleEditor::getBackgroundsRange() const
{
	return boost::make_iterator_range(backgroundFactories.cbegin(), backgroundFactories.cend());
}

int CanvasStyleEditor::getNumBackgroundFactories() const
{
	return static_cast<int>(backgroundFactories.size());
}

IBackgroundsFactoryPtr CanvasStyleEditor::getBackgroundFactory( int index )
{
	return backgroundFactories[index];
}




