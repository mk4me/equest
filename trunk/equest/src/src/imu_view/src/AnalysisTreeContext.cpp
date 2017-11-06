#include "MdePCH.h"
#include "AnalysisTreeContext.h"

#include <QtWidgets/QToolBar>
#include <QtCore/QBuffer>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolButton>
#include <corelib/PluginCommon.h>
#include <coreui/CoreVisualizerWidget.h>
#include "AnalisisWidget.h"


AnalysisTreeContext::AnalysisTreeContext( QTabWidget * flexiTabWidget, coreUI::HierarchyTreeModel* model, AnalysisTreeContextMenu* menuBuilder) :
    flexiTabWidget(flexiTabWidget),
	flexiContent(new QWidget()),
	flexiSection(new coreUI::CoreFlexiToolBarSection()),
	contextWidgetActions(new QWidget()),
    groupID(-1),
    treeModel(model),
    menuBuilder(menuBuilder)
{
	flexiSection->setInnerWidget(flexiContent);
	auto layout_ = new QHBoxLayout;
	layout_->setContentsMargins(0,0,0,0);
	layout_->addWidget(flexiSection);
	layout_->addStretch();
	contextWidgetActions->setLayout(layout_);
	contextWidgetActions->setVisible(false);
}

void AnalysisTreeContext::activateContext( QWidget * contextWidget )
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest ju¿ zaladowany
    if(contextWidget == nullptr || groupID != -1){
        return;
    }

    QAbstractItemView* tree = qobject_cast<QAbstractItemView*>(contextWidget);
    core::IHierarchyItemConstPtr itm = treeModel->internalSmart(tree->currentIndex());
    recreateFlexiSectionWidget(flexiContent, itm);
}

void AnalysisTreeContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

	if(groupID != -1){
		contextWidgetActions->setVisible(false);
		flexiTabWidget->removeTab(groupID);
		groupID = -1;
	}
}

void AnalysisTreeContext::onRegisterContextWidget( QWidget * contextWidget )
{
    QAbstractItemView* tree = qobject_cast<QAbstractItemView*>(contextWidget);
    connect(tree, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onTreeItemActivated(const QModelIndex&)));
}

void AnalysisTreeContext::onUnregisterContextWidget( QWidget * contextWidget )
{

}


void AnalysisTreeContext::recreateFlexiSectionWidget(QWidget* flexiContent, core::IHierarchyItemConstPtr helper)
{
	if (!flexiContent->layout()) {
		flexiContent->setLayout(new QVBoxLayout());		
		flexiContent->layout()->setContentsMargins(0, 0, 0, 0);
		flexiContent->layout()->setSpacing(3);
	}

	const QObjectList& children = flexiContent->children();
	for (int i = children.size() - 1; i >= 0; --i) {
		QWidget* w = qobject_cast<QWidget*>(children[i]);
		if (w) {
			w->setVisible(false);
		}
	}
	
    if (helper) {

		if(groupID == -1){
			groupID = flexiTabWidget->addTab(contextWidgetActions, QObject::tr("Tree"));
			flexiTabWidget->setCurrentIndex(groupID);
		}

		contextWidgetActions->setVisible(true);

        QWidget* container = new QWidget();
        flexiContent->layout()->addWidget(container);
        QLayout* l = new QHBoxLayout();
        container->setLayout(l);

        flexiSection->setName(helper->getName());

        // TODO : wyciek pamiêci !!!
        QMenu* menu = new QMenu("Menu");
        menuBuilder->createMenu(helper, menu);
        auto actions = menu->actions();
        for (auto it = actions.begin(); it != actions.end(); ++it) {
            QToolButton* menuButton = new QToolButton(flexiContent);
            if ((*it)->menu()) {
                menuButton->setMenu((*it)->menu());
            } else {
                menuButton->setDefaultAction(*it);
            }
            menuButton->setPopupMode(QToolButton::InstantPopup);
            menuButton->setText((*it)->text());
            l->addWidget(menuButton);
        }
        
    }else{
		contextWidgetActions->setVisible(false);
	}
}

void AnalysisTreeContext::onTreeItemActivated(const QModelIndex& idx)
{
    recreateFlexiSectionWidget(flexiContent, treeModel->internalSmart(idx));
}

void AnalysisTreeContext::refresh()
{
   QAbstractItemView* tree = qobject_cast<QAbstractItemView*>(getCurrentContextWidget());
   if (tree) {
       core::IHierarchyItemConstPtr itm = treeModel->internalSmart(tree->currentIndex());
       recreateFlexiSectionWidget(flexiContent, itm);
   }
}


