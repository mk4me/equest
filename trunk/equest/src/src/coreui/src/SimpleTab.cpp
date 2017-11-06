#include "CoreUiPCH.h"
#include <coreui/SimpleTab.h>
#include <coreui/ContextAutoPlacer.h>
#include <coreui/IAppUsageContextManager.h>
#include <coreui/SimpleContext.h>

SimpleTab::SimpleTab( QWidget* mainWidget, const QIcon& icon, const QString& label ) :
    AbstractTab(icon, label),
    widget(mainWidget)
{
}

SimpleTab::SimpleTab( const QIcon& icon, const QString& label ) :
    AbstractTab(icon, label),
    widget(new QWidget()),
    rootContext(new coreUI::MdeEmptyContext())
{
}


QWidget* SimpleTab::getMainWidget()
{
    return widget;
}

void SimpleTab::registerConxtext( coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget* flexiWidget )
{
    QList<QAction*> actions = widget->actions();
    if (!actions.empty()) {
        rootContext = coreUI::SimpleContextPtr(new coreUI::SimpleContext(flexiWidget, getLabel(), QStringList()));
        manager->addContext(rootContext, parent);
        manager->addWidgetToContext(rootContext, widget);
    } else {
        rootContext = coreUI::MdeEmptyContextPtr(new coreUI::MdeEmptyContext());
    }
}



