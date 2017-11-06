/********************************************************************
	created:	2015/06/02
	created:	13:49
	filename: 	AnnotationStatusManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__AnnotationStatusManager_H__
#define HEADER_GUARD_DICOM__AnnotationStatusManager_H__

#include <plugins/dicom/Export.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include "hmdbserviceslib/Entity.h"
#include <QtCore/QString>
#include "utils/ObserverPattern.h"
#include <corelib/IThreadPool.h>

namespace  dicom {

	

class AnnotationStatusManager : public utils::Observable<AnnotationStatusManager>
{
public:
		// [ trialId -> [ userId -> annotation ] ]
	typedef std::map<int, std::map<int, hmdbServices::xmlWsdl::Annotation>> Annotations;
		struct AnnotationData
		{
			std::map<QString, int> user2Id;
			std::map<QString, int> trial2Id;
			Annotations	annotations;
		};
			

public:
	AnnotationStatusManager();
		virtual ~AnnotationStatusManager();

public:
	//static AnnotationStatusManager* instance();
	void start();
	hmdbServices::xmlWsdl::Annotation getAnnotationStatus(const std::string& user, int trialId);

private:
	const AnnotationData* getAnnotationData();
	
private:
	hmdbCommunication::IHMDBShallowCopyContext* getShallowCopyContext();

	hmdbCommunication::IHMDBSession * getSession(hmdbCommunication::IHMDBShallowCopyContext* srcContext);
		
	Annotations getAnnotations(hmdbCommunication::IHMDBSession * session);

	void refreshData();

private:
	std::unique_ptr<AnnotationData> data;
	core::Thread thread;
	std::mutex dataMutex;
};
}

#endif // HEADER_GUARD 
