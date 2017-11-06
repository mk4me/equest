#include "MdePCH.h"
#include "AnalisisWidget.h"
#include <corelib/IServiceManager.h>
#include <corelib/IIdentifiable.h>
#include <corelib/PluginCommon.h>
#include <corelib/BaseDataTypes.h>
#include <coreui/CoreVisualizerWidget.h>
#include <coreui/CoreDockWidgetSet.h>
#include <coreui/CoreVisualizerWidget.h>
#include <corelib/HierarchyHelper.h>
#include <coreui/HierarchyTreeModel.h>
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>
#include <corelib/IHierarchyItem.h>
//#include <plugins/newVdf/NewVdfService.h>
#include <coreui/CoreTitleBar.h>
#include <QtWidgets/QMenu>
#include <coreui/ContextAutoPlacer.h>
#include <coreui/DataFilterWidget.h>
#include <coreui/ReportsThumbnailContext.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <coreui/HierarchyTreeModel.h>
#include <QtWidgets/QMessageBox>
#include <QtCore/QBuffer>
#include "AnalysisTreeContext.h"
#include <coreui/SimpleContext.h>
//#include "WaitingWidget.h"
#include "corelib/IHierarchyProvider.h"
#include "corelib/IDataHierarchyManager.h"
//#include "SummaryWindow.h"
//#include "AnalisisTreeWidget.h"



AnalisisWidget::AnalisisWidget( AnalisisModelPtr model, ContextEventFilterPtr contextEventFilter, QWidget* parent,
	int margin /*= 2*/, Qt::WindowFlags flags /*= 0*/ ) : 
    QWidget(parent, flags),
    contextEventFilter(contextEventFilter),
    model(model),
    manager(nullptr),
    flexiTabWidget(nullptr),
    margin(margin),
    filterWidth(-1), 
    filterHeight(-1),
    visualizerFilter(new VisualizerEventFilter(contextEventFilter))
{
    setupUi(this);
    devideArea();
    showTimeline();
    treeView->setModel(model->getTreeModel());
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    
	contextMenu = new AnalysisTreeContextMenu(model, this);
    connect(treeView, SIGNAL(customContextMenuRequested(const QPoint &)), contextMenu ,SLOT(contextualMenu(const QPoint &)));
    connect(treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onTreeItemActivated(const QModelIndex&)));
    connect(model.get(), SIGNAL(expandTree(int)), treeView, SLOT(expandToDepth(int)));
    connect(model.get(), SIGNAL(dataAdded()), this, SIGNAL(dataAdded()));
    connect(contextMenu, SIGNAL(createVisualizer(core::IHierarchyDataItemConstPtr, core::HierarchyHelperPtr)), this, SLOT(createVisualizer(core::IHierarchyDataItemConstPtr, core::HierarchyHelperPtr)));
    connect(visualizerFilter.get(), SIGNAL(focusOn(QWidget*)), this, SLOT(onVisualizerFocus(QWidget*)));
    
    //auto vdfService = core::queryServices<vdf::NewVdfService>(plugin::getServiceManager());
    //connect(vdfService.get(), SIGNAL(transferResults()), this, SLOT(addRoot(core::IHierarchyItemPtr)));
    connect(model.get(), SIGNAL(filterBundleAdded(core::IFilterBundlePtr)), this, SLOT(onFilterBundleAdded(core::IFilterBundlePtr)));

    connect(applyButton, SIGNAL(clicked()), this, SLOT(applyClicked()));
    
    summary = utils::make_shared<SummaryWindow>(SummaryWindowWidget);
    summaryController = new SummaryWindowController(summary, model);
    summary->initialize();

    initReportsCombos();
}

void AnalisisWidget::showTimeline()
{
    auto timeline = plugin::getServiceManager()->getService(core::UID::GenerateUniqueID("{0157346E-D1F3-4A4F-854F-37C87FA3E5F9}"));
    if (timeline) {
        QWidget* controlWidget = timeline->getWidget();
        dynamic_cast<QVBoxLayout*>(bottomMainWindow->layout())->addStretch();
        bottomMainWindow->layout()->addWidget(controlWidget);
        dynamic_cast<QVBoxLayout*>(bottomMainWindow->layout())->addStretch();
    }
}

void AnalisisWidget::devideArea()
{
    topMainWindow = new coreUI::CoreDockWidgetManager();
    topMainWindow->setSetsClosable(true);
    topMainWindow->setTabPosition(QTabWidget::South);
    topMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bottomMainWindow = new QFrame();
    bottomMainWindow->setObjectName(QString("bottomMainWindow"));
    bottomMainWindow->setLayout(new QVBoxLayout);
    bottomMainWindow->layout()->setContentsMargins(0,0,0,0);
    bottomMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QWidget* analisisArea = getArea();
    QLayout* v = analisisArea->layout() ? analisisArea->layout() : new QGridLayout(analisisArea);

    v->setMargin(0);
    v->setContentsMargins(QMargins(0, 0, 0, 0));
    v->addWidget(topMainWindow);
    topMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    v->addWidget(bottomMainWindow);
    //TODO
    //zapewniamy timelie minimalną, ale wystarczająca ilośc miejsca
    //trzeba to zgrać razem z max wysokością dla wizualizatorów
    bottomMainWindow->setMaximumHeight(120); // tymczasowo
    bottomMainWindow->setMinimumHeight(120); // tymczasowo
    analisisArea->setLayout(v);
}

void AnalisisWidget::createVisualizer( core::IHierarchyDataItemConstPtr treeItem, core::HierarchyHelperPtr helper )
{
    createAndAddDockVisualizer(treeItem, helper, nullptr);
}

QDockWidget* AnalisisWidget::createDockVisualizer(const core::VisualizerPtr & visualizer, const QString& titleName)
{
    connect(visualizer.get(), SIGNAL(screenshotTaken(const QPixmap&)), this, SLOT(addToReports(const QPixmap&)));

    auto visWidget = new coreUI::CoreVisualizerWidget(visualizer);
    //visWidget->show();

    auto dockVisWidget = coreUI::CoreDockWidget::embeddWidget(visWidget, titleName, Qt::AllDockWidgetAreas, false);

    registerVisualizerContext(getContextEventFilter(), qobject_cast<coreUI::CoreTitleBar*>(dockVisWidget->titleBarWidget()), qobject_cast<coreUI::CoreVisualizerWidget*>(dockVisWidget->widget()), visualizer);
    dockVisWidget->setMinimumSize((std::max)(80, dockVisWidget->minimumWidth()), (std::max)(80, dockVisWidget->minimumHeight()));
    return dockVisWidget;
}

void AnalisisWidget::registerVisualizerContext(ContextEventFilterPtr contextEventFilter, coreUI::CoreTitleBar * titleBar, coreUI::CoreVisualizerWidget* visualizerDockWidget, const core::VisualizerPtr & visualizer )
{
    this->contextEventFilter = contextEventFilter;
    contextEventFilter->registerClosableContextWidget(titleBar);
    titleBar->installEventFilter(contextEventFilter.get());
    contextEventFilter->registerClosableContextWidget(visualizerDockWidget);
    visualizerDockWidget->installEventFilter(visualizerFilter.get());

    auto visWidget = visualizer->getWidget();

    contextEventFilter->registerClosableContextWidget(visWidget);
    visWidget->installEventFilter(visualizerFilter.get());

    connect(visualizerDockWidget, SIGNAL(destroyed(QObject *)), this, SLOT(visualizerDestroyed(QObject *)));

    //kontekst wizualizatora!!
    coreUI::SimpleContextPtr visualizerUsageContext(new coreUI::SimpleContext(flexiTabWidget, tr("Visualizer"), QStringList(tr("Settings"))));
    manager->addContext(visualizerUsageContext, parent);
    manager->addWidgetToContext(visualizerUsageContext, visualizerDockWidget);
    manager->addWidgetToContext(visualizerUsageContext, titleBar);
    manager->addWidgetToContext(visualizerUsageContext, visWidget);
}

void AnalisisWidget::visualizerDestroyed(QObject * visualizer)
{
    auto w = qobject_cast<QWidget*>(visualizer);
    //visualizerUsageContext->setCurrentContextWidgetDestroyed(true);
    manager->removeContext(w);
    //visualizerUsageContext->setCurrentContextWidgetDestroyed(false);

    contextEventFilter->unregisterClosableContextWidget(w);
    /*auto it = derrivedContextWidgets.right.find(w);
    while( it != derrivedContextWidgets.right.end() && it->first == w){
        contextEventFilter->unregisterClosableContextWidget(it->second);
        ++it;
    }
    derrivedContextWidgets.right.erase(w);
    plainContextWidgets.erase(w);*/
}


QDockWidget* AnalisisWidget::createAndAddDockVisualizer( core::IHierarchyDataItemConstPtr treeItem, core::HierarchyHelperPtr helper, coreUI::CoreDockWidgetSet* dockSet)
{
	auto path = createPathDesc(treeItem);
	return createAndAddDockVisualizer(helper, dockSet, path);
}

class ProviderProxy : public core::IHierarchyProviderProxy
{
public:
	ProviderProxy(core::IHierarchyItemPtr parent) :
		_parent(parent)
	{}

	virtual void appendToHierarchy(core::IHierarchyItemPtr item)
	{
		// hack - jak dostac sie do IDataHierarchyManager prawidlowo?
		auto manager = dynamic_cast<core::IDataHierarchyManager*>(plugin::getDataHierarchyManagerReader());
		auto hierarchyTransaction = manager->transaction();
		core::IHierarchyItemConstPtr root = _parent;
		while (root->getParent()) { root = root->getParent(); }

		_parent->appendChild(item);
		hierarchyTransaction->updateRoot(root);
	}

private:
	core::IHierarchyItemPtr _parent;
};

core::IHierarchyItemConstPtr getParentByHelper(core::IHierarchyItemConstPtr root, core::HierarchyHelperPtr helper)
{
	auto dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(root);
	if (dataItem) {
		auto& helpers = dataItem->getHelpers();
		for (auto it : helpers) {
			if (it == helper) {
				return root;
			}
		}
	}
	auto count = root->getNumChildren();
	for (auto i = 0; i < count; ++i) {
		auto parent = getParentByHelper(root->getChild(i), helper);
		if (parent) {
			return parent;
		}
	}
	return core::IHierarchyItemConstPtr();
}

QDockWidget* AnalisisWidget::createAndAddDockVisualizer( core::HierarchyHelperPtr helper, coreUI::CoreDockWidgetSet* dockSet, const QString &path )
{
    core::VisualizerPtr visualizer = helper->createVisualizer(plugin::getVisualizerManager());
    visualizer->addObserver(this->model.get());
	auto visW = visualizer->visualizer();
	core::IHierarchyProvider* provider = dynamic_cast<core::IHierarchyProvider*>(visW);
	if (provider) {
		core::IHierarchyItemPtr parent;// = utils::const_pointer_cast<core::IHierarchyItem>(helper->getParent().lock());
		auto manager = plugin::getDataHierarchyManagerReader();
		auto t = manager->transaction();
		for (auto it = t->hierarchyBegin(); it != t->hierarchyEnd(); ++it) {
			core::IHierarchyItemConstPtr root = *it;
			auto parent = getParentByHelper(*it, helper);
			if (parent) {
				provider->initHierarchyProvider(utils::make_shared<ProviderProxy>(utils::const_pointer_cast<core::IHierarchyItem>(parent)));
			}
		}
		//provider->initHierarchyProvider(utils::make_shared<ProviderProxy>(parent));
	}
    QDockWidget* visualizerDockWidget = createDockVisualizer(visualizer, path);
	
    if (dockSet) {
        dockSet->addDockWidget(visualizerDockWidget);
    } else {
        topMainWindow->autoAddDockWidget( visualizerDockWidget, tr("Group %1").arg(topMainWindow->count()+1) );
    }
	
    model->addSeriesToVisualizer(visualizer, helper, path, visualizerDockWidget);
    return visualizerDockWidget;

	//--------------------------------	
	//core::VisualizerPtr visualizer = helper->createVisualizer(plugin::getVisualizerManager());
	//visualizer->addObserver(this->model.get());
	//auto visW = visualizer->visualizer();
	//core::IHierarchyProvider* provider = dynamic_cast<core::IHierarchyProvider*>(visW);
	//if (provider) {
	//	core::IHierarchyItemPtr parent = utils::const_pointer_cast<core::IHierarchyItem>(helper->getParent().lock());
	//	provider->initHierarchyProvider(parent);
	//}
	//QDockWidget* visualizerDockWidget = createDockVisualizer(visualizer, path);
	//auto fun = [=]() -> void {
	//	model->addSeriesToVisualizer(visualizer, helper, path, visualizerDockWidget);
	//};
	//auto dck = new QDockWidget("DA");
	//dck->setWidget(new WaitingWidget(visualizerDockWidget, fun));
	//if (dockSet) {
	//    dockSet->addDockWidget(dck);
	//} else {
	//    topMainWindow->autoAddDockWidget( dck, tr("Group %1").arg(topMainWindow->count()+1) );
	//}
	//return dck;
}

void AnalisisWidget::setContextItems( coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget * flexiTabWidget )
{
    this->manager = manager;
    this->parent = parent;
    this->flexiTabWidget = flexiTabWidget;

    //ReportsThumbnailContextPtr visualizerUsageContext(new ReportsThumbnailContext(flexiTabWidget, raportsArea));
    //manager->addContext(visualizerUsageContext, parent);
    //this->raportsArea->addAction(new QAction(tr("Create report"), this));
    //getContextEventFilter()->registerPermamentContextWidget(this->raportsArea);
    //this->raportsArea->installEventFilter(getContextEventFilter().get());
    //manager->addWidgetToContext(visualizerUsageContext, this->raportsArea);
    //connect(visualizerUsageContext.get(), SIGNAL(reportCreated(const QString&)), model.get(), SIGNAL(reportCreated(const QString&)));
    connect(createReportButton, SIGNAL(clicked()), this, SLOT(createReportClicked()));
    connect(this, SIGNAL(reportCreated(const QString&)), model.get(), SIGNAL(reportCreated(const QString&)));
    AnalysisTreeContextPtr treeContext = utils::make_shared<AnalysisTreeContext>(flexiTabWidget, model->getTreeModel(), contextMenu);
    manager->addContext(treeContext, parent);
    getContextEventFilter()->registerPermamentContextWidget(treeView);
    this->treeView->installEventFilter(getContextEventFilter().get());
    manager->addWidgetToContext(treeContext, treeView);
}

//void AnalisisWidget::addRoot( core::IHierarchyItemPtr root )
//{
//    model->getTreeModel()->addRootItem(root);
//    emit dataAdded();
//}

void AnalisisWidget::onFilterBundleAdded( core::IFilterBundlePtr bundle )
{
    coreUI::DataFilterWidget* dataWidget = new coreUI::DataFilterWidget(bundle->getName(), bundle->getIcon());
    int count = bundle->genNumFilters();
    for (int i = 0; i < count; ++i) {
        auto filter = bundle->getFilter(i);
        dataWidget->addFilter(filter->getName(), filter, filter->getIcon());
    }
    addDataFilterWidget(dataWidget);
    connect(dataWidget, SIGNAL(onFilterClicked(core::IFilterCommandPtr)), this, SLOT(onFilterClicked(core::IFilterCommandPtr)));
}


void AnalisisWidget::addDataFilterWidget( coreUI::DataFilterWidget* filter )
{
    filterBundleWidgets.push_back(filter);
    connect(filter, SIGNAL(activityChanged(coreUI::DataFilterWidget*)), this, SLOT(onBundleActivated(coreUI::DataFilterWidget*)));
    if (filterWidth < 0 && filterHeight < 0) {
        filterWidth = filter->width();
        filterHeight = filter->height();
    }

    UTILS_ASSERT(filterHeight == filter->height() && filterWidth == filter->width());


    int count = filterScroll->children().size();
    int x = count % 2;
    int y = count / 2;

    if (x == 0) {
        int w = 3 * margin + filterWidth * 2;
        int h = 2 * margin + (filterHeight + margin) * (y + 1);
        filterScroll->setMinimumSize(w, h);
        containerFrame->setMaximumWidth(w + 22);
        containerFrame->setMinimumWidth(w + 22);
        scrollArea->setMinimumHeight(3 * margin + filterHeight * 2);
    }

    filter->setParent(filterScroll);
    filter->setGeometry(margin + x * (filterWidth + margin),margin +  y * (margin + filterHeight), filterWidth, filterHeight);

    //connect(filter, SIGNAL( clicked()), this, SLOT(switchToFirstTab()));
    connect(this->resetButton, SIGNAL(clicked()), filter, SLOT(resetFilters()));
    //connect(this->resetButton, SIGNAL(clicked()), hmm, SLOT(refreshTree()));
    //for (int i = 0; i < filter->getNumEntries(); ++i) {
    //    connect(filter->getEntry(i), SIGNAL( onFilterClicked(core::IFilterCommandPtr)), this, SLOT(onFilterClicked(core::IFilterCommandPtr)));
    //}
}

void AnalisisWidget::switchToFirstTab()
{
    // Hack - to rozwiazanie nie jest odporne na zmiane kolejności tabów
    tabWidget->setCurrentIndex(0);
}

void AnalisisWidget::applyClicked() 
{
    if (currentFilter) {
        model->applyFilter(currentFilter);
        switchToFirstTab();
    }
}

void AnalisisWidget::onFilterClicked( core::IFilterCommandPtr filter )
{
    auto conf = filter ? filter->getConfigurationWidget() : nullptr;
    if (conf) {
        currentFilter = filter;
        if (!configurationWidget->layout()) {
            configurationWidget->setLayout(new QVBoxLayout());
        }
        bool hasChild = false;
        const QObjectList& childList = configurationWidget->children();
        for (int i = childList.size() - 1; i >= 0; --i) {
            QWidget* w = qobject_cast<QWidget*>(childList.at(i));
            if (w) {
                w->hide();
                if (w == conf) {
                    hasChild = true;
                }
            }
        }
        if (!hasChild) {
            configurationWidget->layout()->addWidget(conf);
        }
        conf->setVisible(true);
        conf->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);\
        applyButton->setVisible(true);
        applyButton->setText(QString("%1 - Apply").arg(filter->getName()));
        
    } else {
        model->applyFilter(filter);
        switchToFirstTab();
    }
}

void AnalisisWidget::addToReports( const QPixmap& pixmap )
{
    if (!(pixmap.width() && pixmap.height())) {
        return;
    }

    tabWidget->setCurrentWidget(raportsTab);
    const int maxH = 128;
    const int maxW = 128;
    QWidget* list = raportsArea;
    QLabel* thumb = new QLabel();

    if (pixmap.width() > pixmap.height()) {
        int newHeight = static_cast<int>(1.0 * maxH * pixmap.height() / pixmap.width());
        thumb->setFixedSize(maxW, newHeight);
    } else {
        int newWidth = static_cast<int>(1.0 * maxW * pixmap.width() / pixmap.height());
        thumb->setFixedSize(newWidth, maxH);
    }

    thumb->setScaledContents(true);
    thumb->setPixmap(pixmap);

    QGridLayout* grid = qobject_cast<QGridLayout*>(list->layout());
    if (grid) {
        int size = list->children().size() - 1;
        int x = size % 2;
        int y = size / 2;
        grid->addWidget(thumb, y, x);
    } else {
        list->layout()->addWidget(thumb);
    }
}

void AnalisisWidget::onBundleActivated( coreUI::DataFilterWidget* widget )
{
    for (auto it = filterBundleWidgets.begin(); it != filterBundleWidgets.end(); ++it) {
        coreUI::DataFilterWidget* dfw = *it;
        if (dfw != widget) {
            dfw->blockSignals(true);
            dfw->setActive(false);
            dfw->blockSignals(false);
        }
    }
}



AnalysisTreeContextMenu::AnalysisTreeContextMenu( AnalisisModelPtr model, AnalisisWidget* widget) :
model(model),
    widget(widget)
{

}


void AnalysisTreeContextMenu::contextualMenu( const QPoint &clickedPoint )
{
    QMenu *menu = new QMenu;
    QTreeView* view = widget->getTreeView();
    UTILS_ASSERT(view);

    //QModelIndex index = view->indexAt(clickedPoint);
    QModelIndex index = view->currentIndex();
    createMenu(index, menu);


    menu->exec(QCursor::pos());
}

void AnalysisTreeContextMenu::onCreateVisualizer()
{
    HelperAction* ha = qobject_cast<HelperAction*>(sender());
    UTILS_ASSERT(ha);
    QModelIndex index = widget->getTreeView()->currentIndex();
    core::IHierarchyItemConstPtr item = model->getTreeModel()->internalSmart(index);
    core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
    UTILS_ASSERT(dataItem);
    emit createVisualizer(dataItem, ha->getHelper());
}

void AnalysisTreeContextMenu::addHelperToMenu( const core::HierarchyHelperPtr& helper, std::map<QString, QMenu*>& submenus, QMenu * menu )
{
    auto list = helper->getText().split('/');
    QMenu* submenu = menu;

    for (auto it = list.begin(); it != list.end(); ++it) {
        if (*it == list.back()) {
            HelperAction* ha = new HelperAction(helper, *it);
            submenu->addAction(ha);
            connect(ha, SIGNAL(triggered()), this, SLOT(onCreateVisualizer()));
        } else {
            auto prevIt = submenus.find(*it);
            if (prevIt == submenus.end()) {
                submenus[*it] = submenu = submenu->addMenu(*it);
            } else {
                submenu = prevIt->second;
            }
        }
    }
}

void AnalysisTreeContextMenu::addAdditionMenuSection( QMenu * menu, const core::HierarchyHelperPtr& helper)
{
    QMenu* addTo = new QMenu(tr("Add to:"), menu);
    addTo->setEnabled(false);
    connect(addTo, SIGNAL(aboutToHide()), this, SLOT(menuHighlightVisualizer()));
    connect(addTo, SIGNAL(hovered(QAction*)), this, SLOT(menuHighlightVisualizer(QAction*)));
	auto dtr = plugin::getRegisteredDataTypesManagerReader();
    for(auto set : widget->topMainWindow->getDockSet()) {
        QMenu* group = new QMenu(widget->topMainWindow->setText(widget->topMainWindow->indexOf(set)), menu);
        for(auto dock : set->getDockWidgets()) {
            coreUI::CoreVisualizerWidget* vw = dynamic_cast<coreUI::CoreVisualizerWidget*>(dock->widget());
            if (vw ) {
                core::VisualizerPtr visualizer = vw->getVisualizer();
                bool compatibile = false;
                utils::TypeInfoSet supportedTypes;
                visualizer->getSupportedTypes(supportedTypes);

                std::vector<utils::TypeInfo> types = helper->getTypeInfos();
                for (unsigned int h = 0; h < types.size(); ++h) {
                    if(supportedTypes.find(types[h]) != supportedTypes.end()){
                        compatibile = true;
                    }else {
                        for(auto it = supportedTypes.begin(); it != supportedTypes.end(); ++it){
                            //if(hierarchyManager->isTypeCompatible(*it, types[h]) == true){
							if (dtr->isDerrived(*it, types[h]) == true){
                                compatibile = true;
                                break;
                            }
                        }
                    }

                    if(compatibile == true){
                        break;
                    }
                }

                if (compatibile) {
                    int maxSeries = visualizer->getMaxSeries();
                    if (maxSeries == -1 || maxSeries > static_cast<int>(visualizer->getNumSeries())) {
                        // TODO
                        HelperAction* addAction = new HelperAction(helper, QString::fromStdString(visualizer->getName()), visualizer);
                        //QAction* addAction = new ContextAction(item, group, visualizer);
                        //addAction->setText(QString::fromStdString(visualizer->getName()));
                        connect(addAction, SIGNAL(triggered()), widget, SLOT(addToVisualizer()));
                        //connect(addAction, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));
                        group->addAction(addAction);
                    }
                }
            }
        }

        if (group->actions().size()) {
            addTo->addMenu(group);
        } else  {
            delete group;
        }
    }

    menu->addMenu(addTo);

    if (addTo->actions().size()) {
        addTo->setEnabled(true);
    }
}


void AnalisisWidget::addToVisualizer()
{
    HelperAction* action = qobject_cast<HelperAction*>(sender());
    try {
        core::VisualizerPtr visualizer = action->getVisualizer();
        /*INewChartVisualizer* newChart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
        if (newChart) {
            newChart->setTitle(tr("Multichart"));
        }*/

        auto helper = action->getHelper();
        static int counter = 0;
        QString path = QString("Custom_addition...%1").arg(counter++);


        auto visDesc = model->getVisualizerDataDescription(visualizer);

        model->addSeriesToVisualizer(visualizer, helper, path, visDesc->visualizerDockWidget );
    } catch (std::exception& e) {
        QString message("Unable to add data to visualizer");
        message += "\n";
        message += tr("reason: ");
        message += tr(e.what());
        QMessageBox::warning(this, tr("Warning"), message);
    } catch (...) {
        QMessageBox::warning(this, tr("Warning"), tr("Unable to add data to visualizer"));
    }
}


void AnalysisTreeContextMenu::addRemovalMenuSection( QMenu * menu, const core::HierarchyHelperPtr& helper)
{

    QMenu* removeFrom = new QMenu(tr("Remove from:"), menu);

    connect(removeFrom, SIGNAL(aboutToHide()), this, SLOT(menuHighlightVisualizer()));
    connect(removeFrom, SIGNAL(hovered(QAction*)), this, SLOT(menuHighlightVisualizer(QAction*)));

    removeFrom->setEnabled(false);
    menu->addMenu(removeFrom);

    auto descriptions = model->getVisualizerDataDescriptions(helper);
    if(!descriptions.empty()) {
        removeFrom->setEnabled(true);
        connect(removeFrom, SIGNAL(aboutToHide()), this, SLOT(menuHighlightVisualizer()));
        connect(removeFrom, SIGNAL(hovered(QAction*)), this, SLOT(menuHighlightVisualizer(QAction*)));
        for (auto it = descriptions.begin(); it != descriptions.end(); it++) {
            AnalisisModel::DataItemDescriptionConstPtr desc = *it;

            HelperAction* action = new HelperAction(helper,
                QString::fromStdString(desc->visualizerWidget->getVisualizer()->getName()), desc->visualizerWidget->getVisualizer());
            //QAction * action = new ContextAction(item, menu, desc.visualizerWidget->getVisualizer());
            //action->setText(QString::fromStdString(desc.visualizerWidget->getVisualizer()->getName()));

            connect(action, SIGNAL(triggered()), widget, SLOT(removeFromVisualizer()));
            //connect(action, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
            removeFrom->addAction(action);
        }

        //TODO, HACK
        HelperAction* all = new HelperAction(helper, tr("All visualizers"));

        //QAction* all = new ContextAction(item, menu);
        //all->setText("All visualizers");
        connect(all, SIGNAL(triggered()), this, SLOT(removeFromAll()));
        //connect(all, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
        removeFrom->addSeparator();
        removeFrom->addAction(all);
    }

}

void AnalysisTreeContextMenu::addCreationMenuSection( QMenu * menu, const core::HierarchyHelperPtr& helper )
{
    QMenu* createIn = new QMenu(tr("Create in:"), menu);
    createIn->setEnabled(false);
    menu->addMenu(createIn);
    for(auto set : widget->topMainWindow->getDockSet()) {
        if (set->isAdditionPossible()) {
            //TODO, HACK
            HelperAction* action = new HelperAction(helper, 
                tr("Set %1").arg(widget->topMainWindow->indexOf(set)), core::VisualizerPtr(), QIcon(), set);
            /*QAction* action = new ContextAction(item, menu, VisualizerPtr(), set);
            action->setText(topMainWindow->setText(topMainWindow->indexOf(set)));*/
            createIn->addAction(action);
            connect(action, SIGNAL(triggered()), widget, SLOT(createNewVisualizer()));
            //connect(action, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
            //TODO
            //connect(action, SIGNAL(hovered(QAction*)), this, SLOT(menuShowGroup(QAction*)));
        }
    }

    // TODO, HACK
    //QAction* newGroup = new ContextAction(item, menu);
    HelperAction* newGroup = new HelperAction(helper, tr("New Group"));
    //newGroup->setText(tr("New group"));
    createIn->addAction(newGroup);
    connect(newGroup, SIGNAL(triggered()), widget, SLOT(createVisualizerInNewSet()));
//    connect(newGroup, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
    if (createIn->actions().size()) {
        createIn->setEnabled(true);
    }
}

void AnalysisTreeContextMenu::menuHighlightVisualizer( QAction* action /*= nullptr*/ )
{
    HelperAction* context = qobject_cast<HelperAction*>(action);
    if (context) {
        widget->highlightVisualizer(context->getVisualizer());

    } else {
        widget->highlightVisualizer(core::VisualizerPtr());
    }
}

void AnalysisTreeContextMenu::createMenu( QModelIndex index, QMenu * menu )
{
    core::IHierarchyItemConstPtr item = model->getTreeModel()->internalSmart(index);
    createMenu(item, menu);

}

void AnalysisTreeContextMenu::createMenu( core::IHierarchyItemConstPtr item, QMenu * menu )
{
    core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
    if (dataItem) {
        auto helpers = dataItem->getHelpers();
        std::map<QString, QMenu*> submenus;
        for (auto it = helpers.begin(); it != helpers.end(); ++it) {
            addHelperToMenu(*it, submenus, menu);
        }
        addAdditionMenuSection(menu, helpers.empty() ? core::HierarchyHelperPtr() : *helpers.begin());
        addRemovalMenuSection(menu, helpers.empty() ? core::HierarchyHelperPtr() : *helpers.begin());
        addCreationMenuSection(menu, helpers.empty() ? core::HierarchyHelperPtr() : *helpers.begin());
    }
}


void AnalisisWidget::removeFromVisualizers( HelperAction* action, bool once)
{
    UTILS_ASSERT(action);
    if (action) {
        typedef std::list<AnalisisModel::DataItemDescriptionConstPtr> DescList;        
        DescList descriptors = model->getVisualizerDataDescriptions(action->getHelper());

        auto it = descriptors.begin();
        while(it != descriptors.end() ) {
            // na razie kopia, w przeciwnym razie jest problem z usuwaniem.
            AnalisisModel::DataItemDescriptionConstPtr desc = *it;

			AnalisisModel::TimeDataItemDescriptionConstPtr timeDesc = utils::dynamic_pointer_cast<const AnalisisModel::TimeDataItemDescription>(desc);
            // jeśli w akcji nie przechowujemy informacji o konkretnym wizualizatorze
            // to znaczy, ze chcemy usunąć dane z wszystkich wizualizatorw
            if (timeDesc != nullptr && action->getVisualizer() == nullptr || desc->visualizerWidget->getVisualizer() == action->getVisualizer()) {

                //teraz usuwamy serie
                auto channel = timeDesc->channel;
                if (channel) {
                    auto series = channel->getVisualizersSeries();

                    for(auto it = series.begin(); it != series.end(); ++it){
                        auto vis = (*it)->visualizer();
                        vis->destroySerie((*it));					 
                    }

                    if (desc->visualizerWidget->getVisualizer()->getNumSeries() == 0) {
                        desc->visualizerDockWidget->close();
                    }

                    if (once) {
                        break;
                    }
                }
                
                // TODO - czy jest to potrzebne?
                //it = items2Descriptions.find(action->getTreeItem()->getHelper());
            }else{
				//it = items2Descriptions.find(action->getTreeItem()->getHelper());
                ++it;
            }
        }
    }
}


void AnalisisWidget::removeFromAll()
{
    HelperAction* action = qobject_cast<HelperAction*>(sender());
    removeFromVisualizers(action, false);
}

void AnalisisWidget::removeFromVisualizer()
{
    HelperAction* action = qobject_cast<HelperAction*>(sender());
    removeFromVisualizers(action, true);
}

void AnalisisWidget::createVisualizerInNewSet()
{
    static const QString setName = tr("Group %1");
    HelperAction* action = qobject_cast<HelperAction*>(sender());
    coreUI::CoreDockWidgetSet* set = new coreUI::CoreDockWidgetSet(topMainWindow);
    topMainWindow->addDockWidgetSet(set, setName.arg(topMainWindow->count()+1));

	QString str = "Dock";
    createAndAddDockVisualizer(action->getHelper(), set, str);
}

void AnalisisWidget::createNewVisualizer()
{
    HelperAction* action = qobject_cast<HelperAction*>(sender());
    try{
    	QString str = "Dock";
        createAndAddDockVisualizer(action->getHelper(), action->getDockSet(), str);
    }catch(std::exception& e ){
        PLUGIN_LOG_ERROR("Error creating visualizer: " << e.what());
    } catch (...) {
        PLUGIN_LOG_ERROR("Error creating visualizer");
    }
}

void AnalisisWidget::onTreeItemActivated( const QModelIndex& index)
{
    auto item = model->getTreeModel()->internalSmart(index);
    if (item) {
        summary->display(item);
    } else {
        summary->clear();
    }
    
}

void AnalisisWidget::highlightVisualizer( core::VisualizerPtr param1 )
{
    auto docks = topMainWindow->getDockSet();
    for(auto set : docks) {
        for(auto dock : set->getDockWidgets()) {
            coreUI::CoreVisualizerWidget* visualizerWidget = dynamic_cast<coreUI::CoreVisualizerWidget*>(dock->widget());
            if (visualizerWidget) {
                visualizerWidget->setStyleSheet(QString());
            }
        }
    }

    if (param1) {
        AnalisisModel::DataItemDescriptionConstPtr desc = model->getVisualizerDataDescription(param1);
        coreUI::CoreDockWidgetSet* set = topMainWindow->tryGetDockSet(desc->visualizerDockWidget);
        if (set) {
            topMainWindow->setCurrentSet(set);
        }
        desc->visualizerWidget->setStyleSheet(QString::fromUtf8(
            "coreUI--CoreVisualizerWidget {" \
            "border: 2px solid red;"\
            "}"));
    }
}

void AnalisisWidget::onVisualizerFocus( QWidget* w )
{
    QObject* obj = w;
    while (obj) {
        auto vis = qobject_cast<coreUI::CoreVisualizerWidget*>(obj);
        if (vis) {
            summaryController->onVisualizer(vis);
            return;
        }
        obj = obj->parent();
    }
}

void AnalisisWidget::createReportClicked()
{
    const QWidget* thumbParent = this->raportsArea;

    QString templateDir = QString::fromStdString(plugin::getResourcePath("templates\\").string());//QString("C:\\programming\\HMEdr\\src\\edr\\resources\\deploy\\templates\\");
    QString cssFile = templateDir + styleComboBox->currentText();
    QFile f(cssFile);
    QString css;
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        css = f.readAll();
        f.close();
    }


    QString p = templateDir + templateComboBox->currentText();
    QFile file(p);
    QString html;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        html = QString::fromUtf8(file.readAll());
    } 
    emit reportCreated(coreUI::ReportsThumbnailContext::createRaport(thumbParent, reportsTitle->text(), html, css ));
}

void AnalisisWidget::initReportsCombos()
{
    reportsTitle->setText(tr("Simple Report"));
   
    QString dirPath = QString::fromStdString(plugin::getResourcePath("templates").string());
    QStringList filters;
    filters << "*.htm" << "*.html";
    QDir templateDir(dirPath);
    templateDir.setNameFilters(filters);

    templateComboBox->addItem(tr("Empty"));
    templateComboBox->addItems(templateDir.entryList());
   
    QDir cssDir(dirPath, "*.css");
    styleComboBox->addItem(tr("Empty"));
    styleComboBox->addItems(cssDir.entryList());
}

QString AnalisisWidget::createPathDesc(core::IHierarchyDataItemConstPtr treeItem)
{
	std::stack<QString> pathStack;
	core::IHierarchyItemConstPtr pomItem = utils::dynamic_pointer_cast<const core::IHierarchyItem>(treeItem);

	while (pomItem != nullptr) {
		pathStack.push(pomItem->getName());
		pomItem = pomItem->getParent();
	}

	QString path;

	path += pathStack.top();
	pathStack.pop();

	while (pathStack.empty() == false) {
		path += "/";
		path += pathStack.top();
		pathStack.pop();
	}

	static int incr = 0;
	QString suffix = QString("/id_%1").arg(incr++);
	path += suffix;
	return path;
}
