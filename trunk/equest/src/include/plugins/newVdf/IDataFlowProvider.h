/********************************************************************
	created:	2013/02/22
	created:	22:2:2013   10:42
	filename: 	IDataFlowProvider.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IDATAFLOWPROVIDER_H__
#define HEADER_GUARD_NEWVDF__IDATAFLOWPROVIDER_H__

#include <list>
//#include <objectwrapperlib/ObjectWrapper.h>
#include <plugins/newVdf/IWorkflowItemBase.h>
#include <plugins/newVdf/DataProcessor.h>
#include <plugins/newVdf/DataSource.h>
#include <plugins/newVdf/DataSink.h>

namespace df { class IProcessingNode; class ISourceNode; class ISinkNode; };

#define VDF_BEGIN \
    private: std::list<vdf::IWorkflowItemBasePtr> __providerItems;\
    public: virtual void provideDataFlowItems(std::list<vdf::IWorkflowItemBaseWeakPtr>& items) {

#define VDF_END \
    for (auto it = __providerItems.begin(); it != __providerItems.end(); ++it) { items.push_back(*it); } }

//! Dodaje elementu przetwarzającego zadanego typu do pluginu.
#define VDF_ADD_DATA_PROCESSOR_SIMPLE(className, uid)                           \
    __providerItems.push_back( vdf::IDataProcessorPtr(new vdf::DataProcessor(new (className), (uid), (#className), [&](const df::IProcessingNode* prototype) { return new className(); })) );

//! Dodaje źródło zadanego typu do pluginu.
#define VDF_ADD_DATA_SOURCE_SIMPLE(className, uid)                           \
	__providerItems.push_back( vdf::IDataSourcePtr(new vdf::DataSource(new (className), (uid), (#className), [&](const df::ISourceNode* prototype) { return new className(); })) );

//! Dodaje sink zadanego typu do pluginu.
#define VDF_ADD_DATA_SINK_SIMPLE(className, uid)                           \
	__providerItems.push_back( vdf::IDataSinkPtr(new vdf::DataSink(new (className), (uid), (#className), [&](const df::ISinkNode* prototype) { return new className(); })) );

//////////////////////////////////////////////////////////////////////////
//! Dodaje elementu przetwarzającego zadanego typu do pluginu.
#define VDF_ADD_DATA_PROCESSOR(className, name, uid, icon)                           									 \
	do {																												 \
	auto processor =  vdf::IDataProcessorPtr(new vdf::DataProcessor(new (className), core::UID::GenerateUniqueID(uid), 	 \
		 (name), [&](const df::IProcessingNode* prototype) { return new className(); }));								 \
	processor->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/processor.png") : icon);                                    \
	__providerItems.push_back(processor);																				 \
	} while (0);																										 
																														 
//! Dodaje źródło zadanego typu do pluginu.																				 
#define VDF_ADD_DATA_SOURCE(className, name, uid, icon)                           										 \
	do {																												 \
	auto source = vdf::IDataSourcePtr(new vdf::DataSource(new (className), core::UID::GenerateUniqueID(uid), 			 \
		(name), [&](const df::ISourceNode* prototype) { return new className(); }));									 \
	source->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/source.png") : icon);         								 \
	__providerItems.push_back(source);																					 \
	} while(0);																											 
																														 
//! Dodaje sink zadanego typu do pluginu.																				 
#define VDF_ADD_DATA_SINK(className, name, uid, icon)                           										 \
	do {																												 \
	auto sink = vdf::IDataSinkPtr(new vdf::DataSink(new (className), core::UID::GenerateUniqueID(uid), 					 \
		(name), [&](const df::ISinkNode* prototype) { return new className(); }));										 \
	sink->setIcon(icon.isNull() ? QIcon(":/newVdf/icons/sink.png") : icon);         									 \
	__providerItems.push_back(sink);																					 \
	} while(0);


// Ułatwia dodawanie elementów Data Flow. Tworzony jest serwis, którego 
// jedynym celem jest dostarczenie elementów przetwarzających. Parametrami
// są: unikalna nazwa oraz unikalny identyfikator. 
// Przykład użycia:
// VDF_SERVICE_BEGIN(UnikalnaNazwaSerwisu, "{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}")
// 		VDF_ADD_DATA_PROCESSOR(NazwaZdefiniowanegoProcesora, core::UID::GenerateUniqueID("{YYYYYYYY-YYYY-YYYY-YYYY-YYYYYYYYYYYY}"))
// VDF_SERVICE_END(UnikalnaNazwaSerwisu)
#define VDF_SERVICE_BEGIN(ServiceName, uid) 								  \
class ServiceName : public plugin::IService, public vdf::IDataFlowProvider	  \
{																			  \
	UNIQUE_ID(uid)															  \
	CLASS_DESCRIPTION(#ServiceName, #ServiceName);							  \
	VDF_BEGIN																  

//! Kontynuacja makra VDF_SERVICE_BEGIN
//! ServiceName musi być identyczne!
#define VDF_SERVICE_END(ServiceName)										  \
	VDF_END																	  \
public:																		  \
	virtual void init( 														  \
		core::ISourceManager * sourceManager, 								  \
		core::IVisualizerManager * visualizerManager,						  \
		core::IDataManager * memoryDataManager, 						  \
		core::IStreamDataManager * streamDataManager, 						  \
		core::IFileDataManager * fileDataManager,								\
		core::IDataHierarchyManager * hierarchyManager) {}						  \
	virtual const bool lateInit()  { return true; } 						  \
	virtual void finalize() {} 												  \
	virtual void update( double deltaTime ) {} 								  \
	virtual QWidget* getWidget() { return nullptr; } 						  \
	virtual QWidgetList getPropertiesWidgets() { return QWidgetList(); }      \
};																			  \
CORE_PLUGIN_ADD_SERVICE(ServiceName);


namespace vdf {

class IDataFlowProvider
{
public:
	virtual ~IDataFlowProvider() {}
	virtual void provideDataFlowItems(std::list<IWorkflowItemBaseWeakPtr>& items) = 0;
};
DEFINE_SMART_POINTERS(IDataFlowProvider);

}

#endif
