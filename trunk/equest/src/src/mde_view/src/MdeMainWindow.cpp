#include "MdePCH.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSplashScreen>
#include <QtWidgets/QMainWindow>
#include <utils/Debug.h>
#include "MdeMainWindow.h"
#include <coreui/CoreTextEditWidget.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTreeView>
#include "AboutDialog.h"
#include <corelib/IServiceManager.h>
#include <corelib/ISourceManager.h>
#include <coreui/CoreDockWidget.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <coreui/IMdeTab.h>
#include <coreui/SimpleTab.h>
#include <coreui/ReportsTab.h>
#include "AnalisisWidget.h"
#include "AnalysisTab.h"
#include "ui_toolboxmaindeffile.h"
#include "MdeServiceWindow.h"
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include <plugins/hmdbCommunication/OperationProgressWidget.h>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <corelib/PluginCommon.h>
#include <plugins/hmdbCommunication/GeneralSourceViewWidget.h>
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include <plugins/hmdbCommunication/DataViewConfigurationWidget.h>
#include <plugins/hmdbCommunication/ShallowCopyFilter.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include "MDEPerspectiveWidget.h"
#include <plugins/hmdbCommunication/DataSourcePerspective.h>
#include <plugins/hmdbCommunication/DataSourceDefaultContent.h>
#include "plugins/hmdbCommunication/SourceOptionsWidget.h"
#include "plugins/kinematic/IKinematicVisualizer.h"
#include "corelib/IVisualizerManager.h"
#include "plugins/hmdbCommunication/ContextConfigurationSettingsFile.h"

#include "coreui/KioskUtilities.h"

using namespace core;

class MDEHMDBSourceView : public hmdbCommunication::IHMDBSourceViewManager::IHMDBSourceContextView
{
public:
	//! \return Nazwa widoku
	virtual const QString name() const { return QObject::tr("MDE view"); }
	//! \param shallowCopyContext Kontekst płytkiej kopii bazy danych jakim zasilamy widok
	//! \return Widok obsługujący kontekst
	virtual QWidget * createView(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext, hmdbCommunication::IHMDBSourceViewManager * viewManager) {

		auto ret = new MDEPerspectiveWidget(shallowCopyContext);
		auto config = ret->generalDataView()->dataViewConfiguration();

		auto fs = viewManager->filtersCount(name());
		if (fs > 0){
			config->addFilterSeparator();
		}

		for (unsigned int i = 0; i < fs; ++i){
			auto f = viewManager->filter(i, name())->create(shallowCopyContext->shallowCopyRemoteContext()->remoteContext()->session().get());
			config->registerFilter(f);
		}

		auto cs = viewManager->contentsCount(name());
		for (unsigned int i = 0; i < cs; ++i){
			auto c = viewManager->content(i, name());
			config->registerContent(c);
		}

		auto ps = viewManager->perspectivesCount(name());
		for (unsigned int i = 0; i < ps; ++i){
			auto p = viewManager->perspective(i, name());
			config->registerPerspective(p);
		}

		return ret;
	}
	//! \return Czy dany widok wymaga połączenia z usługami webowymi
	virtual const bool requiresRemoteContext() const { return true; }
};


void recursiveStyleSheet(QWidget* parent, const QString& replace)
{
	if (parent) {
		QString ss = parent->styleSheet();
		ss.replace(QRegularExpression("91\\s*,\\s*91\\s*,\\s*91"), replace);//QString("91"), QString("1"));
		parent->setStyleSheet(ss);
		auto children = parent->children();
		for (auto it = children.begin(); it != children.end(); ++it) {
			QWidget* w = qobject_cast<QWidget*>(*it);
			recursiveStyleSheet(w, replace);
		}
	}
}



MdeMainWindow::MdeMainWindow(const CloseUpOperations & closeUpOperations, const std::string & appName)
	: coreUI::CoreMainWindow(closeUpOperations), coreUI::SingleInstanceWindow(appName),
    controller(this)
{
    ui = new Ui::HMMMain();
    ui->setupUi(this);
#ifdef DEMO_MODE
	{
		auto iniPath = plugin::getUserApplicationDataPath("mde_communication.ini").string();
		QSettings s(QString::fromStdString(iniPath), QSettings::IniFormat);
		int r = s.value("ColorR", 91).toInt();
		int g = s.value("ColorG", 91).toInt();
		int b = s.value("ColorB", 91).toInt();

		QString title = s.value("AppTitle", "ANALYSIS & SYNTHESIS OF HUMAN MOTION").toString();
		ui->logo->setVisible(false);
		ui->label->setText(title);
		recursiveStyleSheet(this, QString("%1, %2, %3").arg(r).arg(g).arg(b));
		ui->versionLabel->setText(QString("ver. %1").arg(Version::formatedVersion(Version(0, 9, 1)).c_str()));
	}
#endif
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

    contextPlaceholder = new QTabWidget(this);
    contextPlaceholder->setTabsClosable(false);
    contextPlaceholder->setDocumentMode(true);
    contextPlaceholder->setMovable(false);
    contextPlaceholder->setVisible(false);
    
    ui->tabPlaceholder->layout()->addWidget(contextPlaceholder);
    ui->tabPlaceholder->show();
    contextPlaceholder->show();
    ui->templateButton->hide();
    contextPlaceholder->findChild<QTabBar*>()->setDrawBase(false);

    contextEventFilter = ContextEventFilterPtr(new ContextEventFilter(this));
    analysisModel = AnalisisModelPtr(new AnalisisModel());

	
}

MdeMainWindow::~MdeMainWindow()
{
}

void MdeMainWindow::initializeSplashScreen(QSplashScreen * splashScreen)
{
    splashScreen->setPixmap(QPixmap(":/resources/splashscreen/splash.png"));
}

void MdeMainWindow::showSplashScreenMessage(const QString & message)
{
    splashScreen()->showMessage(message, Qt::AlignBottom | Qt::AlignLeft, Qt::red);
}






bool MdeMainWindow::customViewInit(QWidget * log)
{
   plugin::getDataHierarchyManagerReader()->addObserver(analysisModel);
   trySetStyleByName("hmm");

 
#if defined(_WINDOWS)
   this->showFullScreen();
   this->setFixedSize(this->width(), this->height());
#endif
   auto kiosk = [&]() {
	   ui->minimizeButton->setVisible(false);
	   ui->exitButton->setVisible(false);
	   ui->aboutButton->setVisible(false);
	   #ifdef _DEBUG
	   ui->minimizeButton->setVisible(true);
	   #endif
   };
   //coreUI::KioskUtilities::startKioskModeIfParamIsSet(kiosk);
   scopedKiosk = utils::make_unique<coreUI::ScopedKiosk>(kiosk);

   utils::shared_ptr<hmdbCommunication::IHMDBSource> icomm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
   plugin::ISourcePtr commSource = utils::dynamic_pointer_cast<plugin::ISource>(icomm);


   if (icomm != nullptr){
	   auto vm = icomm->viewManager();
	   auto hmdbView = new MDEHMDBSourceView;

	   vm->registerViewPrototype(hmdbView);

	   {
		   typedef hmdbCommunication::ContextConfigurationSettingsFile ConfFile;
		   auto iniPath = plugin::getUserApplicationDataPath("mde_communication.ini").string();
		   PLUGIN_LOG_INFO("Communication configuration file: " << iniPath);


		   //ConfFile::write("C:/rawcommunication.ini", ConfFile::defaultConfig());
		   hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration ccfg = ConfFile::read(QString::fromStdString(iniPath));
//		   ccfg.name = tr("Default PJATK MDE data connection");
//		   ccfg.storageConfiguration.path = QString::fromStdString((plugin::getPaths()->getUserApplicationDataPath() / "db" / "localStorage.db").string());
//		   ccfg.storageConfiguration.password = "P,j.W/s<T>k2:0\"1;2";
//
////#ifdef _DEBUG 
//		   ccfg.motionServicesConfiguration.userConfiguration.user = "test_PJWSTK";
//		   ccfg.motionServicesConfiguration.userConfiguration.password = "PJtestP@ss";
////#endif
//		   ccfg.motionServicesConfiguration.serviceConfiguration.url = "https://v21.pjwstk.edu.pl/HMDB";
//		   ccfg.motionServicesConfiguration.serviceConfiguration.caPath = QString::fromStdString((plugin::getPaths()->getResourcesPath() / "v21.pjwstk.edu.pl.crt").string());
//
//		   ccfg.motionDataConfiguration.serviceConfiguration.url = "ftps://v21.pjwstk.edu.pl";
//		   ccfg.motionDataConfiguration.serviceConfiguration.caPath = "";
//		   ccfg.motionDataConfiguration.userConfiguration.user = "testUser";
//		   ccfg.motionDataConfiguration.userConfiguration.password = "testUser";

		   vm->registerConfiguration(ccfg, hmdbView->name());
	   }

	   vm->registerPerspective(new hmdbCommunication::DataSourcePatientPerspective, hmdbView->name());
	   vm->registerPerspective(new hmdbCommunication::DataSourceGenderPerspective, hmdbView->name());
	   vm->registerPerspective(new hmdbCommunication::DataSourceDisorderPerspective, hmdbView->name());
	   vm->registerContent(new hmdbCommunication::DataSourceDefaultContent, hmdbView->name());
   }
   
   auto sourceManager = plugin::getSourceManager();
   for (int i = 0; i < sourceManager->getNumSources(); ++i) {
       auto source = sourceManager->getSource(i);
       QWidget* widget = source->getWidget();
       if (widget) {
           addTab(coreUI::IMdeTabPtr(new SimpleTab(widget, QIcon(":/mde/icons/Badania.png"),tr(source->name().c_str()))));
       }
   }
   
   AnalisisWidget* aw = new AnalisisWidget(analysisModel, contextEventFilter, nullptr);
   // -----------------------
   QIcon analysisIcon;
   analysisIcon.addPixmap(QPixmap(":/mde/icons/Analizy.png"),QIcon::Active);
   analysisIcon.addPixmap(QPixmap(":/mde/icons/AnalizyH.png"),QIcon::Disabled);
   auto analysisTab = utils::make_shared<AnalysisTab>(aw, analysisIcon, tr("Analysis"));
   analysisTab->setEnabled(false);
   addTab(analysisTab);


#ifndef DEMO_MODE
   QIcon reportsIcon;
   reportsIcon.addPixmap(QPixmap(":/mde/icons/Raporty.png"),QIcon::Active);
   reportsIcon.addPixmap(QPixmap(":/mde/icons/RaportyH.png"),QIcon::Disabled);
   auto reportsTab = utils::make_shared<coreUI::ReportsTab>(reportsIcon, tr("Reports"));
   connect(aw, SIGNAL(reportCreated(const QString& )), reportsTab.get(), SLOT(enable()));
   reportsTab->setEnabled(false);
   addTab(reportsTab);
   addTab(coreUI::IMdeTabPtr(new SimpleTab(log, QIcon(":/mde/icons/Operacje.png"),tr("Log"))));
   // TODO : najlepiej byloby przeniesc to do kontrolera
   connect(analysisModel.get(), SIGNAL(reportCreated(const QString&)), reportsTab->getMainWidget(), SLOT(setHtml(const QString&)));
#endif
   auto serviceManager = plugin::getServiceManager();
   auto timeline = plugin::getServiceManager()->getService(core::UID::GenerateUniqueID("{0157346E-D1F3-4A4F-854F-37C87FA3E5F9}"));
   for (int i = 0; i < serviceManager->getNumServices(); ++i) {
       plugin::IServicePtr service = serviceManager->getService(i);
       
       if (service != timeline) {
            auto w = createServiceWidget(service);
            if (w) {
                addTab(coreUI::IMdeTabPtr(new SimpleTab(w, QIcon(":/mde/icons/Operacje.png"),tr(service->name().c_str()))));
                addPropertiesToServiceWindow(service, w);

            }
       }

        core::IFilterProviderPtr filterProvider = utils::dynamic_pointer_cast<core::IFilterProvider>(service);
        if (filterProvider) {
            analysisModel->addFilterBundles(filterProvider->getFilterBundles());
        }
   }
   // hack, modyfikuje prototyp, zeby ustalic osie w tworzonych wizualizatorach, moze warto pomyslec o konfiguracjach?
   auto visManager = plugin::getVisualizerManager();
   auto proto = visManager->getVisualizerPrototype(core::UID::GenerateUniqueID("{E8B5DEB2-5C57-4323-937D-1FFD288B65B9}"));
   auto kinematicProto = dynamic_cast<kinematic::IKinematicVisualizer*>(const_cast<plugin::IVisualizer*>(proto->visualizer()));
   if (kinematicProto) {
		kinematicProto->setAxisXYZ(true);
   }

   emit modelChanged();
   emit activateTab(*tabs.begin());

#ifdef DEMO_MODE
   SourceOptionsWidget::login();
#endif // DEMO_MODE
   return true;
}

void MdeMainWindow::addTab( coreUI::IMdeTabPtr tab )
{
    tabs.push_back(tab);
    emit tabAdded(tab);
}

MdeServiceWindow* MdeMainWindow::createServiceWidget( plugin::IServicePtr service )
{
    QWidget* viewWidget = service->getWidget();
    
    MdeServiceWindow* w = nullptr;
    if(viewWidget) {
        QString serviceName = QString::fromStdString(service->name());
        w = new MdeServiceWindow(serviceName);
        w->setCentralWidget(viewWidget);
        w->addActions(viewWidget->actions());

        /*QWidgetList properites = service->getPropertiesWidgets();
        if (!properites.empty()) {
            w->loadLayout();
            int propNo = 0;
            for (auto it = properites.begin(); it != properites.end(); ++it) {
                w->createMdeDock(serviceName, *it);
            }
        }*/
    }

    return w;
}

void MdeMainWindow::addPropertiesToServiceWindow( plugin::IServicePtr service, MdeServiceWindow * w )
{
    QWidgetList properites = service->getPropertiesWidgets();
    if (!properites.empty()) {
        w->loadLayout();
        QString serviceName = QString::fromStdString(service->name());
        for (auto it = properites.begin(); it != properites.end(); ++it) {
            w->createMdeDock(serviceName, *it);
        }
    }
}

void MdeMainWindow::onAbout()
{
	std::unique_ptr<AboutDialog> dialog(new AboutDialog);
	dialog->exec();
}


void MdeMainWindowController::addTab( coreUI::IMdeTabPtr tab )
{
    QToolButton* templateB = window->ui->templateButton;
    QToolButton* button = new QToolButton();
    button->setText(tab->getLabel());
    button->setIcon(tab->getIcon());
    button->setIconSize(templateB->iconSize());
    button->setToolButtonStyle(templateB->toolButtonStyle());
    button->setStyleSheet(templateB->styleSheet());
    button->setMinimumSize(templateB->minimumSize());
    button->setCheckable(true);
    button->setFixedWidth(templateB->width());
    button->setEnabled(tab->isEnabled());
    QObject* obj = dynamic_cast<QObject*>(tab.get());
    if (obj) {
        connect(obj, SIGNAL(tabEnabled(bool)), this, SLOT(update()));
        connect(obj, SIGNAL(tabActivated(bool)), this, SLOT(activateTab()));
    } else {
        UTILS_ASSERT(false, "IMdeTab powinien byc zaimplementowany przez jakis QObject, bo implementacja zaklada uzycie sygnalow");
    }
   
    QLayout* layout = window->ui->toolBar->layout();
    auto count = layout->count();
    auto item = layout->itemAt(count - 1);
    layout->removeItem(item);
    layout->addWidget(button);
    layout->addItem(item);

    button2TabWindow[button] = tab;
    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    auto context = coreUI::IAppUsageContextPtr(new coreUI::MdeEmptyContext());
    tab2Contex[tab] = context;

    coreUI::IAppUsageContextManager* contextManager = dynamic_cast<coreUI::IAppUsageContextManager*>(window);
    contextManager->addContext(context);
    contextManager->addWidgetToContext(context, button);
    coreUI::IContextRootPtr tabContex = utils::dynamic_pointer_cast<coreUI::IContextRoot>(tab);
    if (tabContex) {
        tabContex->registerConxtext(contextManager, context, window->contextPlaceholder);
    }

    QWidget* widget = tab->getMainWidget();
    window->ui->mainArea->layout()->addWidget(widget);
    widget->hide();
}

MdeMainWindowController::MdeMainWindowController( MdeMainWindow* mw ) : 
window(mw)
{
    connect(mw, SIGNAL(modelChanged()), this, SLOT(update()));
    connect(mw, SIGNAL(activateTab(coreUI::IMdeTabPtr)), this, SLOT(activateTab(coreUI::IMdeTabPtr)));
    connect(mw, SIGNAL(tabAdded(coreUI::IMdeTabPtr)), this, SLOT(addTab(coreUI::IMdeTabPtr)));
}

void MdeMainWindowController::buttonClicked()
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    UTILS_ASSERT(button);
    coreUI::IMdeTabPtr tab = button2TabWindow[button];
    activateTab(tab);
    handleContext(tab, button);

}

void MdeMainWindowController::activateTab( coreUI::IMdeTabPtr tab )
{
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        coreUI::IMdeTabPtr t = it->second;
        QObject* obj = dynamic_cast<QObject*>(t.get());
        if (obj) {
            obj->blockSignals(true);
            t->setActive(t == tab);
            obj->blockSignals(false);
            t->getMainWidget()->setVisible(false);
            it->first->setChecked( t == tab);
        } else {
            UTILS_ASSERT(false, "obecna implementacja wymaga aby realizacja IMdeTab byla QObject");
        }
    }

    tab->getMainWidget()->setVisible(true);
}

void MdeMainWindowController::activateTab()
{
    QObject* obj = sender();
    coreUI::IMdeTab* tab = dynamic_cast<coreUI::IMdeTab*>(obj);
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        if (it->second.get() == tab) {
            activateTab(it->second);
            handleContext(it->second, it->first);
            return;
        }
    }
}

void MdeMainWindowController::update()
{
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        coreUI::IMdeTabPtr t = it->second;
        it->first->setEnabled( t->isEnabled());
    }
}

void MdeMainWindowController::handleContext( coreUI::IMdeTabPtr tab, QToolButton* button )
{
    if (window->isContextWidget(tab->getMainWidget())) {
        window->setCurrentContext(tab->getMainWidget());
    } else {
        window->setCurrentContext(button);
    }
}


