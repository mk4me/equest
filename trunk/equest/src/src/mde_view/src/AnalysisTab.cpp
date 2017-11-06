#include "MdePCH.h"
#include <coreui/CoreTextEditWidget.h>
#include <coreui/CoreFlexiToolBarSection.h>
#include <coreui/CoreFlexiToolBar.h>
#include <QtWidgets/QToolButton>
#include "AnalysisTab.h"

AnalysisTab::AnalysisTab(AnalisisWidget* widget, const QIcon& icon, const QString& label ) :
    AbstractTab(icon, label),
    widget(widget)
{
    connect(widget, SIGNAL(dataAdded()), this, SLOT(dataAdded()));
}


QWidget* AnalysisTab::getMainWidget()
{
    return widget;
}

void AnalysisTab::registerConxtext( coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget * flexiTabWidget )
{
    widget->setContextItems(manager, parent, flexiTabWidget);
}

void AnalysisTab::dataAdded()
{
    setEnabled(true);
}
