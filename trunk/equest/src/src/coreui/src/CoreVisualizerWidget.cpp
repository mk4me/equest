#include "CoreUiPCH.h"
#include <coreui/CoreVisualizerWidget.h>

#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolButton>
#include <iterator>

#include <corelib/DataAccessors.h>
#include <coreui/CoreMainWindow.h>
#include <corelib/DataAccessors.h>
#include <coreui/CoreTitleBar.h>
#include <coreui/CoreWidgetAction.h>
#include <coreui/CoreAction.h>
#include <functional>

using namespace coreUI;

struct FutureSerieData {
	std::string serieName;
	utils::TypeInfo requestedType;
	core::VariantConstPtr data;
	int localIdx;
};

Q_DECLARE_METATYPE(FutureSerieData);
Q_DECLARE_METATYPE(core::Visualizer::Serie*);

QWidget * CoreVisualizerWidget::createSourceMenuWidget(QWidget * parent)
{
	QToolButton * dataSelectionButton = new QToolButton(parent);
	dataSelectionButton->setText(tr("Data"));
	dataSelectionButton->setIcon(QIcon(QString::fromUtf8(":/coreUI/icons/dane.wejsciowe.png")));
	dataSelectionButton->setPopupMode(QToolButton::MenuButtonPopup);
	//wybór danych
	QMenu * dataSelectioMenu = new QMenu(tr("Data"), dataSelectionButton);
	dataSelectioMenu->setIcon(QIcon(QString::fromUtf8(":/coreUI/icons/dane.wejsciowe.png")));
	connect(dataSelectioMenu, SIGNAL(aboutToShow()), this, SLOT(fillSourcesMenu()));
	connect(dataSelectioMenu, SIGNAL(aboutToHide()), this, SLOT(clearSourcesMenu()));
	dataSelectionButton->setMenu(dataSelectioMenu);
	
	return dataSelectionButton;
}

QWidget * CoreVisualizerWidget::createActiveSerieSwitch(QWidget * parent)
{
	QComboBox * activeSerieSwitch = new QComboBox(parent);
	connect(activeSerieSwitch, SIGNAL(currentIndexChanged(int)), this, SLOT(serieSelected(int)));
	connect(activeSerieSwitch, SIGNAL(destroyed(QObject*)), this, SLOT(activeSerieSwitchDestroyed(QObject*)));

	refreshActiveSerieSwitch(activeSerieSwitch);
	activeSerieSwitches.push_back(activeSerieSwitch);
	return activeSerieSwitch;
}

void CoreVisualizerWidget::refreshActiveSerieSwitchSettings(QComboBox * activeSerieSwitch)
{	
	activeSerieSwitch->blockSignals(true);
	activeSerieSwitch->setDuplicatesEnabled(persistentActiveSerieSwitch->duplicatesEnabled());
	activeSerieSwitch->setInsertPolicy(persistentActiveSerieSwitch->insertPolicy());
	activeSerieSwitch->setMaxCount(persistentActiveSerieSwitch->maxCount());
	activeSerieSwitch->setMaxVisibleItems(persistentActiveSerieSwitch->maxVisibleItems());
	activeSerieSwitch->setMinimumContentsLength(persistentActiveSerieSwitch->minimumContentsLength());
	activeSerieSwitch->setIconSize(persistentActiveSerieSwitch->iconSize());				
	activeSerieSwitch->setSizeAdjustPolicy(persistentActiveSerieSwitch->sizeAdjustPolicy());		
	activeSerieSwitch->blockSignals(false);	
}

void CoreVisualizerWidget::refreshActiveSerieSwitchContent(QComboBox * activeSerieSwitch)
{	
	activeSerieSwitch->blockSignals(true);
	activeSerieSwitch->clear();
	for(auto i = 0; i < persistentActiveSerieSwitch->count(); ++i)
	{
		auto text = persistentActiveSerieSwitch->itemText(i);
		auto icon = persistentActiveSerieSwitch->itemIcon(i);
		if(text.isEmpty() == true && icon.isNull() == true){
			activeSerieSwitch->insertSeparator(i);
		}else{
			activeSerieSwitch->addItem(icon, text);
		}
	}
	activeSerieSwitch->setCurrentIndex(persistentActiveSerieSwitch->currentIndex());
	activeSerieSwitch->setEditText(persistentActiveSerieSwitch->currentText());

	if(persistentActiveSerieSwitch->count() > 1){
		activeSerieSwitch->setEnabled(true);
	}else{
		activeSerieSwitch->setEnabled(false);
	}

	activeSerieSwitch->blockSignals(false);	
}

void CoreVisualizerWidget::refreshActiveSerieSwitch(QComboBox * activeSerieSwitch)
{
	refreshActiveSerieSwitchSettings(activeSerieSwitch);
	refreshActiveSerieSwitchContent(activeSerieSwitch);
}

void CoreVisualizerWidget::tryRefreshActiveSerieSwitchesSettings()
{
	for(auto it = activeSerieSwitches.begin(); it != activeSerieSwitches.end(); ++it){
		refreshActiveSerieSwitchSettings(*it);
	}
}

void CoreVisualizerWidget::tryRefreshActiveSerieSwitchesContent()
{
	for(auto it = activeSerieSwitches.begin(); it != activeSerieSwitches.end(); ++it){
		refreshActiveSerieSwitchContent(*it);
	}
}

void CoreVisualizerWidget::tryRefreshActiveSerieSwitches()
{
	tryRefreshActiveSerieSwitchesSettings();
	tryRefreshActiveSerieSwitchesContent();
}

void CoreVisualizerWidget::activeSerieSwitchDestroyed(QObject * activeSerieSwitch)
{
	activeSerieSwitches.remove(dynamic_cast<QComboBox*>(activeSerieSwitch));
}

//! Zerujący konstruktor.
//! \param parent
//! \param flags
CoreVisualizerWidget::CoreVisualizerWidget(core::VisualizerPtr visualizer, QWidget* parent) : QScrollArea(parent),
	visualizer_(visualizer), persistentActiveSerieSwitch(nullptr)
{
	auto visWidget = visualizer->getOrCreateWidget();

	if(visWidget == nullptr){
		throw loglib::runtime_error("nullptr instead of QWidget obtained from Visualizer");
	}
	visualizer_->getSupportedTypes(supportedDataTypes);

	auto layout = new QHBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);
	layout->addWidget(visWidget);

	CoreCustomWidgetAction * dataSelectAction = new CoreCustomWidgetAction(
		this,
		CoreCustomWidgetAction::WidgetCreator(std::bind(&CoreVisualizerWidget::createSourceMenuWidget, this, std::placeholders::_1)),
		tr("Settings"),
		CoreTitleBar::Left
	);
	
	addAction(dataSelectAction);

	//ustawianie aktualnej serii danych
	if(visualizer_->getMaxSeries() != 1){
		persistentActiveSerieSwitch = new QComboBox(visWidget);
		persistentActiveSerieSwitch->setVisible(false);
		persistentActiveSerieSwitch->addItem(tr("No active session"));		
		CoreCustomWidgetAction * activeDataSelectAction = new CoreCustomWidgetAction(
			this,
			CoreCustomWidgetAction::WidgetCreator(std::bind(&CoreVisualizerWidget::createActiveSerieSwitch, this, std::placeholders::_1)),
			tr("Settings"),
			CoreTitleBar::Left
		);
		addAction(activeDataSelectAction);
	}

	//screenshot
	CoreAction * screenshotAction = new CoreAction(tr("Utils"), tr("Screenshot"), this, CoreTitleBar::Right);
	screenshotAction->setIcon(QIcon(QString::fromUtf8(":coreUI/icons/screenshot-b.png")));
	connect(screenshotAction, SIGNAL(triggered()), visualizer.get(), SLOT(onScreenshotTrigger()));
	addAction(screenshotAction);

	//akcja czyszcząca wszystkie dane wizualizatora
	dataDeselectAll = new QAction(tr("Remove all"), this);
	connect(dataDeselectAll, SIGNAL(triggered()), this, SLOT(removeAllSeries()));

	//! Akcja odpowiedzialna za przełanczanie pomiędzy aktywnym obserwowaniem dataManagera w przyapdku zmian danych związanych z seriami danych
	//CoreAction * liveObserveDataAction = new CoreAction(tr("Utils"), QIcon(), tr("Live data update"), this, CoreTitleBar::Right);
	//liveObserveDataAction->setCheckable(true);
	//liveObserveDataAction->setChecked(true);
	//connect(liveObserveDataAction, SIGNAL(triggered(bool)), this, SLOT(onLiveObserveChange(bool)));

	//addAction(liveObserveDataAction);
}

CoreVisualizerWidget::CoreVisualizerWidget(const CoreVisualizerWidget & visualizer)
{
	//TODO
}

//! Zapewnia możliwość kasowanie widgeta wizualizatora przez jego implementację.
CoreVisualizerWidget::~CoreVisualizerWidget()
{
	
}

core::VisualizerPtr CoreVisualizerWidget::getVisualizer()
{
	return visualizer_;
}

void CoreVisualizerWidget::fillSourcesMenu()
{
	const static QString dataActionName(tr("(Source: %1) Name: %2"));
	const static QString localDataName(tr("Local data name %1"));
	const static QString unknownDataSource(tr("Unknown"));

	QMenu * menu = qobject_cast<QMenu*>(sender());

	auto allDataSubmenu =  menu->addMenu(tr("All data"));
	auto activeDataSubmenu = menu->addMenu(tr("Active data"));
	menu->addSeparator();
	menu->addAction(dataDeselectAll);

	if(activeData.empty() == true){
		dataDeselectAll->setEnabled(false);
		activeDataSubmenu->setEnabled(false);
	}else{
		dataDeselectAll->setEnabled(true);
		//TODO
		//oznaczyć serie które są w wizualizatorze ale nie ma ich już w samych danych wizualizatora!!

		//najpierw serie aktywne		
		for(auto typeIT = activeData.begin(); typeIT != activeData.end(); ++typeIT){
			auto typeMenu = activeDataSubmenu->addMenu(QString::fromUtf8(typeIT->first.name()));
			for(auto dataIT = typeIT->second.begin(); dataIT != typeIT->second.end(); ++dataIT){
				std::string dataSource = unknownDataSource.toStdString();
				getDataSource(dataIT->first, dataSource);
				
				auto action = typeMenu->addAction(dataActionName.arg(QString::fromStdString(dataSource)).arg(QString::fromStdString(dataIT->second->innerSerie()->getName())));
				action->setCheckable(true);
				action->setChecked(true);
				QVariant data;
				data.setValue(dataIT->second);
				action->setData(data);
				connect(action, SIGNAL(triggered()), this, SLOT(removeSerie()));
			}
		}		
	}

	//teraz wszystkie dane
	//lokalny index danych jeśli nie mają nazwy
	int localDataIndex = 0;

	//TODO
	//oznaczyć serie które są w wizualizatorze ale nie ma ich już w samych danych wizualizatora!!

	bool dataExists = false;

	//wszystkie dane wg typów jakie obsługuje wizualizator
	for(auto typeIT = supportedDataTypes.begin(); typeIT != supportedDataTypes.end(); ++typeIT){
		core::ConstVariantsList data;
		visualizer_->getData(*typeIT, data, false);
		if(data.empty() == false){

			dataExists = true;

			auto activeTypeIT = activeData.find(*typeIT);
			auto subDataManu = allDataSubmenu->addMenu(QString::fromUtf8((*typeIT).name()));
			for(auto dataIT = data.begin(); dataIT != data.end(); ++dataIT){
				std::string dataSource = unknownDataSource.toStdString();
				getDataSource(*dataIT, dataSource);

				int serieLocIndex = -1;

				std::string dataName;
				if(getDataName(*dataIT, dataName) == false){
					serieLocIndex = localDataIndex = nextValidLocalSerieIdx(localDataIndex);
					dataName = localDataName.arg(localDataIndex++).toStdString();
				}

				auto action = subDataManu->addAction(dataActionName.arg(QString::fromStdString(dataSource)).arg(QString::fromStdString(dataName)));
				action->setCheckable(true);
				
				bool active = false;
				if(activeTypeIT != activeData.end()){
					auto activeDataIT = activeTypeIT->second.find(*dataIT);
					if(activeDataIT != activeTypeIT->second.end()){
						active = true;
						action->setChecked(true);
						QVariant data;
						data.setValue(activeDataIT->second);
						action->setData(data);
						connect(action, SIGNAL(triggered()), this, SLOT(removeSerie()));
					}
				}

				if(active == false){
					FutureSerieData serieData;
					serieData.data = *dataIT;
					serieData.requestedType = *typeIT;
					serieData.serieName = dataName;
					serieData.localIdx = serieLocIndex;
					action->setChecked(false);
					QVariant data;
					data.setValue(serieData);
					action->setData(data);
					connect(action, SIGNAL(triggered()), this, SLOT(addSerie()));
				}
			}
		}
	}

	//jesli mamy juz max serii danych
	if(dataExists == false || visualizer_->getNumSeries() == visualizer_->getMaxSeries()){
		allDataSubmenu->setEnabled(false);
	}
}

const int CoreVisualizerWidget::nextValidLocalSerieIdx(int startIdx) const
{
	if(usedLocalNameIndexes.empty() == true || startIdx < (*usedLocalNameIndexes.begin()) || startIdx > (*usedLocalNameIndexes.rbegin())){
		return startIdx;
	}

	while(usedLocalNameIndexes.find(startIdx) != usedLocalNameIndexes.end()) { ++startIdx; }

	return startIdx;
}

void CoreVisualizerWidget::clearSourcesMenu()
{
	QMenu * menu = qobject_cast<QMenu*>(sender());
	menu->blockSignals(true);
	menu->clear();
	menu->blockSignals(false);
}

void CoreVisualizerWidget::removeAllSeries()
{	
	visualizer_->setActiveSerie(nullptr);
	if(persistentActiveSerieSwitch != nullptr){
		persistentActiveSerieSwitch->blockSignals(true);
		persistentActiveSerieSwitch->clear();
		persistentActiveSerieSwitch->addItem(tr("No active session"));
		persistentActiveSerieSwitch->blockSignals(false);
		tryRefreshActiveSerieSwitchesContent();
	}
	std::set<int>().swap(usedLocalNameIndexes);
	std::map<utils::TypeInfo, std::map<core::VariantConstPtr, core::Visualizer::Serie*>>().swap(activeData);
	std::map<core::Visualizer::Serie*, int>().swap(serieLocalIdx);
	visualizer_->destroyAllSeries();
}

void CoreVisualizerWidget::serieSelected(int idx)
{
	if(idx == 0){
		visualizer_->setActiveSerie(nullptr);
	}else{
		visualizer_->setActiveSerie(persistentActiveSerieSwitch->itemData(idx).value<core::Visualizer::Serie*>());
	}
}

void CoreVisualizerWidget::addSerie()
{
	QAction * action = qobject_cast<QAction*>(sender());
	FutureSerieData data = action->data().value<FutureSerieData>();
	auto serie = visualizer_->createSerie(data.requestedType, data.data);
	serie->innerSerie()->setName(data.serieName);
	activeData[data.requestedType].insert(std::map<core::VariantConstPtr, core::Visualizer::Serie*>::value_type(data.data, serie));
	if(data.localIdx > -1){
		serieLocalIdx[serie] = data.localIdx;
		usedLocalNameIndexes.insert(data.localIdx);
	}

	if(persistentActiveSerieSwitch != nullptr){
		QVariant serieData;
		serieData.setValue(serie);
		persistentActiveSerieSwitch->addItem(QString::fromStdString(data.serieName), serieData);
		if(persistentActiveSerieSwitch->count() == 2){
			persistentActiveSerieSwitch->setCurrentIndex(1);
		}

		tryRefreshActiveSerieSwitchesContent();
	}else{
		visualizer_->setActiveSerie(serie);
	}
}

void CoreVisualizerWidget::removeSerie()
{
	QAction * action = qobject_cast<QAction*>(sender());
	core::Visualizer::Serie * serie = action->data().value<core::Visualizer::Serie *>();

	auto activeDataIT = activeData.find(serie->innerSerie()->getRequestedDataType());

	activeDataIT->second.erase(serie->innerSerie()->getData());

	if(activeDataIT->second.empty() == true){
		activeData.erase(activeDataIT);
	}

	auto it = serieLocalIdx.find(serie);
	if(it != serieLocalIdx.end())
	{
		usedLocalNameIndexes.erase(it->second);
		serieLocalIdx.erase(it);
	}

	if(persistentActiveSerieSwitch != nullptr){

		QVariant serieData;
		serieData.setValue(serie);

		int idx = persistentActiveSerieSwitch->findData(serieData);
		if(persistentActiveSerieSwitch->currentIndex() == idx){
			int newIdx = 0;
			if(activeData.empty() == false){
				if(idx == 1){
					newIdx = 2;
				}else{
					newIdx = idx -1;
				}
			}

			persistentActiveSerieSwitch->setCurrentIndex(newIdx);
			persistentActiveSerieSwitch->removeItem(idx);
		}

		tryRefreshActiveSerieSwitchesContent();

	}else{
		visualizer_->setActiveSerie(nullptr);
	}

	visualizer_->destroySerie(serie);
	
}

const bool CoreVisualizerWidget::getDataName(core::VariantConstPtr data, std::string & dataName)
{
	return (data->getMetadata("core/uiUserName", dataName) == true || data->getMetadata("core/uiName", dataName) == true
		|| data->getMetadata("core/name", dataName) == true);
}

const bool CoreVisualizerWidget::getDataSource(core::VariantConstPtr data, std::string & dataSource)
{
	return data->getMetadata("core/source", dataSource);
}

void CoreVisualizerWidget::onLiveObserveChange(bool observe)
{
	visualizer_->setLiveObserveActive(observe);


}
