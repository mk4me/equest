/********************************************************************
	created:	2013/04/18
	created:	18:4:2013   18:38
	filename: 	AnalisisModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALISISMODEL_H__
#define HEADER_GUARD_MDE_VIEW__ANALISISMODEL_H__

#include <QtCore/QObject>
#include <coreui/HierarchyTreeModel.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IFilterProvider.h>
#include <corelib/IFilterBundle.h>
#include "ContextEventFilter.h"
#include <coreui/CoreVisualizerWidget.h>
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>
#include <corelib/IDataManager.h>

class QDockWidget;

class AnalisisModel : public QObject, public core::IDataHierarchyManagerReader::IObserver, public core::Visualizer::IObserver
{
    Q_OBJECT
public:
    //! służy do przechowywania informacji o utworzonych wizualizatorach i ich seriach, używane w mapie itemHelper -> itemDesc
    struct DataItemDescription 
    {
        DataItemDescription() {}

		virtual ~DataItemDescription() {}

        //! Konstruktor wypełnia wszystkie pola struktury 
        //! \param visualizer 
        //! \param series 
        //! \param widget 
        DataItemDescription(coreUI::CoreVisualizerWidget* widget, QDockWidget * dockWidget);
        //! widget, w który reprezentuje wizualizator
        coreUI::CoreVisualizerWidget* visualizerWidget;

        QDockWidget * visualizerDockWidget;

        std::string path;
    };
    DEFINE_SMART_POINTERS(DataItemDescription)

		//! służy do przechowywania informacji o utworzonych wizualizatorach i ich seriach, używane w mapie itemHelper -> itemDesc
	struct TimeDataItemDescription : public DataItemDescription
	{
		TimeDataItemDescription() {}

		//! Konstruktor wypełnia wszystkie pola struktury 
		//! \param visualizer 
		//! \param series 
		//! \param widget 
		TimeDataItemDescription(coreUI::CoreVisualizerWidget* widget, QDockWidget * dockWidget)
			: DataItemDescription(widget, dockWidget) {}

		utils::shared_ptr<VisualizerSerieTimelineMultiChannel> channel;		
	};
	DEFINE_SMART_POINTERS(TimeDataItemDescription)

public:
    AnalisisModel();
	virtual ~AnalisisModel() {}

public:
    virtual void observe(const core::IDataHierarchyManagerReader::ChangeList & changes);
    coreUI::HierarchyTreeModel* getTreeModel() { return &model; }
    void addFilterBundles( const core::IFilterProvider::FilterBundles& bundles );
    void applyFilter( core::IFilterCommandPtr filter );
    void addVisualizerDataDescription( core::HierarchyHelperPtr helper, AnalisisModel::DataItemDescriptionPtr desc );
    std::list<DataItemDescriptionConstPtr> getVisualizerDataDescriptions( const core::HierarchyHelperPtr& helper );
    DataItemDescriptionConstPtr getVisualizerDataDescription(const core::VisualizerPtr& visualizer);
    core::HierarchyHelperPtr getHelper(const DataItemDescriptionConstPtr& desc);

    virtual void update(core::Visualizer::Serie * serie, core::Visualizer::SerieModyfication modyfication );
    void addSeriesToVisualizer(core::VisualizerPtr visualizer, core::HierarchyHelperPtr helper, const QString &path, QDockWidget * visualizerDockWidget );

Q_SIGNALS:
    void filterBundleAdded(core::IFilterBundlePtr);
    void reportCreated(const QString& html);
    void expandTree(int);
    void dataAdded();

private:
    // TODO : przyda sie madrzejszy sposob zarzadzania elementami przefiltrowanymi i nieprzefiltrowanymi 
    void saveSourceItems();
    void loadSourceItems();

private:
    coreUI::HierarchyTreeModel model;
    core::IHierarchyItemPtr dataManagerTreeItem; 
    core::IFilterProvider::FilterBundles filters;
    std::vector<core::IHierarchyItemConstPtr> itms;
    //! struktura z informacjami o stworzonych wizualizatorach, ich seriach oraz z którego elementu powstały
    std::multimap<core::HierarchyHelperWeakPtr, DataItemDescriptionPtr> items2Descriptions;
    std::map<core::HierarchyHelperWeakPtr, core::IHierarchyDataItemConstWeakPtr> helper2hierarchyItem;

    std::map<core::Visualizer::Serie*, std::string> seriesToPaths;
};
DEFINE_SMART_POINTERS(AnalisisModel);

#endif
