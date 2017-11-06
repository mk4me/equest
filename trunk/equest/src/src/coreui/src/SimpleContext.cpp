#include "CoreUiPCH.h"
#include <coreui/SimpleContext.h>
#include <QtWidgets/QTabWidget>
#include <coreui/ContextAutoPlacer.h>
#include <coreui/CoreFlexiToolBarSection.h>


coreUI::SimpleContext::SimpleContext( QTabWidget * flexiTabWidget, const QString& name, const QStringList& sectionsToSkip  ) :
flexiTabWidget(flexiTabWidget),
    sectionsToSkip(sectionsToSkip),
    groupID(-1),
    widget(nullptr),
    name(name)
{

}


void coreUI::SimpleContext::activateContext( QWidget * contextWidget )
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest ju¿ zaladowany
    if(contextWidget == nullptr || groupID != -1){
        return;
    }

    groupID = flexiTabWidget->addTab(widget, name);
    widget->setVisible(true);
    flexiTabWidget->setCurrentIndex(groupID);
}

void coreUI::SimpleContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

    if(groupID != -1){
        widget->setVisible(false);
        flexiTabWidget->removeTab(groupID);
        groupID = -1;
    }
}

void coreUI::SimpleContext::onRegisterContextWidget( QWidget * contextWidget )
{
    QList<QAction*> actions = contextWidget->actions();
    if (!actions.empty()) {
        QWidget* actionsWidget = coreUI::ContextAutoPlacer::create(actions, sectionsToSkip);
        if (!widget) {
            widget = new coreUI::CoreFlexiToolBar(flexiTabWidget);
        }

        auto section = new coreUI::CoreFlexiToolBarSection(QObject::tr("Operations"));
        section->setInnerWidget(actionsWidget);
        widget->addSection(section);
        widget->setVisible(false);
    }
}

void coreUI::SimpleContext::onUnregisterContextWidget( QWidget * contextWidget )
{
}
