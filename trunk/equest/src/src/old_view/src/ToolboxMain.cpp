#include "ToolboxMain.h"
#include "ui_toolboxmaindeffile.h"
#include <utils/Push.h>
#include <corelib/BaseDataTypes.h>
#include <QtWidgets/QAction>
#include <iostream>
#include <corelib/PluginCommon.h>
#include <corelib/IServiceManager.h>
#include <corelib/IVisualizerManager.h>
#include <corelib/ISourceManager.h>

#include <utils/Debug.h>

#include <utils/StringConversionTools.h>
#include <coreui/CoreVisualizerWidget.h>

#include <corelib/Visualizer.h>

#include <coreui/CoreDockWidget.h>
#include <coreui/CoreTitleBar.h>
#include <coreui/CoreSplitableDockWidgetT.h>
#include <timelinelib/IChannel.h>
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>
#include <plugins/newTimeline/ITimelineService.h>


using namespace coreUI;
using namespace core;
using namespace plugin;

Q_DECLARE_METATYPE(UniqueID);

struct SortActionsByNames 
{
	inline bool operator()(const QAction* a1, const QAction* a2)
	{
		return a1->text() < a2->text();
	}
};

typedef CoreSplitableDockWidgetT<WidgetClonePolicyCopyConstructor<CoreVisualizerWidget>> VisualizerWidget;

ToolboxMain::ToolboxMain(const CloseUpOperations & closeUpOperations, const std::string & appName)
	: CoreMainWindow(closeUpOperations), SingleInstanceWindow(appName),
	ui(new Ui::EDRMain), visualizersPlaceholder(new QMainWindow)
{
	ui->setupUi(this);
	qApp->setApplicationName("EDR");
	QString style = this->styleSheet();
	setDockOptions( AllowNestedDocks | AllowTabbedDocks );
	setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );
	setCentralWidget( visualizersPlaceholder );
	//setDocumentMode(true);	
	setWindowIcon(QPixmap(QString::fromUtf8(":/coreUI/icons/appIcon.png")));
}

ToolboxMain::~ToolboxMain()
{
	delete helper;
}

void ToolboxMain::initializeSplashScreen(QSplashScreen * splashScreen)
{

}

bool ToolboxMain::customViewInit(QWidget * log)
{
	trySetStyleByName("dark");
	connect(ui->menuWindow, SIGNAL(aboutToShow()), this, SLOT(populateWindowMenu()));	
	auto dockConsole = coreUI::CoreDockWidget::embeddWidget(log, tr("Log"), Qt::BottomDockWidgetArea, true);
	addDockWidget(Qt::BottomDockWidgetArea, dockConsole);
	dockConsole->setFeatures(dockConsole->features() | QDockWidget::DockWidgetVerticalTitleBar);
	dockConsole->setObjectName(QString::fromUtf8("ConsoleWidget"));
	populateVisualizersMenu(ui->menuCreateVisualizer);
	initializeUI();
	return true;
}

void ToolboxMain::initializeUI()
{
	// pozostałe widgety "pływające"
	for (int i = 0; i < getServiceManager()->getNumServices(); ++i) {
		auto service = getServiceManager()->getService(i);

		QWidget* viewWidget = service->getWidget();

		//QWidget* controlWidget = service->getControlWidget();
        //
		//QWidget* settingsWidget = service->getSettingsWidget();

		if ( viewWidget ) {
			addDockWidget(Qt::RightDockWidgetArea, coreUI::CoreDockWidget::embeddWidget(viewWidget, QString::fromStdString(service->name()), Qt::RightDockWidgetArea, true));
		}

        QWidgetList propertiesWidgets = service->getPropertiesWidgets();
        for (auto it = propertiesWidgets.begin(); it != propertiesWidgets.end(); ++it) {
			addDockWidget(Qt::LeftDockWidgetArea, coreUI::CoreDockWidget::embeddWidget(*it, QString::fromStdString(service->name()), Qt::LeftDockWidgetArea, true));
        }

		//if ( controlWidget ) {
		//	addDockWidget(Qt::BottomDockWidgetArea, coreUI::CoreDockWidget::embeddWidget(controlWidget, QString::fromStdString(service->getName()) + " " + tr("control"), Qt::BottomDockWidgetArea, true));
		//}
        //
		//if ( settingsWidget ) {
		//	addDockWidget(Qt::LeftDockWidgetArea, coreUI::CoreDockWidget::embeddWidget(settingsWidget, QString::fromStdString(service->getName()) + " " + tr("settings"), Qt::LeftDockWidgetArea, true));
		//}
	}

	for (int i = 0; i < getSourceManager()->getNumSources(); ++i) {
		auto source = getSourceManager()->getSource(i);

		QWidget* viewWidget = source->getWidget();

		QWidget* controlWidget = source->getControlWidget();

		QWidget* settingsWidget = source->getSettingsWidget();

		if ( viewWidget ) {
			addDockWidget(Qt::RightDockWidgetArea, coreUI::CoreDockWidget::embeddWidget(viewWidget, QString::fromStdString(source->name()), Qt::RightDockWidgetArea, true));
		}

		if ( controlWidget ) {
			addDockWidget(Qt::BottomDockWidgetArea, coreUI::CoreDockWidget::embeddWidget(controlWidget, QString::fromStdString(source->name()) + " " + tr("control"), Qt::BottomDockWidgetArea, true));
		}

		if ( settingsWidget ) {
			addDockWidget(Qt::LeftDockWidgetArea, coreUI::CoreDockWidget::embeddWidget(settingsWidget, QString::fromStdString(source->name()) + " " + tr("settings"), Qt::LeftDockWidgetArea, true));
		}
	}

	auto timelineService = core::queryService<ITimelineService>(plugin::getServiceManager());

	helper = new VisualizerTimelineHelper(timelineService.get());
}

void ToolboxMain::onExit()
{
	close();
}

void ToolboxMain::populateWindowMenu()
{
	// uwaga: nie musimy usuwać starych akcji, ponieważ QMenu pilnuje,
	// aby nie był dodane dwie jednakowe instancje
	populateWindowMenu(ui->menuWindow);
}

void ToolboxMain::populateVisualizersMenu()
{
	// czyścimy menu
	ui->menuCreateVisualizer->clear();
	populateVisualizersMenu(ui->menuCreateVisualizer);
}

void ToolboxMain::actionCreateVisualizer()
{
	QAction* action = qobject_cast<QAction*>(sender());
	auto visProto = getVisualizerManager()->getVisualizerPrototype(action->data().value<UniqueID>());
	VisualizerWidget * dockWidget = new VisualizerWidget();
	CoreVisualizerWidget* widget = new CoreVisualizerWidget(VisualizerPtr(visProto->create()), this);//, Qt::WindowTitleHint);
	widget->getVisualizer()->addDataSource(core::Visualizer::DataSourcePtr(new core::VisualizerMemoryDataSource(plugin::getDataManagerReader())));

	widget->getVisualizer()->addObserver(helper);

	QList<QAction*> actions = widget->getVisualizer()->getOrCreateWidget()->actions();
	actions.append(widget->actions());
	actions.append(dockWidget->actions());

	dockWidget->setWidget(widget);

	auto visualizerTitleBar = CoreTitleBar::supplyWithCoreTitleBar(dockWidget);
	CoreTitleBar::supplyCoreTitleBarWithActions(visualizerTitleBar, actions);
	
	visualizersPlaceholder->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
}

void ToolboxMain::populateVisualizersMenu( QMenu* menu )
{
	std::vector<QAction*> sortedActions;
	// dodajemy wizualizatory
	IVisualizerManager::VisualizerPrototypes visPrototypes;
	getVisualizerManager()->visualizerPrototypes(visPrototypes);
	for(auto vis : visPrototypes) {
		QAction* action = new QAction(utils::Convert::toQString(vis->getName()), menu);
		action->setData( qVariantFromValue(vis->getID()) );
		action->setIcon( vis->getIcon() );
		action->connect( action, SIGNAL(triggered()), this, SLOT(actionCreateVisualizer()) );
		sortedActions.push_back(action);
	}
	std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
	for(auto action : sortedActions) {
		menu->addAction(action);
	}
}

void ToolboxMain::populateWindowMenu( QMenu* menu )
{
	std::vector<QAction*> sortedActions;

	QList<QDockWidget*> dockwidgets = this->findChildren<QDockWidget*>();
	if ( dockwidgets.size() ) {
		// pobranie i posortowanie akcji wg nazw
		sortedActions.reserve(dockwidgets.size());
		for(auto dockWidget : dockwidgets) {
			sortedActions.push_back(dockWidget->toggleViewAction());
		}
		std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
		for(auto action : sortedActions) {
			menu->addAction(action);
		}
		menu->addSeparator();
		sortedActions.resize(0);
	}

	QList<QToolBar*> toolbars = this->findChildren<QToolBar*>();
	if (toolbars.size()) {
		sortedActions.reserve(toolbars.size());
		for(auto toolbar : toolbars) {
			sortedActions.push_back(toolbar->toggleViewAction());
		}
		std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
		for(auto action : sortedActions) {
			menu->addAction(action);
		}
	}
}
