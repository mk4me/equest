#include "MdePCH.h"
#include "MdeServiceWindow.h"
#include <QtCore/QSettings>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QMenu>
#include <coreui/CoreWidgetAction.h>

MdeServiceWindow::MdeServiceWindow( const QString& idName ) :
    id(idName)
{
    loadLayout();

    QToolButton * viewMenuButton = new QToolButton;
    //viewMenuButton->setIcon(QIcon(QString::fromUtf8(":/kinematic/icons/viewa.png")));
    viewMenuButton->setText(tr("Properties"));
    viewMenuButton->setPopupMode(QToolButton::MenuButtonPopup);	
    viewMenu = new QMenu(tr("Properties"), viewMenuButton);    
    
    viewMenuButton->setMenu(viewMenu);

    coreUI::CoreWidgetAction * viewAction = new coreUI::CoreWidgetAction(this, tr("Properties"), coreUI::CoreTitleBar::Right);
    viewAction->setDefaultWidget(viewMenuButton);
    addAction(viewAction);
}

MdeServiceWindow::~MdeServiceWindow()
{
    saveLayout();
}

void MdeServiceWindow::saveLayout()
{
    QSettings settings("PJATK", "IMU");

    settings.beginGroup(id);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void MdeServiceWindow::loadLayout()
{
    QSettings settings("PJATK", "IMU");

    settings.beginGroup(id);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void MdeServiceWindow::createMdeDock( QString serviceName, QWidget* widget )
{
    auto it = serviceCount.find(serviceName);
    if (it == serviceCount.end()) {
        serviceCount[serviceName] = 0;
    }
    coreUI::CoreDockWidget* dw = dockWrap(widget, widget->objectName(), serviceName, serviceCount[serviceName]++);
    
    if (!restoreDockWidget(dw)) {
		static bool left = false;
		addDockWidget(left ? Qt::RightDockWidgetArea : Qt::LeftDockWidgetArea, dw);
		left = !left;
    }
    viewMenu->addAction(dw->toggleViewAction());
}

coreUI::CoreDockWidget* MdeServiceWindow::dockWrap(QWidget* widget, const QString& dockTitle, const QString& serviceName, int propNo )
{
    QString dockName = QString("%1_%2_%3").arg(serviceName).arg(dockTitle).arg(propNo);
    coreUI::CoreDockWidget* dock = new coreUI::CoreDockWidget(dockTitle);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setWidget(widget);	
    dock->setObjectName(dockName);
    return dock;
}

