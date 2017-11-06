/********************************************************************
	created:	2013/05/26
	created:	26:5:2013   10:54
	filename: 	HmmService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMMLIB__HMMSERVICE_H__
#define HEADER_GUARD_HMMLIB__HMMSERVICE_H__

#include <list>
#include <utils/TypeInfo.h>
#include <corelib/IService.h>
#include <corelib/IFilterProvider.h>
								  

class HmmService : public plugin::IService, public core::IFilterProvider
{																		
    UNIQUE_ID("{52037A6F-9C21-447F-A738-11F22FE9AA59}")														
    CLASS_DESCRIPTION("HmmService", "HmmService");						
												
public:																	
    virtual void init( 													
        core::ISourceManager * sourceManager, 								 
        core::IVisualizerManager * visualizerManager,						 
        core::IDataManager * memoryDataManager, 						 
        core::IStreamDataManager * streamDataManager, 						 
        core::IFileDataManager * fileDataManager,
		core::IDataHierarchyManager * hierarchyManager) {}						 
	virtual const bool lateInit();
    virtual void finalize() {} 											
    virtual void update( double deltaTime ) {} 							
    virtual QWidget* getWidget() { return nullptr; } 					
    virtual QWidgetList getPropertiesWidgets() { return QWidgetList(); } 
    virtual core::IFilterProvider::FilterBundles getFilterBundles() const;

private:
    void addSubjectFilterToBundle( core::IFilterBundlePtr fb, const utils::TypeInfo&, const QString& name, const QIcon& icon) const;
};										
DEFINE_SMART_POINTERS(HmmService);

#endif
