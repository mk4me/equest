/********************************************************************
	created:	2015/08/25
	created:	11:47
	filename: 	DicomService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DicomService_H__
#define HEADER_GUARD_DICOM__DicomService_H__

#include <plugins/dicom/DicomPerspective.h>
#include <corelib/IDataHierarchyManager.h>
#include <corelib/ISourceManager.h>
#include "IDicomService.h"
#include "plugins/hmdbCommunication/IHMDBSession.h"


namespace dicom {

	class AnnotationStatusManager;

	class DicomService : public IDicomService
	{
		UNIQUE_ID("{2B0AE786-F194-46DE-A161-CCCFF317E44B}")
			CLASS_DESCRIPTION("DicomService", "DicomService");

	public:
		DicomService();

		virtual ~DicomService();

		virtual void init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager,
						  core::IDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager,
						  core::IDataHierarchyManager * hierarchyManager);

		virtual const bool lateInit();

		virtual void finalize();

		virtual void update(double deltaTime);
		virtual QWidget* getWidget();
		virtual QWidgetList getPropertiesWidgets();

		virtual void updateItemIcon(const std::string& filename, const QIcon& icon);

		static const bool userIsReviewer(hmdbCommunication::IHMDBSession * session);

		AnnotationStatusManager* getAnnotationStatusManager();
	private:
		core::ISourceManager *sourceManager;
		core::IDataHierarchyManager* hierarchyManager;
		DicomPerspectivePtr perspective;
		std::mutex statusMutex;
		utils::unique_ptr<AnnotationStatusManager> statusManager;
	};

}

#endif // HEADER_GUARD 
