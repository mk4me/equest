///********************************************************************
//	created:	2011/10/21
//	created:	21:10:2011   11:09
//	filename: 	AnalisisWidget.h
//	author:		Wojciech Kniec
//	
//	purpose:	
//*********************************************************************/

#ifndef HEADER_GUARD_HMM__ANALISISWIDGET_H__
#define HEADER_GUARD_HMM__ANALISISWIDGET_H__

#include <list>
#include <coreui/CoreDockWidgetManager.h>
#include <coreui/CoreDockWidget.h>
#include <coreui/CoreTitleBar.h>
#include <coreui/CoreVisualizerWidget.h>
#include <coreui/DataFilterWidget.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/Visualizer.h>
#include "ui_AnalisisWidget.h"
#include "AnalisisModel.h"
#include <coreui/IAppUsageContext.h>
#include "SummaryWindow.h"
#include "VisualizerEventFilter.h"

class AnalisisTreeWidget;
class AnalysisTreeContextMenu;
class QSortFilterProxyModel;
class HelperAction;

//! Klasa jest odpowiedzialna za widok zakładki analiz
class AnalisisWidget : public QWidget, private Ui::AnalisisWidget
{
    friend class AnalysisTreeContextMenu;
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param parent rodzic widgeta
    //! \param hmm obiekt widoku HMM 
    //! \param margin margines dla filtrów
    //! \param flags flagi Qt
    AnalisisWidget(AnalisisModelPtr model, ContextEventFilterPtr contextEventFilter, QWidget* parent,
		int margin = 2, Qt::WindowFlags flags = 0);
    virtual ~AnalisisWidget() {}

public:
    //! \return obszar, gdzie laduja wizualizatory i timeline
    QWidget* getArea() { return analisisArea; }
    QTreeView* getTreeView() { return treeView; }
    void registerVisualizerContext(ContextEventFilterPtr contextEventFilter, coreUI::CoreTitleBar * titleBar, coreUI::CoreVisualizerWidget* visualizerDockWidget, const core::VisualizerPtr & visualizer );
    void setContextItems( coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget * flexiTabWidget );
    ContextEventFilterPtr getContextEventFilter() const { return contextEventFilter; }
public Q_SLOTS:
    void createVisualizer( core::IHierarchyDataItemConstPtr treeItem, core::HierarchyHelperPtr helper);
    void addRoot(core::IHierarchyItemPtr root);
    void switchToFirstTab();
    void createNewVisualizer();

Q_SIGNALS:
	void hideCommunication(); 

private:
    void showTimeline();
    void devideArea();
    QDockWidget* createDockVisualizer(const core::VisualizerPtr & visualizer);
    QDockWidget* createAndAddDockVisualizer( core::IHierarchyDataItemConstPtr treeItem, core::HierarchyHelperPtr helper, coreUI::CoreDockWidgetSet* dockSet);
    QDockWidget* createAndAddDockVisualizer( core::HierarchyHelperPtr helper, coreUI::CoreDockWidgetSet* dockSet, QString &path );

    //! dodaje widget z filtrami
    //! \param filter dodawany widget
    void addDataFilterWidget(coreUI::DataFilterWidget* filter);
    void removeFromVisualizers(HelperAction* action, bool once);
    
private Q_SLOTS:

    void closeSet(int idx);
    void visualizerDestroyed(QObject * visualizer);
    //void onFilterBundleAdded(core::IFilterBundlePtr bundle);
    //void onFilterClicked(core::IFilterCommandPtr filter);
    //void addToReports(const QPixmap& pixmap); 
    //! dodaje serie danych z helpera do wybranego wizualizatora
    void addToVisualizer();
    ////! zmienia widoczność filtrów
    ////! \param expand chowa/pokazuje filtry
    //void setFiltersExpanded(bool expand);
    ////! kliknięto w filtr
    ////! \param filter kliknięty
    //void filterClicked(FilterEntryWidget* filter);
    //! Kliknięto zatwierdź w graficznym konfiguratorze
    //void applyClicked();
    ////! proste przejście do pierwszej zakładki (z drzewem danych)
    //void switchToFirstTab();
    ////! odtwarza drzewo danych 
    ////! \param currentFilter filtr, na podstawie którego odtworzone zostanie drzewo
    //void recreateTree(FilterEntryWidget* currentFilter);
    //void onBundleActivated(coreUI::DataFilterWidget* widget);
    void createVisualizerInNewSet();

    void removeFromAll();
    void removeFromVisualizer();

    void onTreeItemActivated(const QModelIndex&);
	void onTreeItemDoubleClicked(const QModelIndex&);
    void highlightVisualizer( core::VisualizerPtr param1 );
    void onVisualizerFocus(QWidget* w);
private:
    coreUI::CoreDockWidgetManager* topMainWindow;
    QFrame* bottomMainWindow;
    AnalisisModelPtr model;
    AnalysisTreeContextMenu* contextMenu;
    coreUI::IAppUsageContextManager* manager;
    coreUI::IAppUsageContextPtr parent;
    QTabWidget * flexiTabWidget;
    QSortFilterProxyModel *proxyModel;
    ////! szerokość filtru w zakładce
    //int filterWidth;
    ////! wysokość filtru w zakładce
    //int filterHeight;
    //! margines dla filtrów w zakładce
    int margin;
    // potrzebne tylko, aby przekazac info między elementami.
    //core::IFilterCommandPtr currentFilter;
    //std::list<coreUI::DataFilterWidget*> filterBundleWidgets;
    SummaryWindowPtr summary;
    SummaryWindowController* summaryController;
    ContextEventFilterPtr contextEventFilter;
    VisualizerEventFilterPtr visualizerFilter;

	struct HelperInnerDescription
	{
		coreUI::CoreDockWidgetSet* dockSet;
		QDockWidget * visualizerDockWidget;
		coreUI::CoreVisualizerWidget* visualizer;
	};

	std::map<core::HierarchyHelperPtr, std::list<HelperInnerDescription>> activeHelpers; 
};


//! Akcja przekazuje informację o konkretnie wybranym helperze
class HelperAction : public QAction 
{
    Q_OBJECT;
public:
    HelperAction(core::HierarchyHelperPtr helper, const QString& text, const core::VisualizerPtr& visualizer = core::VisualizerPtr(), 
        const QIcon& icon = QIcon(), coreUI::CoreDockWidgetSet* set = nullptr, QObject* parent = nullptr) :
        QAction(icon, text, parent),
        helper(helper), 
        visualizer(visualizer),
        set(set)
    {} 

    core::VisualizerPtr getVisualizer() const { return visualizer.lock(); }
    core::HierarchyHelperPtr getHelper() const { return helper; }
    //! \return set, do którego ma trafic wizualizator lub nullptr jeśli nie jest to sprecyzowane
    coreUI::CoreDockWidgetSet* getDockSet() const { return set; }

private:
    core::HierarchyHelperPtr helper;
    core::VisualizerWeakPtr visualizer;
    //! set, do którego ma trafic wizualizator lub nullptr jeśli nie jest to sprecyzowane
    coreUI::CoreDockWidgetSet* set;
};

class TabBarMouseFilter : public QObject
{
	Q_OBJECT;
public:
	TabBarMouseFilter(AnalisisWidget* aw) : aw(aw), QObject(aw) {}
protected:
	bool eventFilter(QObject *obj, QEvent *event);
private:
	AnalisisWidget* aw;
};


//! Pomocnicza klasa obsługująca menu kontekstowe modelu drzewa
//! Elementy drzewa tworzone są z wykorzystaniem "helperów" podpiętych pod element hierarchii danych
class AnalysisTreeContextMenu : public QObject
{
    Q_OBJECT;
public:
    //! 
    //! \param model 
    //! \param widget 
    AnalysisTreeContextMenu(AnalisisModelPtr model, AnalisisWidget* widget);

public:
    void createMenu( QModelIndex index, QMenu * menu );
    void createMenu( core::IHierarchyItemConstPtr item, QMenu * menu );

Q_SIGNALS:
    //! sygnał przekazuje potrzebne dane, do utworzenia wizualizatora.
    void createVisualizer(core::IHierarchyDataItemConstPtr item, core::HierarchyHelperPtr helper);

public Q_SLOTS:
    //! 
    //! \param clickedPoint 
    void contextualMenu(const QPoint& clickedPoint);


    void addHelperToMenu(const core::HierarchyHelperPtr& helper, std::map<QString, QMenu*>& submenus, QMenu * menu );

private Q_SLOTS:
    //! wywoływane, gdy wybrany został helper
    void onCreateVisualizer();
    void menuHighlightVisualizer(QAction* action = nullptr);

private:
    void addAdditionMenuSection( QMenu * menu, const core::HierarchyHelperPtr& helper );
    void addRemovalMenuSection( QMenu * menu, const core::HierarchyHelperPtr& helper );
    void addCreationMenuSection( QMenu * menu, const core::HierarchyHelperPtr& helper );
    
private:
    //! Model analiz, potrzebny do pobrania danych z hierarchii
    AnalisisModelPtr model;
    //! Widget analiz, przekazuje informacje o kliknięciach 
	AnalisisWidget* widget;
};

#endif
