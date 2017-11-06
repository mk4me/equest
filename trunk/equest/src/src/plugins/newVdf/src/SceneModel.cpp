#include "NewVdfPCH.h"
#include "SceneModel.h"
#include <plugins/newVdf/IVisualItem.h>
#include <plugins/newVdf/IVisualOutputPin.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IVisualSinkNode.h>
#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/IVisualProcessingNode.h>
#include <dflib/Model.h>
#include <dflib/STDFModelRunner.h>
#include <dflib/Connection.h>
#include <dflib/Node.h>
#include <type_traits>
//#include <corelib/IThreadPool.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "StyleMergedNode.h"
#include "SimpleMergedNode.h"
#include <plugins/newVdf/INodeConfiguration.h>
#include "TypesWindow.h"

using namespace vdf;

//class VDFThreadPool : public threadingUtils::IThreadPool
//{
//public:
//
//	VDFThreadPool(core::IThreadPool * tp) : tp_(tp)
//	{
//	}
//
//	virtual ~VDFThreadPool() {}
//
//	virtual const size_type maxThreads() const
//	{
//		return tp_->max();
//	}
//
//	virtual const size_type minThreads() const
//	{
//		return tp_->min();
//	}
//
//	virtual const size_type threadsCount() const
//	{
//		return tp_->count();
//	}
//
//	virtual threadingUtils::IThreadPtr getThread()
//	{
//		core::IThreadPool::Threads ret;
//		tp_->getThreads("VDF", ret, 1);
//		ret.front()->setDestination("Node thread");
//		return ret.front();
//	}
//
//	virtual void getThreads(const size_type groupSize, Threads & threads, const bool exact = true)
//	{
//		core::IThreadPool::Threads ret;
//		tp_->getThreads("VDF", ret, groupSize);
//		for (auto it = ret.begin(); it != ret.end(); ++it){
//			(*it)->setDestination("Node thread");
//			threads.push_back(*it);
//		}
//	}
//
//private:
//	core::IThreadPool * tp_;
//};

IVisualConnectionPtr SceneModel::addConnection(IVisualOutputPinPtr outputPin, IVisualInputPinPtr inputPin)
{
	UTILS_ASSERT(outputPin && inputPin);

	auto modelInput = inputPin->getModelPin();
	auto modelOutput = outputPin->getModelPin();

	df::IConnection* con = new df::Connection(modelOutput, modelInput);
	model->addConnection(con);

	IVisualConnectionPtr connection = builder.createConnection(outputPin, inputPin);
	connection->setModelConnection(con);
	addItem(connection);
	return connection;
}

SceneModel::SceneModel(CanvasStyleEditorPtr factories, core::IDataHierarchyManager * hm) :
builder(factories), hm(hm),
model(new df::Model)
//dfThreadFactory(new VDFThreadPool(threadpool))
{
}

void SceneModel::addItem(IVisualItemPtr item)
{
	auto* graphics = item->visualItem();
	graphics2Visual[graphics] = item;

	// nie trzeba tego robic dla elementow, ktore maja parenta
	//if (graphics2Visual.find(graphics->parentItem()) == graphics2Visual.end()) {
	if (!item->isType(IVisualItem::Pin)) {
		emit visualItemAdded(item);
	}

	if (item->isType(IVisualItem::Node)) {
		IVisualNodePtr node = utils::dynamic_pointer_cast<IVisualNode>(item);
		auto modelNode = node->getModelNode();
		if (modelNode) {
			addNode(modelNode);
		}
	}
	else if (item->isType(IVisualItem::InputPin)) {
		inputPins.push_back(utils::dynamic_pointer_cast<IVisualInputPin>(item));
	}
	else if (item->isType(IVisualItem::OutputPin)) {
		outputPins.push_back(utils::dynamic_pointer_cast<IVisualOutputPin>(item));
	}
	else if (item->isType(IVisualItem::Connection)) {
		auto c = utils::dynamic_pointer_cast<IVisualConnection>(item);
		c->getInputPin()->setConnection(c);
		c->getOutputPin()->addConnection(c);
	}
}

bool SceneModel::connectionPossible(IVisualPinPtr pin1, IVisualPinPtr pin2) const
{
	if (pin1->getType() == pin2->getType() || !pin1 || !pin2) {
		return false;
	}

	if (pin1->getParent().lock() == pin2->getParent().lock()) {
		return false;
	}

	PinResolver p(pin1, pin2);
	if (p.getInput()->getConnection().lock()) {
		return false;
	}

	auto modelInput = p.getInput()->getModelPin();
	auto modelOutput = p.getOutput()->getModelPin();
	return model->canConnect(modelOutput, modelInput);
}

void SceneModel::addNode(df::INode* node)
{
	UTILS_ASSERT(node);

	switch (node->type()) {
	case df::INode::PROCESSING_NODE: {
		df::IProcessingNode* processing = dynamic_cast<df::IProcessingNode*>(node);
		model->addNode(processing);
	} return;

	case df::INode::SINK_NODE: {
		df::ISinkNode* sink = dynamic_cast<df::ISinkNode*>(node);
		model->addNode(sink);
	} return;

	case df::INode::SOURCE_NODE: {
		df::ISourceNode* source = dynamic_cast<df::ISourceNode*>(node);
		auto dmNode = dynamic_cast<INodeHierarchyObserver*>(source);
		if (dmNode) {
			dmNode->refresh(hm, core::IDataHierarchyManagerReader::ChangeList());
		}
		model->addNode(source);
	} return;
	}

	UTILS_ASSERT(false);
}

void SceneModel::run()
{
	if (df::STDFModelRunner::verifyModel(model.get()) == true){
		df::STDFModelRunner runner;
		runner.start(model.get(), nullptr);
//		runner.join();
	}
	else{
//		//TODO
//		//obs³uga b³êdów
		UTILS_ASSERT(false);
	}
}

const SceneModel::Connections& SceneModel::getPossibleConections(IVisualPinPtr vpin)
{
	pinsHelper.impossible.clear();
	pinsHelper.possible.clear();
	// todo : optymalizacja
	for (auto it = inputPins.begin(); it != inputPins.end(); ++it) {
		if (*it != vpin) {
			if (connectionPossible(vpin, *it)) {
				pinsHelper.possible.push_back(*it);
			}
			else {
				pinsHelper.impossible.push_back(*it);
			}
		}
	}
	for (auto it = outputPins.begin(); it != outputPins.end(); ++it) {
		if (*it != vpin) {
			if (connectionPossible(vpin, *it)) {
				pinsHelper.possible.push_back(*it);
			}
			else {
				pinsHelper.impossible.push_back(*it);
			}
		}
	}
	return pinsHelper;
}

template<class VisualPinT>
void vdf::SceneModel::removePin(IVisualItemPtr item)
{
	auto pin = utils::dynamic_pointer_cast<VisualPinT>(item);
	UTILS_ASSERT(pin);

	if (std::is_same<VisualPinT, IVisualInputPin>::value) {
		auto inputPin = utils::dynamic_pointer_cast<IVisualInputPin>(pin);
		auto connection = inputPin->getConnection().lock();
		if (connection) {
			removeConnection(connection);
		}
		inputPins.remove(inputPin);
	}
	else if (std::is_same<VisualPinT, IVisualOutputPin>::value) {
		auto outputPin = utils::dynamic_pointer_cast<IVisualOutputPin>(pin);
		int count = outputPin->getNumConnections();
		for (int i = count - 1; i >= 0; --i) {
			auto connection = outputPin->getConnection(i).lock();
			if (connection) {
				removeConnection(connection);
			}
		}
		outputPins.remove(outputPin);
	}
	else {
		UTILS_ASSERT(false);
	}
}

template<class VisualT, class DFNodeT>
void SceneModel::removeNode(IVisualItemPtr item)
{
	auto node = utils::dynamic_pointer_cast<VisualT>(item);
	UTILS_ASSERT(node);
	auto* ptr = dynamic_cast<DFNodeT*>(node->getModelNode());
	UTILS_ASSERT(ptr);
	model->removeNode(ptr);

	if (std::is_same<VisualT, IVisualSourceNode>::value || std::is_base_of<IVisualSourceNode, VisualT>::value) {
		removeOutputPins(node);
	}

	if (std::is_same<VisualT, IVisualSinkNode>::value || std::is_base_of<IVisualSinkNode, VisualT>::value) {
		removeInputPins(node);
	}
}

void SceneModel::removeItem(IVisualItemPtr item)
{
	UTILS_ASSERT(!utils::dynamic_pointer_cast<IVisualConnection>(item));
	commonErase(item);

	switch (item->getType()) {
	case IVisualItem::ProcessingNode:
		removeNode<IVisualProcessingNode, df::ProcessingNode>(item);
		break;

	case IVisualItem::SinkNode:
		removeNode<IVisualSinkNode, df::SinkNode>(item);
		break;

	case IVisualItem::SourceNode:
		removeNode<IVisualSourceNode, df::SourceNode>(item);
		break;

	case (IVisualItem::InputPin) :
		removePin<IVisualInputPin>(item);
		break;

	case (IVisualItem::OutputPin) :
		removePin<IVisualOutputPin>(item);
		break;

	case (IVisualItem::Connection) :
		UTILS_ASSERT(false);
		break;
	}
}

IVisualItemPtr SceneModel::tryGetVisualItem(QGraphicsItem* item)
{
	auto it = graphics2Visual.find(item);
	if (it != graphics2Visual.end()) {
		return it->second;
	}

	return IVisualItemPtr();
}

SceneBuilder::VisualNodeWithPins SceneModel::merge(const QList<QGraphicsItem*>& items)
{
	QList<IVisualNodePtr> nodes = this->getVisualItems<IVisualNodePtr>(items);
	MergedItemPtr merged(new MergedItem());
	merged->nodes = nodes;

	std::vector<IVisualInputPinPtr> ipins;
	std::vector<IVisualOutputPinPtr> opins;

	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		auto graphicsItem = (*it)->visualItem();
		graphicsItem->setVisible(false);
		graphicsItem->update();
		IVisualSourceNodePtr source = utils::dynamic_pointer_cast<IVisualSourceNode>(*it);
		if (source) {
			int count = source->getNumOutputPins();
			for (int i = 0; i < count; ++i) {
				IVisualOutputPinPtr opin = source->getOutputPin(i);

				int numConnections = opin->getNumConnections();
				for (int i = 0; i < numConnections; ++i) {
					auto connection = opin->getConnection(i).lock();
					if (connection) {
						connection->visualItem()->setVisible(false);
					}
				}
				if (!numConnections) {
					opins.push_back(opin);
				}
			}
		}
		IVisualSinkNodePtr sink = utils::dynamic_pointer_cast<IVisualSinkNode>(*it);
		if (sink) {
			int count = sink->getNumInputPins();
			for (int i = 0; i < count; ++i) {
				IVisualInputPinPtr ipin = sink->getInputPin(i);
				auto connection = ipin->getConnection().lock();
				if (connection) {
					connection->visualItem()->setVisible(false);
				}
				else {
					ipins.push_back(sink->getInputPin(i));
				}
			}
		}
	}
	if (!ipins.empty() || !opins.empty()) {
		return  builder.createMerged("Merged", ipins, opins);
	}
	return SceneBuilder::VisualNodeWithPins();
}

void vdf::SceneModel::clearScene()
{
	auto nodes = getVisualItems<IVisualNodePtr>();
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		if ((*it)->isType(IVisualItem::Connection)) {
			removeConnection(*it);
		}
		else {
			removeItem(*it);
		}
	}
	UTILS_ASSERT(graphics2Visual.empty());
	UTILS_ASSERT(inputPins.empty());
	UTILS_ASSERT(outputPins.empty());
}

void vdf::SceneModel::removeInputPins(IVisualNodePtr node)
{
	IVisualSinkNodePtr sink = utils::dynamic_pointer_cast<vdf::IVisualSinkNode>(node);
	if (sink) {
		int count = sink->getNumInputPins();
		for (int i = 0; i < count; ++i) {
			removeItem(sink->getInputPin(i));
		}
	}
}

void vdf::SceneModel::removeOutputPins(IVisualNodePtr node)
{
	IVisualSourceNodePtr source = utils::dynamic_pointer_cast<vdf::IVisualSourceNode>(node);
	if (source) {
		int count = source->getNumOutputPins();
		for (int i = 0; i < count; ++i) {
			removeItem(source->getOutputPin(i));
		}
	}
}

void vdf::SceneModel::commonErase(IVisualItemPtr item)
{
	auto* graphics = item->visualItem();
	auto it = graphics2Visual.find(graphics);
	UTILS_ASSERT(it != graphics2Visual.end());
	graphics2Visual.erase(it);
	//if (graphics2Visual.find(graphics->parentItem()) == graphics2Visual.end()) {
	if (!item->isType(IVisualItem::Pin)) {
		emit visualItemRemoved(item);
	}
}

void vdf::SceneModel::removeConnection(IVisualOutputPinPtr outputPin, IVisualInputPinPtr inputPin)
{
	auto connections = getVisualItems<IVisualConnectionPtr>();

	for (auto it = connections.begin(); it != connections.end(); ++it) {
		IVisualConnectionPtr c = *it;
		if (c->getOutputPin() == outputPin && c->getInputPin() == inputPin) {
			commonErase(c);
			inputPin->setConnection(IVisualConnectionWeakPtr());
			outputPin->removeConnection(c);
			try {
				model->removeConnection(c->getModelConnection());
			}
			catch (std::exception&) {
				// TODO
			}
			return;
		}
	}

	UTILS_ASSERT(false);
}

void vdf::SceneModel::removeConnection(IVisualItemPtr item)
{
	auto c = utils::dynamic_pointer_cast<IVisualConnection>(item);
	UTILS_ASSERT(c);
	removeConnection(c->getOutputPin(), c->getInputPin());
}

void vdf::SceneModel::addNodeWithPins(const SceneBuilder::VisualNodeWithPins& nodeWithPins, const QPointF& pos)
{
	auto node = nodeWithPins.get<0>();
	auto vis = node->visualItem();
	vis->setPos(pos);

	addItem(node);

	SceneBuilder::Pins pins = nodeWithPins.get<1>();
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		addItem(*it);
	}

	pins = nodeWithPins.get<2>();
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		addItem(*it);
	}
}

vdf::SceneBuilder::VisualNodeWithPins vdf::SceneModel::Serializer::createItemByEntry(const std::string& name)
{
	return typesModel->createItemByEntry(QString::fromStdString(name));
}

vdf::SceneModel::Serializer::Infos vdf::SceneModel::Serializer::extractInfos() const
{
	auto nodes = model->getVisualItems<IVisualNodePtr>();
	std::map<IVisualNodePtr, int> node2idx;
	NodeInfo::Collection infos;
	int count = nodes.size();
	for (int i = 0; i < count; ++i) {
		NodeInfo ni;
		node2idx[nodes[i]] = i;
		ni.index = i;
		ni.id = boost::lexical_cast<std::string>(typesModel->getId(nodes[i]->getName()));
		ni.x = nodes[i]->visualItem()->x();
		ni.y = nodes[i]->visualItem()->y();
		ni.name = nodes[i]->getName().toStdString();
		infos.push_back(ni);
	}

	ConnectionInfo::Collection connections;
	auto connectionItems = model->getVisualItems<IVisualConnectionPtr>();
	for (auto it = connectionItems.begin(); it != connectionItems.end(); ++it) {
		IVisualInputPinPtr ipin = (*it)->getInputPin();
		IVisualOutputPinPtr opin = (*it)->getOutputPin();

		ConnectionInfo info;
		info.inputNodeIndex = node2idx[ipin->getParent().lock()];
		info.inputPinIndex = ipin->getIndex();
		info.outputNodeIndex = node2idx[opin->getParent().lock()];
		info.outputPinIndex = opin->getIndex();
		connections.push_back(info);
	}

	return std::make_pair(infos, connections);
}
