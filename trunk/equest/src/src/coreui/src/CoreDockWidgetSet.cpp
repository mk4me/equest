#include "CoreUiPCH.h"
#include <coreui/CoreDockWidgetSet.h>
#include <coreui/CoreDockWidget.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTabBar>
#include <stdexcept>
#include <QtGui/QCloseEvent>

using namespace coreUI;

const int maxWidgets = 5;

CoreDockWidgetSet::CoreDockWidgetSet( const QString &title, QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= 0*/ ) :
	QWidget(parent, flags),
	maxWidgetsNumber(maxWidgets),
	additionPossible(true)
{
	setWindowTitle(title);
	mainWindow = new QMainWindow();
    mainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWindow->setDockOptions(QMainWindow::AllowNestedDocks);
    mainWindow->setContentsMargins(0,0,0,0);
	auto layout = new QHBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(mainWindow);
    setLayout(layout);
}

CoreDockWidgetSet::CoreDockWidgetSet( QWidget *parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/ ) :
	QWidget(parent, flags),
	maxWidgetsNumber(maxWidgets), 
	additionPossible(true)
{
	mainWindow = new QMainWindow();
	mainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWindow->setDockOptions(QMainWindow::AllowNestedDocks);
	mainWindow->setContentsMargins(0,0,0,0);
	auto layout = new QHBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(mainWindow);
	setLayout(layout);
}

CoreDockWidgetSet::~CoreDockWidgetSet()
{

}

bool CoreDockWidgetSet::isAdditionPossible() const
{
	return (additionPossible && (getNumWidgets() < getMaxWidgetsNumber()));
}

void CoreDockWidgetSet::addDockWidget(QDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation)
{
    QWidget::setUpdatesEnabled(false);
    if (isAdditionPossible() == true) {
        this->mainWindow->addDockWidget(area, widget, orientation);
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onDockWidgetClosed(QObject*)));
        connect(widget, SIGNAL(dockLocationChanged ( Qt::DockWidgetArea )), this, SLOT(onDockWidgetLocationChanged(Qt::DockWidgetArea)));
        widgetsList.push_back(widget);
    } else {
        throw loglib::runtime_error("Unable to add widget");
    }
    QWidget::setUpdatesEnabled(true);
    raise();
	emit widgetSetChange(widgetsList.size());
}

void CoreDockWidgetSet::addDockWidget( QDockWidget* widget, Qt::Orientation orientation )
{
	addDockWidget(widget, Qt::TopDockWidgetArea, orientation);
	emit widgetSetChange(widgetsList.size());
}

void CoreDockWidgetSet::addDockWidget( QDockWidget* widget )
{
    addDockWidget(widget, ((getNumWidgets() % 2) == 1) ? Qt::Horizontal : Qt::Vertical);
	emit widgetSetChange(widgetsList.size());
}

void CoreDockWidgetSet::removeDockWidget(QDockWidget * widget)
{
	auto it = std::find(widgetsList.begin(), widgetsList.end(), widget);
	if(it != widgetsList.end()){
		QWidget::setUpdatesEnabled(false);
		this->mainWindow->removeDockWidget(widget);
		disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onDockWidgetClosed(QObject*)));
		disconnect(widget, SIGNAL(dockLocationChanged ( Qt::DockWidgetArea )), this, SLOT(onDockWidgetLocationChanged(Qt::DockWidgetArea)));
		widgetsList.erase(it);
		QWidget::setUpdatesEnabled(true);
		emit widgetSetChange(widgetsList.size());
	}
}

int CoreDockWidgetSet::getNumWidgets() const 
{
	return widgetsList.size();
}

void CoreDockWidgetSet::blockAddition( bool additionPossible )
{
	this->additionPossible = additionPossible;
}

void CoreDockWidgetSet::onDockWidgetClosed( QObject* object )
{ 
    // konwersja wystarczająca, poniewaz chcemy tylko usunąć obiekt z listy
    QDockWidget* widget = reinterpret_cast<QDockWidget*>(object);
    widgetsList.remove(widget);
	emit widgetSetChange(widgetsList.size());
}

void CoreDockWidgetSet::onDockWidgetLocationChanged(Qt::DockWidgetArea area)
{
    //HACK!!
    //TO POWINNO SIĘ DAć STYLOWAć - NIESTETY JEST TO UKRYTE PRZEZ QT
    //TYLKO TAK MOŻNA USUNĄć ODSTĘP POMIĘDZY QTabBar a Widgetem związanym z aktualnym Tabem
    //patrz również: http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbase.html
    // http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbasev2.html#details
    // http://developer.qt.nokia.com/doc/qt-4.8/qstyle.html
    // http://developer.qt.nokia.com/doc/qt-4.8/qtabbar.html
    // 
    QDockWidget * changedDock = qobject_cast<QDockWidget*>(sender());    
    QList<QTabBar*> tabBars = mainWindow->findChildren<QTabBar*>();
    for(int i = 0; i < tabBars.size(); ++i){
        QTabBar* tab = tabBars[i];
        if(tab->parent() == mainWindow){
            tab->setDrawBase(false);
        }
    }

    //odswieżamy tytuł okna - konieczne by style tabów poprawnie działałay (text się dobrze renderował)
    if(changedDock->windowTitle().isEmpty() == false){
        QString originalTitle(changedDock->windowTitle());
        changedDock->setWindowTitle(originalTitle + QString(" "));
        changedDock->setWindowTitle(originalTitle);
    }
}

int CoreDockWidgetSet::getMaxWidgetsNumber() const
{
	return maxWidgetsNumber;
}

void CoreDockWidgetSet::setMaxWidgetsNumber(int val)
{
	maxWidgetsNumber = val;
}

CoreDockWidgetSet::const_range CoreDockWidgetSet::getDockWidgets() const
{
	return boost::make_iterator_range(widgetsList.cbegin(), widgetsList.cend());
}

void CoreDockWidgetSet::closeEvent(QCloseEvent *event)
{
	bool accept = true;
	auto wl = widgetsList;

	blockSignals(true);
	for(auto it = wl.begin(); it != wl.end(); ++it){
		if((*it)->close() == false){
			accept = false;
		}
	}

	if(accept == true){
		event->accept();
	}else{
		blockSignals(false);
		event->ignore();
		emit widgetSetChange(widgetsList.size());
	}	
}