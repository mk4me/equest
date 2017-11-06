#include <coreui/CoreDockFullScreenSwitch.h>
#include <QtWidgets/QDockWidget>
#include <QtGui/QtEvents>
#include <utils/Push.h>

using namespace coreUI;

CoreDockFullScreenSwitch::CoreDockFullScreenSwitch(QDockWidget * dock, QObject *parent)
	: QObject(parent), dock(dock), wasFloating(dock->isFloating()), update(true)
{
	connect(dock, SIGNAL(topLevelChanged(bool)), this, SLOT(onTopLevelChanged(bool)));
	dock->installEventFilter(this);
}

CoreDockFullScreenSwitch::~CoreDockFullScreenSwitch()
{
	dock->removeEventFilter(this);
	dock = nullptr;
}

void CoreDockFullScreenSwitch::toggleFullScreen()
{
	utils::Push p(update, false);
	if (dock->isFullScreen() == true) {
		dock->showNormal();		
		dock->setFloating(wasFloating);		
	}
	else{
		wasFloating = dock->isFloating();
		if (wasFloating == false){
			dock->setFloating(true);
		}

		dock->showFullScreen();
	}
}

void CoreDockFullScreenSwitch::onTopLevelChanged(bool floating)
{
	if (update == true){
		wasFloating = floating;
	}
}


bool CoreDockFullScreenSwitch::eventFilter(QObject * watched, QEvent * event)
{
	if (update == true && event->type() == QEvent::WindowStateChange){
		auto wsc = dynamic_cast<QWindowStateChangeEvent*>(event);
		const auto wasFullScreen = wsc->oldState() & Qt::WindowFullScreen;
		if (wasFullScreen != dock->isFullScreen()){		
			if (wasFullScreen == true && wasFloating == false){
				utils::Push p(update, false);
				dock->setFloating(false);			
			}
			else{
				wasFloating = true;
			}
		}
	}

	return QObject::eventFilter(watched, event);
}
