#include "CoreUiPCH.h"
#include <stdexcept>
#include <QtWidgets/QMenu>
#include <QtGui/QPainter>
#include <loglib/ILog.h>
#include <corelib/PluginCommon.h>
#include <coreui/CoreTitleBar.h>
#include "ui_CoreTitleBar.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include <utils/Push.h>

using namespace coreUI;

CoreTitleBar::CoreTitleBar(QDockWidget* parent, ToggleType toggleType)
	: QWidget(parent), fullScreenSwitch(parent),
	ui(new Ui::CoreTitleBar), leftToolbar(new QToolBar),
	rightToolbar(new QToolBar), verticalOrientation_(false), toggleType_(toggleType)
{
    ui->setupUi(this);

	refreshFeatures(parent->features());
	setWindowTitle(parent->windowTitle());
	// obserwujemy zmiany tytulu okna
	parent->installEventFilter(this);

	//obserwujemy zmiany cech okna
	connect(parent, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)), this, SLOT(refreshFeatures(QDockWidget::DockWidgetFeatures)));
	//zmiana dokowania po stronie dockWidgeta z pominięciem titlebara
	connect(parent, SIGNAL(topLevelChanged(bool)), this, SLOT(onTopLevelChanged(bool)));

	ui->titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	ui->iconPlaceholder->setVisible(false);
	ui->actionDock_Undock->setChecked(!(parent->isFloating()));
	ui->buttonFloat->setDefaultAction(ui->actionDock_Undock);
	ui->buttonClose->setDefaultAction(ui->actionClose);
	ui->buttonFullscreen->setDefaultAction(ui->actionFull_screen_Window);
	leftToolbar->setParent(ui->leftToolbarPlaceholder);
	leftToolbar->setVisible(false);
	rightToolbar->setParent(ui->rightToolbarPlaceholder);
	rightToolbar->setVisible(false);
	ui->leftToolbarPlaceholder->layout()->addWidget(leftToolbar);
	ui->rightToolbarPlaceholder->layout()->addWidget(rightToolbar);

	connect(leftToolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
	connect(rightToolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(onActionTriggered(QAction*)));
	setIconSize(QSize(16,16));
}

CoreTitleBar::~CoreTitleBar()
{

}

void CoreTitleBar::onTopLevelChanged(bool floating)
{
	ui->actionDock_Undock->blockSignals(true);
	ui->actionDock_Undock->setChecked(floating);
	ui->actionDock_Undock->blockSignals(false);
}

void CoreTitleBar::toggleFloating(bool floating)
{
	if(parentWidget() != nullptr){
		QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget());
		if(dockWidget != nullptr) {

			if (dockWidget->isFullScreen() == true){
				//try return from fullscreen
				dockWidget->showNormal();
			}

			dockWidget->setFloating(floating);
		}
	}
}

void CoreTitleBar::closeWindow()
{
	if(parentWidget() != nullptr){
		QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget());
		if(dockWidget != nullptr) {
			dockWidget->close();		
			return;
		}
	}

	this->close();
}

void CoreTitleBar::toggleFullScreen(bool fullScreen)
{
	if (parentWidget() != nullptr){
		QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget());
		if (dockWidget != nullptr) {
			fullScreenSwitch.toggleFullScreen();
		}
	}	
}

void CoreTitleBar::setToggleType(ToggleType toggleType)
{
	toggleType_ = toggleType;
}

CoreTitleBar::ToggleType CoreTitleBar::toggleType() const
{
	return toggleType_;
}

bool CoreTitleBar::eventFilter(QObject * watched, QEvent * event)
{
	if(event->type() == QEvent::WindowTitleChange || event->type() == QEvent::ModifiedChange){
		auto widget = qobject_cast<QWidget *>(watched);
		QString cap = widget->windowTitle();

		if (cap.isEmpty() == false){			

			QLatin1String placeHolder("[*]");
			int placeHolderLength = 3; // QLatin1String doesn't have length()

			int index = cap.indexOf(placeHolder);

			// here the magic begins
			while (index != -1) {
				index += placeHolderLength;
				int count = 1;
				while (cap.indexOf(placeHolder, index) == index) {
					++count;
					index += placeHolderLength;
				}

				if (count%2) { // odd number of [*] -> replace last one
					int lastIndex = cap.lastIndexOf(placeHolder, index - 1);
					if (widget->isWindowModified()
						&& widget->style()->styleHint(QStyle::SH_TitleBar_ModifyNotification, 0, widget))
						cap.replace(lastIndex, 3, QWidget::tr("*"));
					else
						cap.remove(lastIndex, 3);
				}

				index = cap.indexOf(placeHolder, index);
			}

			cap.replace(QLatin1String("[*][*]"), placeHolder);
		}

		setWindowTitle(cap);
		ui->actionToggle_visibility->setText(cap);
		updateTitleOrientation();
	}else if(event->type() == QEvent::WindowIconChange){
		auto widget = qobject_cast<QWidget *>(watched);
		auto icon = widget->windowIcon();
		if(icon.isNull() == true){
			ui->iconPlaceholder->setPixmap(QPixmap());
			ui->iconPlaceholder->setVisible(false);
		}else{
			ui->iconPlaceholder->setPixmap(icon.pixmap(height()));
			ui->iconPlaceholder->setVisible(true);
		}
	}else if(event->type() == QEvent::IconTextChange){
		auto widget = qobject_cast<QWidget *>(watched);
		auto iconText = widget->windowIconText();
		ui->iconPlaceholder->setToolTip(iconText);
	}
	else if (event->type() == QEvent::WindowStateChange){
		//zmiana stanu okna - fullscreen | normal |floating | dock?
		auto widget = qobject_cast<QWidget *>(watched);

		ui->actionFull_screen_Window->blockSignals(true);
		ui->actionFull_screen_Window->setChecked(widget->isFullScreen());
		ui->actionFull_screen_Window->blockSignals(false);
	
		if (widget->isFullScreen() == true){
			ui->actionDock_Undock->blockSignals(true);
			ui->actionDock_Undock->setChecked(true);
			ui->actionDock_Undock->blockSignals(false);
		}	
	}

	return QWidget::eventFilter(watched, event);
}

void CoreTitleBar::changeEvent(QEvent * event)
{
	if(event->type() == QEvent::WindowTitleChange){
		auto t = windowTitle();
		ui->actionToggle_visibility->setText(t);
		updateTitleOrientation();	
	}

	QWidget::changeEvent(event);
}

bool CoreTitleBar::event(QEvent * event)
{
	switch (event->type()) {
	case QEvent::Hide:
		if (!isHidden())
			break;
		// fallthrough intended
	case QEvent::Show:
		ui->actionToggle_visibility->setChecked(event->type() == QEvent::Show);
		emit visibilityChanged(event->type() == QEvent::Show);
		break;
	}

	return QWidget::event(event);
}

void CoreTitleBar::updateTitleOrientation()
{
	QString newTitle(windowTitle());

	if(verticalOrientation_ == true){		
		for(auto i = newTitle.size() - 1; i > 0; --i){
			newTitle.insert(i, "<BR>");
		}

		newTitle = "<html>" + newTitle + "</html>";
	}
	
	ui->titleLabel->setText(newTitle);	
}

void CoreTitleBar::onActionTriggered(QAction * action)
{
	emit actionTriggered(action, actionSide(action));
}

QAction * CoreTitleBar::actionAt(const QPoint & p) const
{
	auto ret = leftToolbar->actionAt(p);
	if(ret == nullptr){
		ret = rightToolbar->actionAt(p);
	}

	return ret;
}

QAction * CoreTitleBar::actionAt(int x, int y) const
{
	auto ret = leftToolbar->actionAt(x,y);
	if(ret == nullptr){
		ret = rightToolbar->actionAt(x,y);
	}

	return ret;
}

void CoreTitleBar::addAction(QAction * action)
{
	SideType side = Left;
	auto a = dynamic_cast<ICoreTitleBarAction*>(action);
	if(a != nullptr){
		side = a->side();
	}

	addAction(action, side);
}

void CoreTitleBar::addAction(QAction * action, SideType side)
{
	switch(side){
	case Left:
		leftToolbar->addAction(action);
		leftToolbar->setVisible(true);
		break;

	case Right:
		rightToolbar->addAction(action);
		rightToolbar->setVisible(true);
		break;

	case Any:
		{
			auto s = Left;
			if (rightToolbar->actions().size() < leftToolbar->actions().size()){
				s = Right;
			}

			addAction(action, s);
			break;
		}
	case Both:
		addAction(action, Left);
		addAction(action, Right);
		break;
	}
}

CoreTitleBar::SideType CoreTitleBar::actionSide(QAction * action) const
{
	SideType side = Left;

	char counter = 0;

	if (leftToolbar->actions().contains(action) == true){
		++counter;
	}
	
	if (rightToolbar->actions().contains(action) == true){
		++counter;
		side = Right;
	}
	
	if (counter == 0){
		throw std::runtime_error("Action not found");
	}

	return ((counter == 1) ? side : Both);
}

QAction * CoreTitleBar::addAction(const QString & text, SideType side)
{
	QAction * ret = nullptr;
	switch (side){
	case Left:
		ret = leftToolbar->addAction(text);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addAction(text);
		rightToolbar->setVisible(true);
		break;
	case Any:
	{
		auto s = Left;
		if (rightToolbar->actions().size() < leftToolbar->actions().size()){
			s = Right;
		}

		ret = addAction(text, s);
		break;
	}
	}

	return ret;
}

QAction * CoreTitleBar::addAction(const QIcon & icon, const QString & text, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addAction(icon, text);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addAction(icon, text);
		rightToolbar->setVisible(true);
		break;
	case Any:
	{
		auto s = Left;
		if (rightToolbar->actions().size() < leftToolbar->actions().size()){
			s = Right;
		}

		ret = addAction(icon, text, s);
		break;
	}
	}

	return ret;
}

QAction * CoreTitleBar::addAction(const QString & text, const QObject * receiver, const char * member, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addAction(text, receiver, member);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addAction(text, receiver, member);
		rightToolbar->setVisible(true);
		break;
	case Any:
	{
		auto s = Left;
		if (rightToolbar->actions().size() < leftToolbar->actions().size()){
			s = Right;
		}

		ret = addAction(text, receiver, member, s);
		break;
	}
	}

	return ret;
}

QAction * CoreTitleBar::addAction(const QIcon & icon, const QString & text, const QObject * receiver, const char * member, SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addAction(icon, text, receiver, member);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addAction(icon, text, receiver, member);
		rightToolbar->setVisible(true);
		break;
	case Any:
	{
		auto s = Left;
		if (rightToolbar->actions().size() < leftToolbar->actions().size()){
			s = Right;
		}

		ret = addAction(icon, text, receiver, member, s);
		break;
	}
	}

	return ret;
}

QAction * CoreTitleBar::addSeparator(SideType side)
{
	QAction * ret = nullptr;
	switch(side){
	case Left:
		ret = leftToolbar->addSeparator();
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addSeparator();
		rightToolbar->setVisible(true);
		break;
	case Any:
	{
		auto s = Left;
		if (rightToolbar->actions().size() < leftToolbar->actions().size()){
			s = Right;
		}

		ret = addSeparator(s);
		break;
	}
	}

	return ret;
}

QAction * CoreTitleBar::addWidget(QWidget * widget, SideType side)
{
	QAction * ret = nullptr;
	switch (side){
	case Left:
		ret = leftToolbar->addWidget(widget);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->addWidget(widget);
		rightToolbar->setVisible(true);
		break;
	case Any:
	{
		auto s = Left;
		if (rightToolbar->actions().size() < leftToolbar->actions().size()){
			s = Right;
		}

		ret = addWidget(widget, s);
		break;
	}
	}

	return ret;
}

void CoreTitleBar::clearActions(SideType side)
{
	switch (side){
	case Left:
		leftToolbar->clear();
		leftToolbar->setVisible(false);
		break;

	case Right:
		rightToolbar->clear();
		rightToolbar->setVisible(false);
		break;

	case Both:
		clearActions(Left);
		clearActions(Right);
		break;
	}
}

void CoreTitleBar::setActionsEnabled(SideType side, bool enable)
{
	switch (side){
	case Left:
		leftToolbar->setEnabled(enable);		
		break;

	case Right:	
		rightToolbar->setEnabled(enable);
		break;

	case Both:
		setActionsEnabled(Left, enable);
		setActionsEnabled(Right, enable);
		break;
	}
}

void CoreTitleBar::setActionsVisible(SideType side, bool visible)
{
	switch (side){
	case Left:
		leftToolbar->setVisible(visible);
		break;

	case Right:
		rightToolbar->setVisible(visible);
		break;

	case Both:
		setActionsVisible(Left, visible);
		setActionsVisible(Right, visible);
		break;
	}
}

QSize CoreTitleBar::iconSize () const
{
	return leftToolbar->iconSize();
}

QAction * CoreTitleBar::insertSeparator(QAction * before)
{
	QAction * ret = nullptr;
	auto s = actionSide(before);
	
	switch(s){
	case Left:
		ret = leftToolbar->insertSeparator(before);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->insertSeparator(before);
		rightToolbar->setVisible(true);
		break;
	case Both:
	{
		//TODO
	}
	}	

	return ret;
}

QAction * CoreTitleBar::insertWidget(QAction * before, QWidget * widget)
{
	QAction * ret = nullptr;
	auto s = actionSide(before);
	switch(s){
	case Left:
		ret = leftToolbar->insertWidget(before, widget);
		leftToolbar->setVisible(true);
		break;

	case Right:
		ret = rightToolbar->insertWidget(before, widget);
		rightToolbar->setVisible(true);
		break;
	case Both:
	{
		//TODO
	}
	}	

	return ret;
}

void CoreTitleBar::toggleVisibility(bool visible)
{
	setVisible(visible);
}

QAction * CoreTitleBar::toggleVisibilityAction()
{
	return ui->actionToggle_visibility;
}

Qt::ToolButtonStyle CoreTitleBar::titlebarButtonStyle() const
{
	return leftToolbar->toolButtonStyle();
}

QWidget * CoreTitleBar::widgetForAction(QAction * action)
{
	QWidget * ret = nullptr;
	auto s = actionSide(action);
	switch (s){
	case Left:
		ret = leftToolbar->widgetForAction(action);
		break;

	case Right:
		ret = rightToolbar->widgetForAction(action);
		break;
	}

	return ret;
}

void CoreTitleBar::setIconSize(const QSize & iconSize)
{
	leftToolbar->setIconSize(iconSize);
	rightToolbar->setIconSize(iconSize);
}

void CoreTitleBar::setTitlebarButtonStyle(Qt::ToolButtonStyle titlebarButtonStyle)
{
	leftToolbar->setToolButtonStyle(titlebarButtonStyle);
	rightToolbar->setToolButtonStyle(titlebarButtonStyle);
}

bool CoreTitleBar::isTitleVisible() const
{
    return ui->titleLabel->isVisible();
}

void CoreTitleBar::setTitleVisible(bool visible)
{
    ui->titleLabel->setVisible(visible);
}

void CoreTitleBar::setToggleDockButtonVisible(bool visible)
{
	ui->buttonFloat->setVisible(visible);
}

void CoreTitleBar::setToggleDockButtonEnabled(bool enable)
{
	ui->buttonFloat->setEnabled(enable);
}

void CoreTitleBar::setToggleFullScreenButtonVisible(bool visible)
{
	ui->buttonFullscreen->setVisible(visible);
}

void CoreTitleBar::setToggleFullScreenButtonEnabled(bool enable)
{
	ui->buttonFullscreen->setEnabled(enable);
}

void CoreTitleBar::setCloseButtonVisible(bool visible)
{
	ui->buttonClose->setVisible(visible);
}

void CoreTitleBar::setCloseButtonEnabled(bool enable)
{
	ui->buttonClose->setEnabled(enable);
}

void CoreTitleBar::refreshFeatures(QDockWidget::DockWidgetFeatures features)
{
	ui->buttonClose->setVisible(features & QDockWidget::DockWidgetClosable);
	ui->buttonFloat->setVisible(features & QDockWidget::DockWidgetFloatable);
	setEnabled(features & QDockWidget::DockWidgetMovable);    

	const bool vertical = (features & QDockWidget::DockWidgetVerticalTitleBar);
	if(vertical != verticalOrientation_) {
		verticalOrientation_ = vertical;

		// będziemy je dodawać do nowego layoutu, stary usuniemy
		QBoxLayout * newLayout = nullptr;
		if(verticalOrientation_ == true){
			// I need to be vertical
			newLayout = new QVBoxLayout();
			ui->titleLabel->setAlignment(Qt::AlignCenter);
			setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		} else {
			// I need to be horizontal
			newLayout = new QHBoxLayout();
			ui->titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		}

		// pobieramy wszystkie widgety - ikonę, tytuł, lewy toolbar, prawy toolbar i button		
		while(layout()->isEmpty() == false){
			auto item = layout()->takeAt(layout()->count()-1);
			if(item->widget() != nullptr){
				newLayout->addWidget(item->widget());
			}else if(item->spacerItem()){
				newLayout->addStretch();
			}else {
				newLayout->addLayout(item->layout());
			}

			delete item;
		}

		delete layout();

		//albo własna implementacja z pionowym textem
		updateTitleOrientation();

		leftToolbar->setOrientation(verticalOrientation_ == true ? Qt::Vertical : Qt::Horizontal);
		rightToolbar->setOrientation(verticalOrientation_ == true ? Qt::Vertical : Qt::Horizontal);

		setLayout(newLayout);
	}
}

CoreTitleBar * CoreTitleBar::supplyWithCoreTitleBar(QDockWidget * dockWidget)
{
    if(dockWidget->titleBarWidget() != nullptr){
        return nullptr;
    }

    auto titleBar = new CoreTitleBar(dockWidget);
	dockWidget->setTitleBarWidget(titleBar);

    return titleBar;
}

void CoreTitleBar::supplyCoreTitleBarWithActions(CoreTitleBar * titleBar, QWidget * widget)
{
	supplyCoreTitleBarWithActions(titleBar, widget->actions());
}

void CoreTitleBar::supplyCoreTitleBarWithActions(CoreTitleBar * titleBar, const QList<QAction *> & actions)
{
	for(auto it = actions.begin(); it != actions.end(); ++it){
		titleBar->addAction(*it);
	}
}

void CoreTitleBar::paintEvent(QPaintEvent * event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CoreTitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (toggleType_ == FullScreen){
		ui->actionFull_screen_Window->toggle();
	}
	else{
		ui->actionDock_Undock->toggle();
	}
}