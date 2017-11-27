/********************************************************************
	created:	2014/04/29
	created:	29:4:2014   11:21
	filename: 	EquestExporterService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST_EXPORTER__EQUESTEXPORTERSERVICE_H__
#define HEADER_GUARD_EQUEST_EXPORTER__EQUESTEXPORTERSERVICE_H__

#include <corelib/IService.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/BaseDataTypes.h>
#include <plugins/equestExporter/IEquestExporterService.h>
#include "ExporterModel.h"

namespace equestExporter {

	class EquestExporterServiceWidget;

	class EquestExporterService : public IEquestExporterService
	{
		UNIQUE_ID("{C0B7E29A-BD8F-4BF5-B395-E9B93106A9E7}")
		CLASS_DESCRIPTION("Equest Exporter Service", "Equest Exporter Service");
		
	public:
		EquestExporterService();
		virtual ~EquestExporterService();

	public:
		//! 
		//! \param managersAccessor 
		virtual void init(core::ISourceManager * sourceManager,
			core::IVisualizerManager * visualizerManager,
			core::IDataManager * memoryDataManager,
			core::IStreamDataManager * streamDataManager,
			core::IFileDataManager * fileDataManager,
			core::IDataHierarchyManager * hierarchyManager);
		//! 
		//! \param actions 
		virtual QWidget* getWidget();
		//! 
		//! \param actions 
		virtual QWidgetList getPropertiesWidgets();

		virtual const bool lateInit();
		virtual void finalize();
		virtual void update(double time);

	public:
		virtual QWidget* getExporterDialog();
				
	private:
		EquestExporterServiceWidget* exportWidget;
		core::IDataManager * memoryManager;
		ExporterModelPtr exporterModel;
	};
	DEFINE_SMART_POINTERS(EquestExporterService);
}

#endif
