#include "NewVdfPCH.h"
#include "TypesWindow.h"
#include "CanvasStyleEditor.h"
#include "NewVdfWidget.h"
#include <plugins/newVdf/IDataSourceManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSource.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <corelib/PluginCommon.h>
#include "ui_TypesWindow.h"
#include "TypesModel.h"

using namespace vdf;

TypesWindow::TypesWindow(TypesModelPtr model, QWidget* parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    //model(model),
    ui(new Ui::TypesWindow())
{
    ui->setupUi(this);
    
    ui->processorsList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->processorsList->setDragEnabled(true);

    ui->sinksList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sinksList->setDragEnabled(true);
    
    ui->sourcesList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sourcesList->setDragEnabled(true);

	connect(model.get(), SIGNAL(entryAdded(const QString&, const QIcon&, IVisualItem::Type)), this, SLOT(addEntry(const QString&, const QIcon&, IVisualItem::Type)));
}

//void TypesWindow::insert( const QString& name, const QPointF& scenePos )
//{
//    auto item = createItemByEntry(name);
//	commmandStack->addCommand(utils::ICommandPtr(new AddToSceneCommand(sceneModel, item, scenePos)));
//}

void TypesWindow::addEntry( const QString& entry, const QIcon& icon, IVisualItem::Type type )
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setText(entry);

    item->setSizeHint(QSize(64, 64));

    switch (type)  {
    case IVisualItem::ProcessingNode:
        item->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/processor.png"): icon);
        ui->processorsList->addItem(item);
        break;

    case IVisualItem::SourceNode:
        item->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/source.png") : icon);
        ui->sourcesList->addItem(item);
        break;

    case IVisualItem::SinkNode:
        item->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/sink.png") : icon);
        ui->sinksList->addItem(item);
        break;

    default:
        UTILS_ASSERT(false);
    }
}
//
//SceneBuilder::VisualNodeWithPins TypesWindow::createItemByEntry( const QString& entry )
//{
//    SceneBuilder::VisualNodeWithPins item;
//    
//    auto it = name2node.find(entry);
//    if (it != name2node.end()) {
//        auto node = it->second;
//        IOutputDescriptionConstPtr output = utils::dynamic_pointer_cast<const IOutputDescription>(node);
//		IInputDescriptionConstPtr input = utils::dynamic_pointer_cast<const IInputDescription>(node);
//        
//        int out = 0;
//		if (output) {
//			std::vector<IOutputDescription::OutputInfo> outputInfo;
//			output->getOutputInfo(outputInfo);
//			out = static_cast<int>(outputInfo.size());
//		}
//        int in = 0;
//		if (input) {
//			std::vector<IInputDescription::InputInfo> inputInfo;
//			input->getInputInfo(inputInfo);
//			in = static_cast<int>(inputInfo.size());
//		}
//        item = sceneModel->getBuilder().createType(it->first, node->getIcon(), node->create(), in, out);
//    }
//    UTILS_ASSERT(item.get<0>() != nullptr);
//    return item;
//}
//
//void TypesWindow::update( const IDataProcessorManager* pm )
//{
//    auto rProcessors = pm->enumPrototypes();
//    for(auto it = rProcessors.begin(); it != rProcessors.end(); ++it ) {
//        QString name = QString::fromStdString((*it)->getName());
//        auto present = name2node.find(name);
//        if (present == name2node.end()) {
//            name2node[name] = *it;
//            addEntry(name, (*it)->getIcon(), IVisualItem::ProcessingNode);
//        }
//    }
//}
//
//void TypesWindow::update( const IDataSourceManager* sm )
//{
//    auto rSources = sm->enumPrototypes();
//    for(auto it = rSources.begin(); it != rSources.end(); ++it ) {
//        QString name = QString::fromStdString((*it)->getName());
//        auto present = name2node.find(name);
//        if (present == name2node.end()) {
//			IWorkflowItemBasePtr base = *it;
//            name2node[name] = *it;
//            addEntry(name, (*it)->getIcon(),IVisualItem::SourceNode);
//        }
//    }
//}
//
//void TypesWindow::update( const IDataSinkManager* sm )
//{
//    auto rSinks = sm->enumPrototypes();
//    for(auto it = rSinks.begin(); it != rSinks.end(); ++it ) {
//        QString name = QString::fromStdString((*it)->getName());
//        auto present = name2node.find(name);
//        if (present == name2node.end()) {
//            name2node[name] = *it;
//            addEntry(name, (*it)->getIcon(), IVisualItem::SinkNode);
//        } 
//    }
//}
//
//core::UniqueID vdf::TypesWindow::getId( const QString& name ) const
//{
//    auto it = name2node.find(name);
//    if (it != name2node.end()) {
//        return it->second->getID();
//    }
//
//    throw std::runtime_error("Node not found");
//}
//
