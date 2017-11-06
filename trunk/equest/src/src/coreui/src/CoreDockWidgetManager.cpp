#include "CoreUiPCH.h"
#include <coreui/CoreDockWidgetSet.h>
#include <coreui/CoreDockWidgetManager.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTabBar>

using namespace coreUI;

CoreDockWidgetManager::CoreDockWidgetManager( QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= 0*/ ) :
	QWidget(parent, flags), tabWidget(new QTabWidget), autoCloseEmptySets_(true),
    maxWidgetsInSetHint(5)
{
	auto layout = new QHBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(tabWidget);
	tabWidget->setParent(this);
	setLayout(layout);
	tabWidget->setMovable(true);
	tabWidget->setTabsClosable(false);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabWidgetChange(int)));
	connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequest(int)));
}

CoreDockWidgetManager::~CoreDockWidgetManager()
{

}

void CoreDockWidgetManager::removeTabsDistance()
{
	if (dockList.size() > 1) {
		//HACK!!
		//TO POWINNO SIĘ DAć STYLOWAć - NIESTETY JEST TO UKRYTE PRZEZ QT
		//TYLKO TAK MOŻNA USUNĄć ODSTĘP POMIĘDZY QTabBar a Widgetem związanym z aktualnym Tabem
		//patrz również: http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbase.html
		// http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbasev2.html#details
		// http://developer.qt.nokia.com/doc/qt-4.8/qstyle.html
		// http://developer.qt.nokia.com/doc/qt-4.8/qtabbar.html
		//

		QList<QTabBar*> tabBars = this->findChildren<QTabBar*>();

		for(int i = 0; i < tabBars.size(); ++i){
			QTabBar* tab = tabBars[i];
			if(tab != nullptr && tab->parent() == this){
				tab->setDrawBase(false);
			}
		}
	}
}

void CoreDockWidgetManager::setAutoCloseEmptySets(const bool val)
{
	autoCloseEmptySets_ = val;
}

const bool CoreDockWidgetManager::autoCloseEmptySets() const
{
	return autoCloseEmptySets_;
}

void CoreDockWidgetManager::onWidgetSetChange(unsigned int number)
{
	if(autoCloseEmptySets_ == true && number == 0){
		auto cdws = qobject_cast<CoreDockWidgetSet*>(sender());
		removeSet(indexOf(cdws));
	}
}

int CoreDockWidgetManager::addDockWidgetSet(CoreDockWidgetSet* set, const QString & label)
{
	auto idx = tabWidget->addTab(set, label);
	innerAddDockWidgetSet(set, idx);
	return idx;
}

int CoreDockWidgetManager::addDockWidgetSet( CoreDockWidgetSet* set, const QIcon & icon, const QString & label)
{
	auto idx = tabWidget->addTab(set, icon, label);
	innerAddDockWidgetSet(set, idx);
	return idx;
}

CoreDockWidgetSet* CoreDockWidgetManager::autoAddDockWidget( QDockWidget* widget, const QIcon & icon, const QString & label )
{
	for (auto it = dockList.begin(); it != dockList.end(); ++it) {
		if ((*it)->isAdditionPossible() == true) {
			(*it)->addDockWidget(widget);
			setCurrentSet(*it);
			return *it;
		}
	}

	CoreDockWidgetSet* set = new CoreDockWidgetSet();
	set->setAttribute(Qt::WA_DeleteOnClose);
    set->setMaxWidgetsNumber(getMaxWidgetsInSetHint());
	set->addDockWidget(widget);
	privateDockList.push_back(set);
	addDockWidgetSet(set, icon, label);
	setCurrentSet(set);
	return set;
}

CoreDockWidgetSet* CoreDockWidgetManager::autoAddDockWidget( QDockWidget* widget, const QString & label )
{
	CoreDockWidgetSet* set = nullptr;
	auto current = currentSet();
	if (current && current->isAdditionPossible()) {
		set = current;
	} else {
	    auto it = std::find_if(dockList.begin(), dockList.end(),
			[](const CoreDockWidgetSet* set) {return set->isAdditionPossible(); });
		set = it != dockList.end() ? *it : nullptr;
	}
	if(set == nullptr){
		set = new CoreDockWidgetSet();
		set->setAttribute(Qt::WA_DeleteOnClose);
        set->setMaxWidgetsNumber(getMaxWidgetsInSetHint());
		privateDockList.push_back(set);
		addDockWidgetSet(set, label);
	}

	set->addDockWidget(widget);

	setCurrentSet(set);
	return set;
}

CoreDockWidgetSet* CoreDockWidgetManager::tryGetDockSet( QDockWidget* widget )
{
    for (auto it = dockList.begin(); it != dockList.end(); ++it) {
        for(const auto dock : (*it)->getDockWidgets()) {
            if (dock == widget) {
                return *it;
            }
        }

    }

    return nullptr;
}

CoreDockWidgetManager::const_range CoreDockWidgetManager::getDockSet() const
{
	return boost::make_iterator_range(dockList.cbegin(), dockList.cend());
}

CoreDockWidgetManager::range CoreDockWidgetManager::getDockSet()
{
	return boost::make_iterator_range(dockList.begin(), dockList.end());
}

int CoreDockWidgetManager::count() const
{
	return tabWidget->count();
}

int	CoreDockWidgetManager::currentIndex() const
{
	return tabWidget->currentIndex();
}

CoreDockWidgetSet * CoreDockWidgetManager::currentSet() const
{
	return qobject_cast<CoreDockWidgetSet*>(tabWidget->currentWidget());
}

CoreDockWidgetSet * CoreDockWidgetManager::set(int index) const
{
	return qobject_cast<CoreDockWidgetSet*>(tabWidget->widget(index));
}

void CoreDockWidgetManager::clear()
{
	tabWidget->clear();
}

int CoreDockWidgetManager::indexOf(CoreDockWidgetSet* set) const
{
	return tabWidget->indexOf(set);
}

QWidget * CoreDockWidgetManager::cornerWidget(Qt::Corner corner) const
{
	return tabWidget->cornerWidget(corner);
}

bool CoreDockWidgetManager::documentMode() const
{
	return tabWidget->documentMode();
}

Qt::TextElideMode CoreDockWidgetManager::elideMode() const
{
	return tabWidget->elideMode();
}

QSize CoreDockWidgetManager::iconSize() const
{
	return tabWidget->iconSize();
}

int CoreDockWidgetManager::insertSet(int index, CoreDockWidgetSet* set, const QString & label)
{
	auto idx = tabWidget->insertTab(index, set, label);
	dockList.push_back(set);
	removeTabsDistance();
	return idx;
}

int CoreDockWidgetManager::insertSet(int index, CoreDockWidgetSet* set, const QIcon & icon, const QString & label)
{
	auto idx = tabWidget->insertTab(index, set, icon, label);
	dockList.push_back(set);
	removeTabsDistance();

	return idx;
}

bool CoreDockWidgetManager::isMovable() const
{
	return tabWidget->isMovable();
}

bool CoreDockWidgetManager::isSetEnabled(int index) const
{
	return tabWidget->isTabEnabled(index);
}

void CoreDockWidgetManager::removeSet(int index)
{
	auto s = set(index);

	if(s->close() == false){
		return;
	}

	dockList.remove(s);
	tabWidget->removeTab(index);
	removeTabsDistance();

	auto it = std::find(privateDockList.begin(), privateDockList.end(), s);
	if(it != privateDockList.end()){
		privateDockList.erase(it);
	}
}

void CoreDockWidgetManager::setCornerWidget(QWidget * widget, Qt::Corner corner)
{
	tabWidget->setCornerWidget(widget, corner);
	removeTabsDistance();
}

void CoreDockWidgetManager::setDocumentMode(bool set)
{
	tabWidget->setDocumentMode(set);
	removeTabsDistance();
}

void CoreDockWidgetManager::setElideMode(Qt::TextElideMode elideMode)
{
	tabWidget->setElideMode(elideMode);
	removeTabsDistance();
}

void CoreDockWidgetManager::setIconSize(const QSize & size)
{
	tabWidget->setIconSize(size);
	removeTabsDistance();
}

void CoreDockWidgetManager::setMovable(bool movable)
{
	tabWidget->setMovable(movable);
}

void CoreDockWidgetManager::setSetEnabled(int index, bool enable)
{
	tabWidget->setTabEnabled(index, enable);
}

void CoreDockWidgetManager::setSetIcon(int index, const QIcon & icon)
{
	tabWidget->setTabIcon(index, icon);
	removeTabsDistance();
}

void CoreDockWidgetManager::setTabPosition(QTabWidget::TabPosition tabPosition)
{
	tabWidget->setTabPosition(tabPosition);
	removeTabsDistance();
}

void CoreDockWidgetManager::setTabShape(QTabWidget::TabShape s)
{
	tabWidget->setTabShape(s);
	removeTabsDistance();
}

void CoreDockWidgetManager::setSetText(int index, const QString & label)
{
	tabWidget->setTabText(index, label);
}

void CoreDockWidgetManager::setSetToolTip(int index, const QString & tip)
{
	tabWidget->setTabToolTip(index, tip);
}

void CoreDockWidgetManager::setSetWhatsThis(int index, const QString & text)
{
	tabWidget->setTabWhatsThis(index, text);
}

void CoreDockWidgetManager::setSetsClosable(bool closeable)
{
	tabWidget->setTabsClosable(closeable);
}

void CoreDockWidgetManager::setUsesScrollButtons(bool useButtons)
{
	tabWidget->setUsesScrollButtons(useButtons);
	removeTabsDistance();
}

QIcon CoreDockWidgetManager::setIcon(int index) const
{
	return tabWidget->tabIcon(index);
}

QTabWidget::TabPosition CoreDockWidgetManager::tabPosition() const
{
	return tabWidget->tabPosition();
}

QTabWidget::TabShape CoreDockWidgetManager::tabShape() const
{
	return tabWidget->tabShape();
}

QString CoreDockWidgetManager::setText(int index) const
{
	return tabWidget->tabText(index);
}

QString CoreDockWidgetManager::setToolTip(int index) const
{
	return tabWidget->tabToolTip(index);
}

QString CoreDockWidgetManager::setWhatsThis(int index) const
{
	return tabWidget->tabWhatsThis(index);
}

bool CoreDockWidgetManager::setsClosable() const
{
	return tabWidget->tabsClosable();
}

bool CoreDockWidgetManager::usesScrollButtons() const
{
	return tabWidget->usesScrollButtons();
}

void CoreDockWidgetManager::setCurrentSet(int idx)
{
	tabWidget->setCurrentIndex(idx);
}

void CoreDockWidgetManager::setCurrentSet(CoreDockWidgetSet * set)
{
	tabWidget->setCurrentWidget(set);
}

void CoreDockWidgetManager::onTabWidgetChange(int idx)
{
	emit currentSetChanged(idx);
}

void CoreDockWidgetManager::onTabCloseRequest(int idx)
{
	emit setCloseRequested(idx);
}

int coreUI::CoreDockWidgetManager::getMaxWidgetsInSetHint() const
{
    return maxWidgetsInSetHint;
}

void coreUI::CoreDockWidgetManager::setMaxWidgetsInSetHint( int val )
{
    maxWidgetsInSetHint = val;
}

void coreUI::CoreDockWidgetManager::innerAddDockWidgetSet( CoreDockWidgetSet* set,
	const int idx)
{
	dockList.push_back(set);

	removeTabsDistance();

	setCurrentSet(idx);

	connect(set, SIGNAL(widgetSetChange(unsigned int)), this, SLOT(onWidgetSetChange(unsigned int)));
}
