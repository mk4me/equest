/********************************************************************
	created:	2014/04/29
	created:	29:4:2014   11:21
	filename: 	MedusaExporterService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__MEDUSAEXPORTERSERVICE_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__MEDUSAEXPORTERSERVICE_H__

#include <corelib/IService.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/BaseDataTypes.h>
#include <plugins/medusaExporter/IMedusaExporterService.h>
#include "ExporterModel.h"

namespace medusaExporter {

	class MedusaExporterServiceWidget;

	class MedusaExporterService : public IMedusaExporterService
	{
		UNIQUE_ID("{9B95F335-1E27-4245-8D00-A17C190F9D2A}")
		CLASS_DESCRIPTION("Medusa Exporter Service", "Medusa Exporter Service");
		
	public:
		MedusaExporterService();
		virtual ~MedusaExporterService();

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
		MedusaExporterServiceWidget* exportWidget;
		core::IDataManager * memoryManager;
		ExporterModelPtr exporterModel;
	};
	DEFINE_SMART_POINTERS(MedusaExporterService);
}

#endif
