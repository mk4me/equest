/********************************************************************
    created:  2012/11/19
    created:  19:11:2012   10:04
    filename: CanvasStyleEditor.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CANVASSTYLEEDITOR_H__
#define HEADER_GUARD___CANVASSTYLEEDITOR_H__


#include <boost/range.hpp>
#include <plugins/newVdf/INodesFactory.h>
#include <plugins/newVdf/IPinsFactory.h>
#include <plugins/newVdf/IConnectionsFactory.h>
#include <plugins/newVdf/IBackgroundsFactory.h>

namespace vdf {

class CanvasStyleEditor 
{
public:
    typedef std::vector<INodesFactoryPtr> NodesFactoriesCollection;
    typedef std::vector<IPinsFactoryPtr> PinsFactoriesCollection;
    typedef std::vector<IConnectionsFactoryPtr> ConnectionsFactoriesCollection;
	typedef std::vector<IBackgroundsFactoryPtr> BackgroundFactoriesCollection;

    typedef boost::iterator_range<NodesFactoriesCollection::iterator> NodesFactoriesRange;
    typedef boost::iterator_range<NodesFactoriesCollection::const_iterator> NodesFactoriesConstRange;
    typedef boost::iterator_range<PinsFactoriesCollection::iterator> PinsFactoriesRange;
    typedef boost::iterator_range<PinsFactoriesCollection::const_iterator> PinsFactoriesConstRange;
    typedef boost::iterator_range<ConnectionsFactoriesCollection::iterator> ConnectionsFactoriesRange;
	typedef boost::iterator_range<ConnectionsFactoriesCollection::const_iterator> ConnectionsFactoriesConstRange;
	typedef boost::iterator_range<BackgroundFactoriesCollection::iterator> BackgroundsFactoriesRange;
	typedef boost::iterator_range<BackgroundFactoriesCollection::const_iterator> BackgroundsFactoriesConstRange;

public:
	explicit CanvasStyleEditor();
	virtual ~CanvasStyleEditor();

public:
	void registerNodesFactory(INodesFactoryPtr nFactory);
	void registerPinsFactory(IPinsFactoryPtr pFactory);
	void registerConnectionsFactory(IConnectionsFactoryPtr pFactory);
	void registerBackgroundsFactory(IBackgroundsFactoryPtr pFactory);
	virtual std::string serialize() const;
	virtual void deserialize(const std::string & src);

public:
    INodesFactoryConstPtr getCurrentNodesFactory() const;
    IConnectionsFactoryConstPtr getCurrentConnectionsFactory() const;
    IPinsFactoryConstPtr getCurrentPinsFactory() const;
	IBackgroundsFactoryPtr getCurrentBackgroundsFactory() const;

    void setCurrentPinsFactory(IPinsFactoryPtr val);
    void setCurrentNodesFactory(INodesFactoryPtr val);
    void setCurrentConnectionsFactory(IConnectionsFactoryPtr val);
	void setCurrentBackgroundsFactory(IBackgroundsFactoryPtr val);

    NodesFactoriesRange getNodesRange();
    NodesFactoriesConstRange getNodesRange() const;
    PinsFactoriesRange getPinsRange();
    PinsFactoriesConstRange getPinsRange() const;
    ConnectionsFactoriesRange getConnectionsRange();
	ConnectionsFactoriesConstRange getConnectionsRange() const;
	BackgroundsFactoriesRange getBackgroundsRange();
	BackgroundsFactoriesConstRange getBackgroundsRange() const;

	int getNumNodeFactories() const;
	INodesFactoryPtr getNodeFactory(int index);
	int getNumPinFactories() const;
	IPinsFactoryPtr getPinFactory(int index);
	int getNumConnectionFactories() const;
	IConnectionsFactoryPtr getConnectionFactory(int index);
	int getNumBackgroundFactories() const;
	IBackgroundsFactoryPtr getBackgroundFactory(int index);

private:
    NodesFactoriesCollection nodesFactories;
    PinsFactoriesCollection pinsFactories;
    ConnectionsFactoriesCollection connectionsFactories;
	BackgroundFactoriesCollection backgroundFactories;

    INodesFactoryPtr currentNodesFactory;
    IPinsFactoryPtr currentPinsFactory;
    IConnectionsFactoryPtr currentConnectionsFactory;
	IBackgroundsFactoryPtr currentBackgroundFactory;
};
DEFINE_SMART_POINTERS(CanvasStyleEditor);
}
#endif	//	HEADER_GUARD___CANVASSTYLEEDITOR_H__
