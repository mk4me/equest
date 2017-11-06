#include "CoreUiPCH.h"
#include <coreui/CoreCompoundVisualizerWidget.h>
#include <coreui/CoreVisualizerWidget.h>
#include <QtWidgets/QHBoxLayout>
#include <coreui/CoreWidgetAction.h>
#include <functional>

using namespace coreUI;

Q_DECLARE_METATYPE(CoreVisualizerWidget*);

CoreCompoundVisualizerWidget::CoreCompoundVisualizerWidget(CoreCompoundVisualizerWidget & visualizer) : visualizerSwitch(nullptr), persistentVisualizerSwitch(new QComboBox)
{
	persistentVisualizerSwitch->setVisible(false);
	init();
}

CoreCompoundVisualizerWidget::CoreCompoundVisualizerWidget() : visualizerSwitch(nullptr), persistentVisualizerSwitch(new QComboBox)
{
	persistentVisualizerSwitch->setVisible(false);
}

CoreCompoundVisualizerWidget::~CoreCompoundVisualizerWidget()
{
	delete persistentVisualizerSwitch;
}

void CoreCompoundVisualizerWidget::tryRefreshVisualizerSwitchSettings()
{
	if(visualizerSwitch != nullptr){
		visualizerSwitch->blockSignals(true);
		visualizerSwitch->setDuplicatesEnabled(persistentVisualizerSwitch->duplicatesEnabled());
		visualizerSwitch->setInsertPolicy(persistentVisualizerSwitch->insertPolicy());
		visualizerSwitch->setMaxCount(persistentVisualizerSwitch->maxCount());
		visualizerSwitch->setMaxVisibleItems(persistentVisualizerSwitch->maxVisibleItems());
		visualizerSwitch->setMinimumContentsLength(persistentVisualizerSwitch->minimumContentsLength());
		visualizerSwitch->setIconSize(persistentVisualizerSwitch->iconSize());				
		visualizerSwitch->setSizeAdjustPolicy(persistentVisualizerSwitch->sizeAdjustPolicy());		
		visualizerSwitch->blockSignals(false);
	}
}

void CoreCompoundVisualizerWidget::tryRefreshVisualizerSwitchContent()
{
	if(visualizerSwitch != nullptr){
		visualizerSwitch->blockSignals(true);
		visualizerSwitch->clear();
		for(auto i = 0; i < persistentVisualizerSwitch->count(); ++i)
		{
			auto text = persistentVisualizerSwitch->itemText(i);
			auto icon = persistentVisualizerSwitch->itemIcon(i);
			if(text.isEmpty() == true && icon.isNull() == true){
				visualizerSwitch->insertSeparator(i);
			}else{
				visualizerSwitch->addItem(icon, text);
			}
		}
		visualizerSwitch->setCurrentIndex(persistentVisualizerSwitch->currentIndex());
		visualizerSwitch->setEditText(persistentVisualizerSwitch->currentText());

		visualizerSwitch->blockSignals(false);
	}
}

void CoreCompoundVisualizerWidget::tryRefreshVisualizerSwitch()
{
	tryRefreshVisualizerSwitchSettings();
	tryRefreshVisualizerSwitchContent();
}

void CoreCompoundVisualizerWidget::switchDestroyed(QObject * visSwitch)
{
	visualizerSwitch = nullptr;
}

QWidget * CoreCompoundVisualizerWidget::createVisualizerSwitch(QWidget * parent)
{
	if(visualizerSwitch != nullptr){
		throw loglib::runtime_error("Visualizer switch already exists");
	}

	visualizerSwitch = new QComboBox(parent);	
	connect(visualizerSwitch, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentIndex(int)));
	connect(visualizerSwitch, SIGNAL(destroyed(QObject*)), this, SLOT(switchDestroyed(QObject*)));
	tryRefreshVisualizerSwitch();
	
	return visualizerSwitch;
}

void CoreCompoundVisualizerWidget::init()
{
	persistentVisualizerSwitch->setEditText(tr("No visualizer"));
	setLayout(new QHBoxLayout);
	CoreCustomWidgetAction * switchVisualizerAction = new CoreCustomWidgetAction(
		this,
		CoreCustomWidgetAction::WidgetCreator(std::bind(&CoreCompoundVisualizerWidget::createVisualizerSwitch, this, std::placeholders::_1)),
		tr("Utils"),
		CoreTitleBar::Left
	);

	addAction(switchVisualizerAction);
}

void CoreCompoundVisualizerWidget::addVisualizer(core::VisualizerPtr visualizer, const QString & visualizerName)
{
	CoreVisualizerWidget * visualizerWidget = new CoreVisualizerWidget(visualizer, this);

	QVariant data;
	data.setValue(visualizerWidget);
	persistentVisualizerSwitch->addItem(visualizer->getIcon(), visualizerName, data);

	layout()->addWidget(visualizerWidget);

	tryRefreshVisualizerSwitchContent();
}

void CoreCompoundVisualizerWidget::addVisualizer(const QIcon & icon, core::VisualizerPtr visualizer, const QString & visualizerName)
{
	CoreVisualizerWidget * visualizerWidget = new CoreVisualizerWidget(visualizer, this);
	
	QVariant data;
	data.setValue(visualizerWidget);
	persistentVisualizerSwitch->addItem(icon, visualizerName, data);	

	layout()->addWidget(visualizerWidget);

	tryRefreshVisualizerSwitchContent();
}

int CoreCompoundVisualizerWidget::count() const
{
	return persistentVisualizerSwitch->count();
}

int CoreCompoundVisualizerWidget::currentIndex() const
{
	return persistentVisualizerSwitch->currentIndex();
}

core::VisualizerPtr CoreCompoundVisualizerWidget::currentVisualizer() const
{
	CoreVisualizerWidget * currentVisWidget = currentVisualizerWidget();

	if(currentVisWidget == nullptr){
		return core::VisualizerPtr();
	}

	return currentVisWidget->getVisualizer();
}

CoreVisualizerWidget * CoreCompoundVisualizerWidget::currentVisualizerWidget() const
{
	if(persistentVisualizerSwitch->count() == 0){
		return nullptr;
	}

	return persistentVisualizerSwitch->itemData(visualizerSwitch->currentIndex()).value<CoreVisualizerWidget*>();
}

bool CoreCompoundVisualizerWidget::nameDuplicatesEnabled() const
{
	return persistentVisualizerSwitch->duplicatesEnabled();
}

int CoreCompoundVisualizerWidget::findVisualizer(core::VisualizerPtr visualizer) const
{
	int ret = -1;

	for(int i = 0; i < persistentVisualizerSwitch->count(); ++i){
		if(persistentVisualizerSwitch->itemData(i).value<CoreVisualizerWidget*>()->getVisualizer() == visualizer){
			ret = i;
			break;
		}
	}

	return ret;
}

int CoreCompoundVisualizerWidget::findVisualizer(const QString & visualizerName) const
{
	return persistentVisualizerSwitch->findText(visualizerName);
}

void CoreCompoundVisualizerWidget::insertVisualizer(int index, core::VisualizerPtr visualizer, const QString & visualizerName)
{
	CoreVisualizerWidget * visualizerWidget = new CoreVisualizerWidget(visualizer, this);

	QVariant data;
	data.setValue(visualizerWidget);
	persistentVisualizerSwitch->insertItem(index, visualizer->getIcon(), visualizerName, data);

	layout()->addWidget(visualizerWidget);

	tryRefreshVisualizerSwitchContent();
}

void CoreCompoundVisualizerWidget::insertVisualizer(int index, const QIcon & icon, core::VisualizerPtr visualizer, const QString & visualizerName)
{
	CoreVisualizerWidget * visualizerWidget = new CoreVisualizerWidget(visualizer, this);	

	QVariant data;
	data.setValue(visualizerWidget);
	persistentVisualizerSwitch->insertItem(index, icon, visualizerName, data);

	layout()->addWidget(visualizerWidget);

	tryRefreshVisualizerSwitchContent();
}

QComboBox::InsertPolicy CoreCompoundVisualizerWidget::insertPolicy() const
{
	return persistentVisualizerSwitch->insertPolicy();
}

void CoreCompoundVisualizerWidget::insertSeparator(int index)
{
	persistentVisualizerSwitch->insertSeparator(index);
}

QIcon CoreCompoundVisualizerWidget::visualizerIcon(int index) const
{
	return persistentVisualizerSwitch->itemIcon(index);
}

QString CoreCompoundVisualizerWidget::visualizerText(int index) const
{
	return persistentVisualizerSwitch->itemText(index);
}

int CoreCompoundVisualizerWidget::maxCount() const
{
	return persistentVisualizerSwitch->maxCount();
}

int CoreCompoundVisualizerWidget::maxVisibleItems() const
{
	return persistentVisualizerSwitch->maxVisibleItems();
}

int CoreCompoundVisualizerWidget::minimumContentsLength() const
{
	return persistentVisualizerSwitch->minimumContentsLength();
}

void CoreCompoundVisualizerWidget::removeVisualizer(int index)
{
	CoreVisualizerWidget * visWidget = persistentVisualizerSwitch->itemData(index).value<CoreVisualizerWidget *>();

	if(index == currentIndex()){
		visWidget->setVisible(false);
		persistentVisualizerSwitch->removeItem(index);
		innerShowCurrentIndex(currentIndex());
	}

	layout()->removeWidget(visWidget);
	delete visWidget;

	tryRefreshVisualizerSwitchContent();
}

void CoreCompoundVisualizerWidget::setNameDuplicatesEnabled(bool enable)
{
	persistentVisualizerSwitch->setDuplicatesEnabled(enable);
	tryRefreshVisualizerSwitchSettings();
}

void CoreCompoundVisualizerWidget::setIconSize(const QSize & size)
{
	persistentVisualizerSwitch->setIconSize(size);
	tryRefreshVisualizerSwitchSettings();
}

void CoreCompoundVisualizerWidget::setInsertPolicy(QComboBox::InsertPolicy policy)
{
	persistentVisualizerSwitch->setInsertPolicy(policy);
	tryRefreshVisualizerSwitchSettings();
}

void CoreCompoundVisualizerWidget::setVisualizerIcon(int index, const QIcon & icon)
{
	persistentVisualizerSwitch->setItemIcon(index, icon);
	tryRefreshVisualizerSwitchContent();
}

void CoreCompoundVisualizerWidget::setVisualizerText(int index, const QString & text)
{
	persistentVisualizerSwitch->setItemText(index, text);
	tryRefreshVisualizerSwitchContent();
}

void CoreCompoundVisualizerWidget::setMaxCount(int max)
{
	persistentVisualizerSwitch->setMaxCount(max);
	tryRefreshVisualizerSwitchSettings();
}

void CoreCompoundVisualizerWidget::setMaxVisibleVisualizers(int maxItems)
{
	persistentVisualizerSwitch->setMaxVisibleItems(maxItems);
	tryRefreshVisualizerSwitchSettings();
}

void CoreCompoundVisualizerWidget::setMinimumContentsLength(int characters)
{
	persistentVisualizerSwitch->setMinimumContentsLength(characters);
	tryRefreshVisualizerSwitchSettings();
}

void CoreCompoundVisualizerWidget::setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy policy)
{
	persistentVisualizerSwitch->setSizeAdjustPolicy(policy);
	tryRefreshVisualizerSwitchSettings();
}

QComboBox::SizeAdjustPolicy CoreCompoundVisualizerWidget::sizeAdjustPolicy() const
{
	return persistentVisualizerSwitch->sizeAdjustPolicy();
}

void CoreCompoundVisualizerWidget::clear()
{
	persistentVisualizerSwitch->clear();
	tryRefreshVisualizerSwitchContent();
}

void CoreCompoundVisualizerWidget::setCurrentIndex(int index)
{
	CoreVisualizerWidget * currentVisWidget = currentVisualizerWidget();

	if(currentVisWidget != nullptr){
		currentVisWidget->setVisible(false);
	}

	persistentVisualizerSwitch->blockSignals(true);
	persistentVisualizerSwitch->setCurrentIndex(index);
	persistentVisualizerSwitch->blockSignals(false);

	if(visualizerSwitch != nullptr){
		visualizerSwitch->blockSignals(true);
		visualizerSwitch->setCurrentIndex(index);
		visualizerSwitch->blockSignals(false);
	}

	innerShowCurrentIndex(index);
}

void CoreCompoundVisualizerWidget::innerShowCurrentIndex(int idx)
{
	if(idx == -1){
		persistentVisualizerSwitch->setEditText(tr("No visualizers"));
		if(visualizerSwitch != nullptr){
			visualizerSwitch->blockSignals(true);
			visualizerSwitch->setEditText(tr("No visualizers"));
			visualizerSwitch->blockSignals(false);
		}
	}else{
		persistentVisualizerSwitch->itemData(idx).value<CoreVisualizerWidget *>()->setVisible(true);	
	}

	emit currentIndexChanged(idx);
}