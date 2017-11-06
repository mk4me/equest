/********************************************************************
	created:	2012/12/27
	created:	27:12:2012   8:34
	filename: 	SceneModel.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SCENEMODEL_H__
#define HEADER_GUARD_NEWVDF__SCENEMODEL_H__

#include <utils/SmartPtr.h>
#include "CanvasStyleEditor.h"
#include <boost/tuple/tuple.hpp>
#include <dflib/IConnection.h>
#include <dflib/IModel.h>
#include "SceneBuilder.h"
#include <type_traits>
//#include <plugins/newVdf/TreeBuilder.h>
#include <corelib/IDataManagerReader.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <vector>
#include <corelib/PluginCommon.h>

namespace threadingUtils {
	class IThreadPool;
}

namespace vdf {
	class TypesModel;
	DEFINE_SMART_POINTERS(TypesModel)
	class IVisualItem;
	typedef utils::shared_ptr<df::IModel> IModelPtr;
	typedef utils::shared_ptr<df::IConnection> IConnectionPtr;

	class PinResolver
	{
	public:
		PinResolver(IVisualPinPtr p1, IVisualPinPtr p2)
		{
			if (p1->isType(IVisualItem::InputPin)) {
				input = utils::dynamic_pointer_cast<IVisualInputPin>(p1);
				output = utils::dynamic_pointer_cast<IVisualOutputPin>(p2);
			}
			else {
				input = utils::dynamic_pointer_cast<IVisualInputPin>(p2);
				output = utils::dynamic_pointer_cast<IVisualOutputPin>(p1);
			}

			UTILS_ASSERT(input && output);
		}

		operator IVisualInputPinPtr() { return input; }
		operator IVisualOutputPinPtr() { return output; }

		vdf::IVisualOutputPinPtr getOutput() const { return output; }
		vdf::IVisualInputPinPtr getInput() const { return input; }

	private:
		IVisualInputPinPtr input;
		IVisualOutputPinPtr output;
	};

	class TypesWindow;
	class SceneModel;
	DEFINE_SMART_POINTERS(SceneModel);

	class SceneModel : public QObject
	{
		Q_OBJECT;

	public:
		struct Connections
		{
			std::vector<IVisualPinPtr> possible;
			std::vector<IVisualPinPtr> impossible;
		};

		struct MergedItem
		{
			QList<IVisualNodePtr> nodes;
			IVisualNodePtr node;
			QList<IVisualInputPinPtr> inputPins;
			QList<IVisualOutputPinPtr> outputPins;
		};
		DEFINE_SMART_POINTERS(MergedItem);

	public:
		SceneModel(CanvasStyleEditorPtr factories, core::IDataHierarchyManager * hm);//, core::ThreadPool* threadpool = plugin::getThreadPool());
		virtual ~SceneModel() {}

		public slots:
		SceneBuilder::VisualNodeWithPins merge(const QList<QGraphicsItem*>& items);

	public:
		IVisualConnectionPtr addConnection(IVisualOutputPinPtr outputPin, IVisualInputPinPtr inputPin);
		void removeConnection(IVisualOutputPinPtr outputPin, IVisualInputPinPtr inputPin);
		void removeConnection(IVisualItemPtr item);

		void addItem(IVisualItemPtr item);
		void addNodeWithPins(const SceneBuilder::VisualNodeWithPins& nodeWidthPins, const QPointF& pos);
		void removeItem(IVisualItemPtr item);
		const SceneBuilder& getBuilder() const { return builder; }

		bool connectionPossible(IVisualPinPtr pin1, IVisualPinPtr pin2) const;
		const Connections& getPossibleConections(IVisualPinPtr vpin);

		IVisualItemPtr tryGetVisualItem(QGraphicsItem* item);

		void clearScene();

		template <class VisualItemT>
		QList<VisualItemT> getVisualItems()
		{
			QList<VisualItemT> ret;
			for (auto it = graphics2Visual.begin(); it != graphics2Visual.end(); ++it) {
				auto item = utils::dynamic_pointer_cast<typename VisualItemT::element_type>(it->second);
				if (item) {
					ret.push_back(item);
				}
			}

			return ret;
		}

		template <class VisualItemT>
		QList<VisualItemT> getVisualItems(const QList<QGraphicsItem *>& graphicsItems)
		{
			QList<VisualItemT> ret;

			QList<VisualItemT> items = getVisualItems<VisualItemT>();
			for (auto it = items.begin(); it != items.end(); ++it) {
				if (graphicsItems.contains((*it)->visualItem())) {
					ret.push_back(*it);
				}
			}
			return ret;
		}
		void run();

	signals:
		void visualItemAdded(IVisualItemPtr item);
		void visualItemRemoved(IVisualItemPtr item);

	private:
		void addNode(df::INode* node);
		template<class VisualT, class DFNodeT>
		void removeNode(IVisualItemPtr item);
		template<class VisualPinT>
		void removePin(IVisualItemPtr item);
		void removeOutputPins(IVisualNodePtr node);
		void removeInputPins(IVisualNodePtr node);
		void commonErase(IVisualItemPtr item);

	public:
		class Serializer
		{
		public:
			Serializer(SceneModelPtr sm, TypesModelPtr tm) : model(sm), typesModel(tm) {}
		private:
			struct NodeInfo
			{
				NodeInfo() : x(0.0f), y(0.0f), index(-1) {}
				std::string id;
				std::string name;
				float x;
				float y;
				int index;

				template <typename Archive>
				void serialize(Archive& ar, const unsigned int version)
				{
					ar & boost::serialization::make_nvp("id", id);
					ar & boost::serialization::make_nvp("name", name);
					ar & boost::serialization::make_nvp("x", x);
					ar & boost::serialization::make_nvp("y", y);
					ar & boost::serialization::make_nvp("index", index);
				}

				typedef std::vector<NodeInfo> Collection;
			};

			struct ConnectionInfo
			{
				ConnectionInfo() :
					outputNodeIndex(-1),
					outputPinIndex(-1),
					inputNodeIndex(-1),
					inputPinIndex(-1)
				{}
				int outputNodeIndex;
				int outputPinIndex;
				int inputNodeIndex;
				int inputPinIndex;

				template <typename Archive>
				void serialize(Archive& ar, const unsigned int version)
				{
					ar & boost::serialization::make_nvp("outputNodeIndex", outputNodeIndex);
					ar & boost::serialization::make_nvp("outputPinIndex", outputPinIndex);
					ar & boost::serialization::make_nvp("inputNodeIndex", inputNodeIndex);
					ar & boost::serialization::make_nvp("inputPinIndex", inputPinIndex);
				}

				typedef std::vector<ConnectionInfo> Collection;
			};

			typedef std::pair<NodeInfo::Collection, ConnectionInfo::Collection> Infos;
			friend class boost::serialization::access;
			template<class Archive>
			void save(Archive & ar, const unsigned int version) const
			{
				Infos i = extractInfos();
				ar  & boost::serialization::make_nvp("nodes", i.first);
				ar  & boost::serialization::make_nvp("connections", i.second);
			}
			template<class Archive>
			void load(Archive & ar, const unsigned int version)
			{
				NodeInfo::Collection nc;
				ConnectionInfo::Collection cc;
				ar & boost::serialization::make_nvp("nodes", nc);
				ar & boost::serialization::make_nvp("connections", cc);

				// TODO: sprawdzanie czy dane sa poprawne.
				model->clearScene();

				std::map<int, IVisualNodePtr> idx2Node;
				for (auto it = nc.begin(); it != nc.end(); ++it) {
					auto itm = createItemByEntry(it->name);
					idx2Node[it->index] = itm.get<0>();
					model->addNodeWithPins(itm, QPointF(it->x, it->y));
				}

				for (auto it = cc.begin(); it != cc.end(); ++it) {
					IVisualSourceNodePtr outNode = utils::dynamic_pointer_cast<IVisualSourceNode>(idx2Node[it->outputNodeIndex]);
					IVisualSinkNodePtr inNode = utils::dynamic_pointer_cast<IVisualSinkNode>(idx2Node[it->inputNodeIndex]);

					model->addConnection(
						outNode->getOutputPin(it->outputPinIndex),
						inNode->getInputPin(it->inputPinIndex)
						);
				}
			}

			Infos extractInfos() const;
			SceneBuilder::VisualNodeWithPins createItemByEntry(const std::string&);
			BOOST_SERIALIZATION_SPLIT_MEMBER();
			SceneModelPtr model;
			TypesModelPtr typesModel;
		};

	private:
		core::IDataHierarchyManager * hm;
		std::map<QGraphicsItem*, IVisualItemPtr> graphics2Visual;
		SceneBuilder builder;
		std::list<IVisualInputPinPtr> inputPins;
		std::list<IVisualOutputPinPtr> outputPins;
		IModelPtr model;
		mutable Connections pinsHelper;
		std::vector<MergedItemPtr> mergedItems;
		utils::shared_ptr<threadingUtils::IThreadPool> dfThreadFactory;
	};
}

#endif
