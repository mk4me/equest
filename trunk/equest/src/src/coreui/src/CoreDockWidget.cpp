#include "CoreUiPCH.h"
#include <coreui/CoreDockWidget.h>
#include <coreui/CoreAction.h>
#include <coreui/CoreTitleBar.h>
#include <utils/Push.h>
#include <QtGui/QtEvents>

using namespace coreUI;

CoreDockWidget::CoreDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(title, parent, flags)//, wasFloating(false), updateFullScreen(true)
{
	setContentsMargins(0,0,0,0);	
}

CoreDockWidget::CoreDockWidget(QWidget * parent, Qt::WindowFlags flags)
	: QDockWidget(parent, flags)//, wasFloating(false), updateFullScreen(true)
{
    setContentsMargins(0,0,0,0);	
}

CoreDockWidget::~CoreDockWidget()
{
	
}

const bool CoreDockWidget::isDockWidgetPermanent(const QDockWidget * dockWidget)
{
	return dockWidget->testAttribute(Qt::WA_DeleteOnClose) == false;
}

void CoreDockWidget::setDockWidgetPermament(QDockWidget * dockWidget, bool permanent)
{
	dockWidget->setAttribute(Qt::WA_DeleteOnClose, !permanent);
}

CoreDockWidget* CoreDockWidget::embeddWidget(QWidget * widget, const QString & windowTitle,
	const Qt::DockWidgetArea allowedAreas, const bool permanent)
{
	std::unique_ptr<CoreDockWidget> embeddedDockWidget(new CoreDockWidget(windowTitle));
	embeddedDockWidget->setWidget(widget);
	embeddedDockWidget->setAllowedAreas(allowedAreas);
	embeddedDockWidget->setPermanent(permanent);

	CoreTitleBar::supplyWithCoreTitleBar(embeddedDockWidget.get());
	return embeddedDockWidget.release();
}

bool CoreDockWidget::isPermanent() const
{
    return isDockWidgetPermanent(this);
}

void CoreDockWidget::setPermanent(const bool permanent)
{
    setDockWidgetPermament(this, permanent);
}

void CoreDockWidget::toggleFloating()
{
    QDockWidget::setFloating( !isFloating() );
}