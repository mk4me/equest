#include "NewVdfPCH.h"
#include "NewVdfWidget.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QVBoxLayout>
#include <coreui/CoreAction.h>
#include <coreui/CoreWidgetAction.h>
#include "SimpleItem.h"
#include "VdfScene.h"
#include "Command.h"
//#include "VdfView.h"
#include <plugins/newVdf/INodeConfiguration.h>
#include <plugins/newVdf/UniversalSink.h>
#include <coreui/WheelGraphicsView.h>
#include <coreui/HierarchyTreeModel.h>
#include <QtWidgets/QMessageBox>

using namespace vdf;

NewVdfWidget::NewVdfWidget(utils::ICommandStackPtr stack, SceneModelPtr sceneModel, TypesModelPtr typesModel, coreUI::HierarchyTreeModel* treeModel) :
    sceneModel(sceneModel),
	commandStack(stack),
    treeModel(treeModel)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

	stateMachine = SceneStateMachinePtr(new SceneStateMachine(this));
    connect(stateMachine.get(), SIGNAL(singleNodeSelected(IVisualNodePtr)), this, SIGNAL(singleNodeSelected(IVisualNodePtr)));
	scene = new VdfScene(stateMachine, sceneModel, typesModel);
	view = new coreUI::WheelGraphicsView(scene, Qt::ControlModifier);
    view->setAcceptDrops(true);

	//QToolBar* toolbar = new QToolBar("States");
    coreUI::CoreAction*  und = new coreUI::CoreAction(tr("Edit")  , QIcon(":/coreUI/icons/textedit/editundo.png"), tr("Undo"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  red = new coreUI::CoreAction(tr("Edit")  , QIcon(":/coreUI/icons/textedit/editredo.png"), tr("Redo"), this, coreUI::CoreTitleBar::Left);

    coreUI::CoreAction*  mrg = new coreUI::CoreAction(tr("Misc") , QIcon(":/kinematic/icons/skeletal_trace.png"), tr("Merge"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  dmr = new coreUI::CoreAction(tr("Misc") , QIcon(":/kinematic/icons/skeletal_trace.png"), tr("Demerge"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  sav = new coreUI::CoreAction(tr("Misc") , QIcon(":/kinematic/icons/skeletal_trace.png"), tr("Save"), this, coreUI::CoreTitleBar::Left);

    coreUI::CoreAction*  clr = new coreUI::CoreAction(tr("Clear") , QIcon(":/newVdf/icons/rubber.png"), tr("Scene"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  del = new coreUI::CoreAction(tr("Edit"), QIcon(":/coreUI/icons/x.png"), tr("Delete"), this, coreUI::CoreTitleBar::Left);
    coreUI::CoreAction*  run = new coreUI::CoreAction(tr("Run"), QIcon(":/newVdf/icons/run.png"), tr("Run"), this, coreUI::CoreTitleBar::Left);

	//mrg->setEnabled(false);
	
	connect(und, SIGNAL(triggered()), this, SLOT(undo()));
	connect(red, SIGNAL(triggered()), this, SLOT(redo()));
	connect(mrg, SIGNAL(triggered()), this, SLOT(merge()));
    connect(dmr, SIGNAL(triggered()), this, SLOT(demerge()));
    connect(sav, SIGNAL(triggered()), this, SLOT(saveScene()));
	connect(clr, SIGNAL(triggered()), this, SLOT(clearScene()));
	connect(del, SIGNAL(triggered()), this, SLOT(deleteSelected()));
    connect(run, SIGNAL(triggered()), this, SLOT(runDF()));
       
    this->addAction(und);
    this->addAction(red);
    //this->addAction(mrg);
    //this->addAction(dmr);
    //this->addAction(sav);
    this->addAction(del);
    this->addAction(clr);
    this->addAction(run);

    mrg->setEnabled(false);
    dmr->setEnabled(false);
    sav->setEnabled(false);
    	
    layout->addWidget(view);
    setMinimumSize(300, 200);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

NewVdfWidget::~NewVdfWidget()
{
}

void NewVdfWidget::undo()
{
	commandStack->undo();
}

void NewVdfWidget::redo()
{
	commandStack->redo();
}

void NewVdfWidget::demerge()
{

}

void NewVdfWidget::saveScene()
{

}

void NewVdfWidget::merge()
{
	auto node = sceneModel->merge(scene->selectedItems());
	// TODO
	commandStack->addCommand(utils::ICommandPtr(new AddToSceneCommand(sceneModel, node, node.get<0>()->visualItem()->scenePos())));
}

void vdf::NewVdfWidget::deleteSelected()
{
    auto selected = stateMachine->getScene()->selectedItems();
    auto command = utils::ICommandPtr(new RemoveSelectedCommand(stateMachine->getSceneModel(), selected));
    stateMachine->getCommandStack()->addCommand(command);
}

void vdf::NewVdfWidget::clearScene()
{
    if(QMessageBox::question( nullptr, tr("Confirm operation"), tr("Are You sure You want to clear scene?"),
        QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort ) == QMessageBox::Yes) {
        stateMachine->clear();
    	sceneModel->clearScene();
    }
}

void vdf::NewVdfWidget::runDF()
{
	//QMessageBox::critical(this, tr("Costume connection problem"), tr("Could not connect costume - please retry and follow costume initialization instructions."));
	//return;
    
	std::list<UniversalSink*> sinks;
    auto nodes = sceneModel->getVisualItems<IVisualNodePtr>();
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto modelNode = (*it)->getModelNode();
        INodeValidation* valid = dynamic_cast<INodeValidation*>(modelNode);
        if (valid && !valid->isNodeValid()) {
            QMessageBox::critical(this, tr("Error"), QString("Info: %1").arg(valid->getErrorMessage()));
            return;
        }

        UniversalSink* usink = dynamic_cast<UniversalSink*>(modelNode);
        if (usink) {
			usink->disconnect(SIGNAL(itemConsumed(core::IHierarchyItemPtr)));
            connect(usink, SIGNAL(itemConsumed(core::IHierarchyItemPtr)), this, SLOT(onSinkGenerate(core::IHierarchyItemPtr)));
            sinks.push_back(usink);
        }
    }

    static int processingNo = 0;
    try {

        root = core::HierarchyItemPtr(new core::HierarchyItem(QString("Processing %1").arg(++processingNo), QString()));
        sceneModel->run();
       /* for (auto it = sinks.begin(); it != sinks.end(); ++it) {
            root->appendChild((*it)->getDataItem());
        }*/
        treeModel->addRootItem(root);
    } catch (std::exception& e) {
        QMessageBox::critical(this, tr("Error"), QString("Info: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
    }
}

void vdf::NewVdfWidget::onSinkGenerate( core::IHierarchyItemPtr item )
{
    root->appendChild(item);
}
