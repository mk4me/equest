#include "NewVdfPCH.h"
#include "TypesModel.h"
#include "CanvasStyleEditor.h"
#include "NewVdfWidget.h"
#include <plugins/newVdf/IDataSourceManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSource.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <corelib/PluginCommon.h>

using namespace vdf;

TypesModel::TypesModel(utils::ICommandStackPtr stack, CanvasStyleEditorPtr canvas, SceneModelPtr sceneModel, QObject* parent) :
	QObject(parent),
    canvas(canvas),
    sceneModel(sceneModel),
	commmandStack(stack)
{
}

void TypesModel::insert( const QString& name, const QPointF& scenePos )
{
    auto item = createItemByEntry(name);
	commmandStack->addCommand(utils::ICommandPtr(new AddToSceneCommand(sceneModel, item, scenePos)));
}


SceneBuilder::VisualNodeWithPins TypesModel::createItemByEntry( const QString& entry )
{
    SceneBuilder::VisualNodeWithPins item;
    
    auto it = name2node.find(entry);
    if (it != name2node.end()) {
        auto node = it->second;
        IOutputDescriptionConstPtr output = utils::dynamic_pointer_cast<const IOutputDescription>(node);
		IInputDescriptionConstPtr input = utils::dynamic_pointer_cast<const IInputDescription>(node);
        
        int out = 0;
		if (output) {
			std::vector<IOutputDescription::OutputInfo> outputInfo;
			output->getOutputInfo(outputInfo);
			out = static_cast<int>(outputInfo.size());
		}
        int in = 0;
		if (input) {
			std::vector<IInputDescription::InputInfo> inputInfo;
			input->getInputInfo(inputInfo);
			in = static_cast<int>(inputInfo.size());
		}
        item = sceneModel->getBuilder().createType(it->first, node->getIcon(), node->create(), in, out);
    }
    UTILS_ASSERT(item.get<0>() != nullptr);
    return item;
}

void TypesModel::update( const IDataProcessorManager* pm )
{
    auto rProcessors = pm->enumPrototypes();
    for(auto it = rProcessors.begin(); it != rProcessors.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
            name2node[name] = *it;
			emit entryAdded(name, (*it)->getIcon(), IVisualItem::ProcessingNode);
        }
    }
}

void TypesModel::update( const IDataSourceManager* sm )
{
    auto rSources = sm->enumPrototypes();
    for(auto it = rSources.begin(); it != rSources.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
			IWorkflowItemBasePtr base = *it;
            name2node[name] = *it;
			emit entryAdded(name, (*it)->getIcon(), IVisualItem::SourceNode);
        }
    }
}

void TypesModel::update( const IDataSinkManager* sm )
{
    auto rSinks = sm->enumPrototypes();
    for(auto it = rSinks.begin(); it != rSinks.end(); ++it ) {
        QString name = QString::fromStdString((*it)->getName());
        auto present = name2node.find(name);
        if (present == name2node.end()) {
            name2node[name] = *it;
			emit entryAdded(name, (*it)->getIcon(), IVisualItem::SinkNode);
        } 
    }
}

core::UniqueID vdf::TypesModel::getId( const QString& name ) const
{
    auto it = name2node.find(name);
    if (it != name2node.end()) {
        return it->second->getID();
    }

    throw std::runtime_error("Node not found");
}

