#include <plugins/dicom/AnnotationStatusManager.h>
#include "plugins/hmdbCommunication/IHMDBSource.h"
#include "corelib/ISourceManager.h"
#include "plugins/hmdbCommunication/IHMDBSession.h"
#include <hmdbserviceslib/IBasicQueriesWS.h>
#include "corelib/PluginCommon.h"
#include <hmdbserviceslib/BasicQueriesWS.h>
#include "plugins/hmdbCommunication/IHMDBShallowCopyContext.h"
#include "plugins/hmdbCommunication/IHMDBSession.h"
#include <hmdbserviceslib/IAuthorizationWS.h>
#include <hmdbserviceslib/Entity.h>
#include <queue>
#include <stack>

dicom::AnnotationStatusManager::AnnotationStatusManager()
{
	refreshData();
	start();
}

//dicom::AnnotationStatusManager* dicom::AnnotationStatusManager::instance()
//{
//	static AnnotationStatusManager sm;
//	// przeniesione do konstruktora
//	/*sm.refreshData();
//	sm.start();*/
//	return &sm;
//}

hmdbCommunication::IHMDBSession * dicom::AnnotationStatusManager::getSession(hmdbCommunication::IHMDBShallowCopyContext* srcContext)
{
	if (srcContext && srcContext->shallowCopyRemoteContext()) {
		auto session = srcContext->shallowCopyRemoteContext()->remoteContext()->session().get();
		return session;
	}
	return nullptr;
}

hmdbCommunication::IHMDBShallowCopyContext* dicom::AnnotationStatusManager::getShallowCopyContext()
{
	auto source = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (source != nullptr) {
		auto srcContext = source->shallowContext(0);
		return srcContext.get();
	}
	return nullptr;
}

dicom::AnnotationStatusManager::~AnnotationStatusManager()
{
	thread.interrupt();
	thread.join();
}

dicom::AnnotationStatusManager::Annotations dicom::AnnotationStatusManager::getAnnotations(hmdbCommunication::IHMDBSession * session)
{
	Annotations res;
	auto resp = session->motionQueries()->listAnnotationsXML();
	auto annotations = hmdbServices::xmlWsdl::parseAnnotations(resp);
	for (auto& a : annotations) {
		res[a.trialID][a.userID] = a;
	}

	return res;
}

std::map<QString, int> getTrials(hmdbCommunication::IHMDBShallowCopyContext* shallow)
{
	std::map<QString, int> res;
	auto & shallowCopy = shallow->shallowCopyDataContext()->shallowCopy()->motionShallowCopy;
	auto& trials = shallowCopy.trials;
	for (auto it = trials.begin(); it != trials.end(); ++it) {
		auto* trial = it->second;
		res[QString::fromStdString(trial->trialName)] = trial->trialID;
	}
	return res;
}

std::map<QString, int> getUsers(hmdbCommunication::IHMDBSession * session)
{
	std::map<QString, int> res;

	auto resp = session->authorization()->listUsers();
	auto usersList = hmdbServices::xmlWsdl::parseUsersList(resp);
	for (auto it = usersList.begin(); it != usersList.end(); ++it) {
		res[QString::fromStdString(it->login)] = it->id;
	}
	return res;
}

const dicom::AnnotationStatusManager::AnnotationData* dicom::AnnotationStatusManager::getAnnotationData()
{
//	auto session = getSession(getShallowCopyContext());
////	auto modTime = refresh ? session->motionQueries()->dataModificationTime() : lastUpdate;
////	if (lastUpdate < modTime) {
//	if (!data) {
//		refreshData();
//		start();
//	}
	return data ? data.get() : nullptr;
}

void dicom::AnnotationStatusManager::start()
{
	 thread = plugin::getThreadPool()->get("AnnatationStatusManager", "Synchronization");
	 thread.run([&]() {
		 std::condition_variable cv;
		 std::mutex m;
		 std::unique_lock<std::mutex> ul(m);
		 while (true) {
			 this->refreshData();
			 //cv.wait_for(ul, std::chrono::minutes(1));
			 //core::Thread::InterruptiblePolicy::wait(cv, ul, std::chrono::minutes(1));
			 core::Thread::InterruptiblePolicy::wait_for(cv, ul, std::chrono::minutes(1), std::chrono::milliseconds(1));
		 }
	 });
}

void dicom::AnnotationStatusManager::refreshData()
{
	auto context = getShallowCopyContext();
	auto session = getSession(context);
	if (session) {
		std::lock_guard<std::mutex> lock(dataMutex);
		data = std::unique_ptr<AnnotationData>(new AnnotationData);
		data->user2Id = getUsers(session);
		data->trial2Id = getTrials(context);
		data->annotations = getAnnotations(session);
	}
	else {
		throw std::runtime_error("Communication session does not exist");
	}
}

hmdbServices::xmlWsdl::Annotation dicom::AnnotationStatusManager::getAnnotationStatus(const std::string& user, int trialId)
{
	std::lock_guard<std::mutex> lock(dataMutex);
	//const AnnotationStatusManager::AnnotationData* ad = getAnnotationData();
	return data->annotations.at(trialId).at(data->user2Id.at(QString::fromStdString(user)));
}


