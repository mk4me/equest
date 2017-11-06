#include "MdePCH.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSplashScreen>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplitter>
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
#include <corelib/IVisualizerManager.h>
#include <corelib/IVisualizer.h>
#include <QtWidgets/QMessageBox>
#include <QtGui/QCloseEvent>
#include <corelib/Version.h>
#include <plugins/medusaExporter/IMedusaExporterService.h>
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
#include <plugins/hmdbCommunication/DataSourceDefaultContent.h>
#include <plugins/dicom/AnnotationStatusFilter.h>
#include "ImageTableWidget.h"
#include "ImageTableModel.h"
#include "LoginDialog.h"
#include "plugins/hmdbCommunication/SourceOptionsWidget.h"
#include "plugins/hmdbCommunication/ContextConfigurationSettingsFile.h"

using namespace core;

class MEDUSAHMDBSourceView : public hmdbCommunication::IHMDBSourceViewManager::IHMDBSourceContextView
{
public:
	//! \return Nazwa widoku
	virtual const QString name() const { return QObject::tr("MEDUSA view"); }

	//! \param shallowCopyContext Kontekst płytkiej kopii bazy danych jakim zasilamy widok
	//! \return Widok obsługujący kontekst
	virtual QWidget * createView(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext, hmdbCommunication::IHMDBSourceViewManager * viewManager) {

		auto ret = new GeneralSourceViewWidget(shallowCopyContext);
		auto config = ret->dataViewConfiguration();
		config->setPerspectiveVisible(false);
		config->setContentVisible(false);
		config->setFilterVisible(true);		
		
		auto fs = viewManager->filtersCount(name());
		if (fs > 0){
			config->addFilterSeparator();
		}

		auto scrc = shallowCopyContext->shallowCopyRemoteContext();
		if (scrc != nullptr){

			for (unsigned int i = 0; i < fs; ++i){

				auto f = viewManager->filter(i, name())->create(scrc->remoteContext()->session().get());
				config->registerFilter(f);
			}

		}
		else{
			for (unsigned int i = 0; i < fs; ++i){

				auto f = viewManager->filter(i, name())->create(nullptr);
				config->registerFilter(f);
			}
		}

		hmdbCommunication::IDataSourcePerspective * perspective = nullptr;
		hmdbCommunication::IDataSourceContent * content = nullptr;

		if (viewManager->perspectivesCount(name()) > 0){
			perspective = viewManager->perspective(0, name());
		}
		else if (viewManager->perspectivesCount() > 0){
			perspective = viewManager->perspective(0);
		}

		ret->dataView()->setPerspective(perspective);

		if (viewManager->contentsCount(name()) > 0){
			content = viewManager->content(0, name());
		}
		else if (viewManager->contentsCount() > 0){
			content = viewManager->content(0);
		}

		ret->dataView()->setContent(content);

		ret->setVisible(false);
		return ret;
	}
	//! \return Czy dany widok wymaga połączenia z usługami webowymi
	virtual const bool requiresRemoteContext() const { return true; }
};

//! Perspektywa medusy - tylko sesje
class MEDUSAPerspective : public hmdbCommunication::IDataSourcePerspective
{
public:

	MEDUSAPerspective()
	{

	}

	virtual ~MEDUSAPerspective()
	{

	}

	virtual void defaultHeaders(QStringList & headers) const
	{
		headers << QObject::tr("Sessions");
	}

	virtual const QString name() const
	{
		return QObject::tr("Sessions only");
	}

	virtual void rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const hmdbCommunication::ShallowCopy & shallowCopy)
	{
		std::multimap<hmdbServices::ID, hmdbCommunication::TreeWidgetContentItem*> sessionItems;
		std::list<hmdbCommunication::TreeWidgetContentItem*> unrecognizedSessionItems;

		auto subjectsITEnd = shallowCopy.motionShallowCopy.performers.end();
		for (auto subjectIT = shallowCopy.motionShallowCopy.performers.begin(); subjectIT != subjectsITEnd; ++subjectIT){
			//jeżeli pusty pacjent to go pomijamy
			if (subjectIT->second->performerConfs.empty() == true){
				continue;
			}

			auto perfConfsITEnd = subjectIT->second->performerConfs.end();
			for (auto perfConfIT = subjectIT->second->performerConfs.begin(); perfConfIT != perfConfsITEnd; ++perfConfIT){
				if (perfConfIT->second->session->trials.empty() == true){
					continue;
				}

				//generuje item sesji
				auto sessionItem = new hmdbCommunication::TreeWidgetContentItem(hmdbCommunication::SessionContent, perfConfIT->second->session->sessionID);

				const auto sNumber = sessionLocalNumebr(perfConfIT->second->session->sessionName);

				if (sNumber > -1){
					sessionItems.insert(std::multimap<hmdbServices::ID, hmdbCommunication::TreeWidgetContentItem*>::value_type(sNumber, sessionItem));
				}
				else{
					unrecognizedSessionItems.push_back(sessionItem);
				}

				auto motionsITEnd = perfConfIT->second->session->trials.end();
				for (auto motionIT = perfConfIT->second->session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
					if (motionIT->second->files.empty() == true){
						continue;
					}

					//generuje item motiona
					auto motionItem = new hmdbCommunication::TreeWidgetContentItem(hmdbCommunication::MotionContent, motionIT->second->trialID);

					sessionItem->addChild(motionItem);
				}
			}
		}

		for (auto it = sessionItems.begin(); it != sessionItems.end(); ++it){
			treeWidgetItem->addChild(it->second);
		}

		for (auto it = unrecognizedSessionItems.begin(); it != unrecognizedSessionItems.end(); ++it){
			treeWidgetItem->addChild(*it);
		}
	}

	const bool headers(const QTreeWidgetItem * item, QStringList & _headers) const
	{
		_headers << QObject::tr("Database");
		return true;
	}

private:

	static const int sessionLocalNumebr(const std::string & sessionName)
	{
		int ret = -1;

		auto sIdx = sessionName.find("-S");
		if (sIdx != std::string::npos){
			sIdx += 2;
			const auto indexString = sessionName.substr(sIdx, sessionName.size() - sIdx);
			if (indexString.empty() == false){
				try{
					ret = boost::lexical_cast<int>(indexString);
				}
				catch (...){

				}
			}
		}

		return ret;
	}
};

MdeMainWindow::MdeMainWindow(const CloseUpOperations & closeUpOperations, const std::string & appName)
	: coreUI::CoreMainWindow(closeUpOperations), coreUI::SingleInstanceWindow(appName),
	controller(this), commWidget(nullptr)
{
    ui = new Ui::HMMMain();
    ui->setupUi(this);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
	this->hide();
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
	ui->versionLabel->setText(QString("ver. %1").arg(Version::formatedVersion().c_str()));

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
    splashScreen()->showMessage(message, Qt::AlignBottom | Qt::AlignLeft, Qt::white);
}

bool MdeMainWindow::customViewInit(QWidget * log)
{
	plugin::getDataHierarchyManagerReader()->addObserver(analysisModel);
   trySetStyleByName("hmm");
 
#if defined(_WINDOWS)
   this->showFullScreen();
   this->setFixedSize(this->width(), this->height());
#endif
   utils::shared_ptr<hmdbCommunication::IHMDBSource> icomm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
   plugin::ISourcePtr commSource = utils::dynamic_pointer_cast<plugin::ISource>(icomm);

   if (!icomm) {
	   PLUGIN_LOG_ERROR("Unable to find communication plugin");
	   return false;
   }

   auto hmdbView = new MEDUSAHMDBSourceView;
   auto vm = icomm->viewManager();

	vm->registerViewPrototype(hmdbView);

	auto ccfg = createHmdbConfig();
	{
		vm->registerConfiguration(ccfg, hmdbView->name());
	}

	vm->registerPerspective(new MEDUSAPerspective, hmdbView->name());
	vm->registerContent(new hmdbCommunication::DataSourceDefaultContent, hmdbView->name());

	//dodajemy filtry dla adnotacji		
	auto as = QObject::tr("Annotation status");
	vm->registerFilter(new dicom::AnnotationStatusFilter((as + ": " + QObject::tr("in edition")), true, false, dicom::AnnotationStatusFilter::InEdition), hmdbView->name());
	vm->registerFilter(new dicom::AnnotationStatusFilter((as + ": " + QObject::tr("in verification")), true, false, dicom::AnnotationStatusFilter::InVerification), hmdbView->name());
	vm->registerFilter(new dicom::AnnotationStatusFilter((as + ": " + QObject::tr("verified")), true, false, dicom::AnnotationStatusFilter::Verified), hmdbView->name());
   

   
   auto sourceManager = plugin::getSourceManager();
   for (int i = 0; i < sourceManager->getNumSources(); ++i) {
       auto source = sourceManager->getSource(i);
       QWidget* widget = source->getWidget();
       if (source != commSource && widget) {
           addTab(coreUI::IMdeTabPtr(new SimpleTab(widget, QIcon(":/mde/icons/Badania.png"),tr(source->name().c_str()))));
       }
   }

   this->commWidget = commSource->getWidget();

   //icomm->setCompactMode(true);
   //commWidget->setMaximumWidth(304);
   
   //hack
   auto list = commWidget->findChildren<SourceOptionsWidget*>();
   UTILS_ASSERT(!list.isEmpty());
   SourceOptionsWidget* sourceOptionsWidget = *list.begin();
   sourceOptionsWidget->hide();
   QSplitter* compound = new QSplitter();
   AnalisisWidget* aw = new AnalisisWidget(analysisModel, contextEventFilter, nullptr);
   connect(aw, &AnalisisWidget::hideCommunication, this, [&]() {this->commWidget->setVisible(!this->commWidget->isVisible()); });
   // -----------------------
   compound->addWidget(commWidget);
   compound->addWidget(aw);
   addTab(coreUI::IMdeTabPtr(new CompoundAnalysisTab(compound, aw, QIcon(":/mde/icons/Analizy.png"), tr("Analysis"))));

//#ifndef DEMO_MODE
   addTab(coreUI::IMdeTabPtr(new SimpleTab(log, QIcon(":/mde/icons/Operacje.png"),tr("Log"))));
//#endif
   // TODO : najlepiej byloby przeniesc to do kontrolera
//   bool cc = connect(analysisModel.get(), SIGNAL(reportCreated(const QString&)), reportsTab->getMainWidget(), SLOT(setHtml(const QString&)));
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

//#ifndef DEMO_MODE
   QToolButton* exporterButton = controller.createButton(tr("Exporter"), QIcon(":/mde/icons/Operacje.png"));
   controller.addToolbarButton(exporterButton);
   connect(exporterButton, SIGNAL(clicked()), this, SLOT(showMedusaExporterDialog()));
   addTab(coreUI::IMdeTabPtr(new ImageTableTab(aw, QIcon(":/mde/icons/Operacje.png"), tr("TableView"))));
//#endif

   emit activateTab(*tabs.begin());

#ifdef DEMO_MODE
   SourceOptionsWidget::login();
#else
   bool done = false;
   do {
	   LoginDialog ld(this, ccfg.motionServicesConfiguration.userConfiguration.user, ccfg.motionServicesConfiguration.userConfiguration.password);
	   if (ld.exec() == QDialog::Accepted) {
		   auto config = sourceOptionsWidget->getConnectionProfile();
		   config.motionServicesConfiguration.userConfiguration.user = ld.getUser();
		   config.motionServicesConfiguration.userConfiguration.password = ld.getPassword();
		   sourceOptionsWidget->setConnectionProfile(config);
		   sourceOptionsWidget->onLogin();
		   QStringList errors;
		   sourceOptionsWidget->verify(errors);
		   done = errors.isEmpty();
		   if (!done) {
			   QMessageBox::warning(nullptr, QString("Login failed"), errors.join("\n"));
		   }

	   } else {
		   return false;
	   }
   } while (!done);
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

void MdeMainWindowController::addTab( coreUI::IMdeTabPtr tab )
{
    QToolButton* button = createButton(tab->getLabel(), tab->getIcon());
    addToolbarButton(button);


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
    connect(mw, SIGNAL(activateTab(coreUI::IMdeTabPtr)), this, SLOT(activateTab(coreUI::IMdeTabPtr)));
    connect(mw, SIGNAL(tabAdded(coreUI::IMdeTabPtr)), this, SLOT(addTab(coreUI::IMdeTabPtr)));
}

void MdeMainWindowController::buttonClicked()
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    UTILS_ASSERT(button);
    coreUI::IMdeTabPtr tab = button2TabWindow[button];
    activateTab(tab);
    if (window->isContextWidget(tab->getMainWidget())) {
        window->setCurrentContext(tab->getMainWidget());
    } else {
        window->setCurrentContext(button);
    }
}

void MdeMainWindowController::activateTab( coreUI::IMdeTabPtr tab )
{
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        coreUI::IMdeTabPtr t = it->second;
        t->setActive(t == tab);
        t->getMainWidget()->setVisible(false);
        it->first->setChecked( t == tab);
    }

    tab->getMainWidget()->setVisible(true);
}

QToolButton* MdeMainWindowController::createButton(const QString& label, const QIcon& icon)
{
    QToolButton* templateB = window->ui->templateButton;
    QToolButton* button = new QToolButton();
    button->setText(label);
    button->setIcon(icon);
    button->setIconSize(templateB->iconSize());
    button->setToolButtonStyle(templateB->toolButtonStyle());
    button->setStyleSheet(templateB->styleSheet());
    button->setMinimumSize(templateB->minimumSize());
    button->setCheckable(true);
    button->setFixedWidth(templateB->width());
    return button;
}

void MdeMainWindowController::addToolbarButton(QToolButton* button)
{
    QLayout* layout = window->ui->toolBar->layout();
    auto count = layout->count();
    auto item = layout->itemAt(count - 1);
    layout->removeItem(item);
    layout->addWidget(button);
    layout->addItem(item);
}

void MdeMainWindow::closeEvent(QCloseEvent* event)
{
	bool close = true;

	auto visManager = plugin::getVisualizerManager();

	if (visManager) {
		core::IVisualizerManager::VisualizerInstances visInstances;

		visManager->getVisualizersInstances(visInstances);

		for(auto it = visInstances.begin(); it != visInstances.end(); ++it){

			auto w = (*it)->getWidget();
			if(w != nullptr && w->isWindowModified() == true){
				close = false;
				break;
			}
		}
	}
	if(close == false){
		if(QMessageBox::question( this, tr("Confirm exit"), tr("There are some unsaved data changes. Do You want to close application and discard them?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::No){

				event->ignore();
		}else{

			onClose();
			event->accept();

		}
	}else{
		coreUI::CoreMainWindow::closeEvent(event);
	}
}

void MdeMainWindow::showMedusaExporterDialog()
{
	auto exporter = core::queryService<medusaExporter::IMedusaExporterService>(plugin::getServiceManager());
	if (exporter) {
        QWidget* dlg = exporter->getExporterDialog();
        dlg->setWindowFlags(Qt::Tool);
        dlg->setWindowModality(Qt::ApplicationModal);
		dlg->show();
	}
}

hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration MdeMainWindow::createHmdbConfig()
{
	hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration ccfg;
	ccfg.name = tr("Default PJATK MEDUSA data connection");
	ccfg.storageConfiguration.path = QString::fromStdString((plugin::getPaths()->getUserApplicationDataPath() / "db" / "localStorage.db").string());
	ccfg.storageConfiguration.password = "P,j.W/s<T>k2:0\"1;2";
#ifdef _DEBUG
	//ccfg.motionServicesConfiguration.userConfiguration.user = "usg-medusapl";
	//ccfg.motionServicesConfiguration.userConfiguration.password = "usg-medusaxx";
	ccfg.motionServicesConfiguration.userConfiguration.user = "test-student";
	ccfg.motionServicesConfiguration.userConfiguration.password = "test-Medusa";
#endif
	ccfg.motionServicesConfiguration.serviceConfiguration.url = "https://v21.pjwstk.edu.pl/HMDB";
	ccfg.motionServicesConfiguration.serviceConfiguration.caPath = QString::fromStdString((plugin::getPaths()->getResourcesPath() / "v21.pjwstk.edu.pl.crt").string());

	ccfg.motionDataConfiguration.serviceConfiguration.url = "ftps://v21.pjwstk.edu.pl";
	ccfg.motionDataConfiguration.serviceConfiguration.caPath = "";
	ccfg.motionDataConfiguration.userConfiguration.user = "testUser";
	ccfg.motionDataConfiguration.userConfiguration.password = "testUser";

	typedef hmdbCommunication::ContextConfigurationSettingsFile ConfFile;
	auto iniPath = plugin::getUserApplicationDataPath("medusa_communication.ini").string();
	PLUGIN_LOG_INFO("Communication configuration file: " << iniPath);
	ccfg = ConfFile::read(QString::fromStdString(iniPath), ccfg);
	return ccfg;
}

