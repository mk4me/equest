/********************************************************************
	created:	2015/08/25
	created:	11:47
	filename: 	EquestService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST__EquestService_H__
#define HEADER_GUARD_EQUEST__EquestService_H__

#include <plugins/equest/EquestPerspective.h>
#include <corelib/IDataHierarchyManager.h>
#include <corelib/ISourceManager.h>
#include "IEquestService.h"
#include "plugins/hmdbCommunication/IHMDBSession.h"


namespace equest {

	class AnnotationStatusManager;

	class EquestService : public IEquestService
	{
		UNIQUE_ID("{113D5D8D-F694-4C8C-80BB-DA8BC8EF6D2E}")
			CLASS_DESCRIPTION("EquestService", "EquestService");

	public:
		EquestService();

		virtual ~EquestService();

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
		EquestPerspectivePtr perspective;
		std::mutex statusMutex;
		utils::unique_ptr<AnnotationStatusManager> statusManager;
	};

}

#endif // HEADER_GUARD 
