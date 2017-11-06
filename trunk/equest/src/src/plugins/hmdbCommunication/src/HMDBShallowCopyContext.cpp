#include "CommunicationPCH.h"
#include "HMDBShallowCopyContext.h"
#include "HMDBSourceContextOperations.h"
#include "HMDBStatusManager.h"
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <plugins/hmdbCommunication/IPatient.h>
#include <plugins/hmdbCommunication/IHierarchyPerspective.h>
#include "Patient.h"

#include <hmdbserviceslib/DateTimeUtils.h>

#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/ISubject.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>

#include <plugins/kinematic/Wrappers.h>
#include <plugins/video/Wrappers.h>
#include <plugins/c3d/C3DCollections.h>

#include <corelib/PluginCommon.h>
#include <corelib/IServiceManager.h>
#include <corelib/ISourceManager.h>
#include <corelib/IParserManagerReader.h>
#include <utils/Utils.h>
#include <boost/lexical_cast.hpp>

using namespace hmdbCommunication;

HMDBShallowCopyDataContext::HMDBShallowCopyDataContext() : statusManager_(new HMDBStatusManager)
{

}

HMDBShallowCopyDataContext::~HMDBShallowCopyDataContext()
{

}

ShallowCopyConstPtr HMDBShallowCopyDataContext::shallowCopy() const
{
	return shallowCopy_;
}

IHMDBStatusManagerConstPtr HMDBShallowCopyDataContext::dataStatusManager() const
{
	return statusManager_;
}

IHMDBStatusManagerPtr HMDBShallowCopyDataContext::dataStatusManager()
{
	return statusManager_;
}

void HMDBShallowCopyDataContext::setShallowCopy(const ShallowCopyConstPtr shallowCopy)
{
	shallowCopy_ = shallowCopy;
	if (shallowCopy_ != nullptr){
		//statusManager_->transaction()->rebuild(shallowCopy_);
	}
}

HMDBShallowCopyLocalContext::HMDBShallowCopyLocalContext(IHMDBShallowCopyDataContextPtr shallowCopyContext,
	IHMDBLocalContextPtr localContext, core::IDataHierarchyManager * hdm) : shallowCopyContext_(shallowCopyContext),
	localContext_(localContext), hdm(hdm)
{

}

HMDBShallowCopyLocalContext::~HMDBShallowCopyLocalContext()
{
	unloadAll();
}

void childItems(const hmdbServices::MotionShallowCopy::Files & files,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = files.begin(); IT != files.end(); ++IT){
		fileIDs.insert(IT->second->fileID);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Trials & motions,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = motions.begin(); IT != motions.end(); ++IT){
		childItems(IT->second->files, fileIDs);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Session * session,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	childItems(session->files, fileIDs);	

	for (auto IT = session->trials.begin(); IT != session->trials.end(); ++IT){
		motionIDs.insert(IT->second->trialID);
		childItems(IT->second->files, fileIDs);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Sessions sessions,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = sessions.begin(); IT != sessions.end(); ++IT){
		childItems(IT->second, motionIDs, fileIDs);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Performer * subject,
	std::set<hmdbServices::ID> & sessionIDs,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = subject->performerConfs.begin(); IT != subject->performerConfs.end(); ++IT){
		childItems(IT->second->session, motionIDs, fileIDs);
		sessionIDs.insert(IT->second->session->sessionID);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Performers subjects,
	std::set<hmdbServices::ID> & sessionIDs,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = subjects.begin(); IT != subjects.end(); ++IT){
		childItems(IT->second, sessionIDs, motionIDs, fileIDs);
	}
}

void childItems(const hmdbServices::MedicalShallowCopy::Patient * patient,
	std::set<hmdbServices::ID> & subjectsIDs,
	std::set<hmdbServices::ID> & sessionIDs,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	if (patient->performer != nullptr){
		subjectsIDs.insert(patient->performer->performerID);
		childItems(patient->performer, sessionIDs, motionIDs, fileIDs);
	}	
}

void childItems(const hmdbServices::MedicalShallowCopy::Patients & patients,
	std::set<hmdbServices::ID> & subjectsIDs,
	std::set<hmdbServices::ID> & sessionIDs,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = patients.begin(); IT != patients.end(); ++IT){
		childItems(IT->second, subjectsIDs, sessionIDs, motionIDs, fileIDs);
	}
}

std::map<DataType, std::set<hmdbServices::ID>> childItems(const DataType type,
	const hmdbServices::ID id, const ShallowCopyConstPtr shallowCopy)
{
	std::map<DataType, std::set<hmdbServices::ID>> ret;	

	switch (type)
	{
	case MotionType:

	{
		std::set<hmdbServices::ID> fileIDs;

		if (id < 0){
			childItems(shallowCopy->motionShallowCopy.trials, fileIDs);
		}
		else{
			auto it = shallowCopy->motionShallowCopy.trials.find(id);
			if (it != shallowCopy->motionShallowCopy.trials.end()){
				childItems(it->second->files, fileIDs);
			}
		}

		if (fileIDs.empty() == false){
			ret[FileType].insert(fileIDs.begin(), fileIDs.end());
		}
	}

		break;

	case SessionType:

	{
		std::set<hmdbServices::ID> fileIDs;
		std::set<hmdbServices::ID> motionIDs;

		if (id < 0){
			childItems(shallowCopy->motionShallowCopy.sessions, motionIDs, fileIDs);
		}
		else{
			auto it = shallowCopy->motionShallowCopy.sessions.find(id);
			if (it != shallowCopy->motionShallowCopy.sessions.end()){
				childItems(it->second, motionIDs, fileIDs);
			}
		}

		if (motionIDs.empty() == false){
			ret[MotionType].insert(motionIDs.begin(), motionIDs.end());
		}

		if (fileIDs.empty() == false){
			ret[FileType].insert(fileIDs.begin(), fileIDs.end());
		}
	}

		break;

	case SubjectType:

	{
		std::set<hmdbServices::ID> fileIDs;
		std::set<hmdbServices::ID> motionIDs;
		std::set<hmdbServices::ID> sessionIDs;

		if (id < 0){
			childItems(shallowCopy->motionShallowCopy.performers, sessionIDs, motionIDs, fileIDs);
		}
		else{
			auto it = shallowCopy->motionShallowCopy.performers.find(id);
			if (it != shallowCopy->motionShallowCopy.performers.end()){
				childItems(it->second, sessionIDs, motionIDs, fileIDs);
			}
		}

		if (sessionIDs.empty() == false){
			ret[SessionType].insert(sessionIDs.begin(), sessionIDs.end());
		}

		if (motionIDs.empty() == false){
			ret[MotionType].insert(motionIDs.begin(), motionIDs.end());
		}

		if (fileIDs.empty() == false){
			ret[FileType].insert(fileIDs.begin(), fileIDs.end());
		}
	}

		break;

	case PatientType:

	{
		std::set<hmdbServices::ID> fileIDs;
		std::set<hmdbServices::ID> motionIDs;
		std::set<hmdbServices::ID> sessionIDs;
		std::set<hmdbServices::ID> subjectIDs;

		if (id < 0){
			childItems(shallowCopy->medicalShallowCopy.patients, subjectIDs, sessionIDs, motionIDs, fileIDs);
		}
		else{
			auto it = shallowCopy->medicalShallowCopy.patients.find(id);
			if (it != shallowCopy->medicalShallowCopy.patients.end()){
				childItems(it->second, subjectIDs, sessionIDs, motionIDs, fileIDs);
			}
		}

		if (subjectIDs.empty() == false){
			ret[SubjectType].insert(subjectIDs.begin(), subjectIDs.end());
		}

		if (sessionIDs.empty() == false){
			ret[SessionType].insert(sessionIDs.begin(), sessionIDs.end());
		}

		if (motionIDs.empty() == false){
			ret[MotionType].insert(motionIDs.begin(), motionIDs.end());
		}

		if (fileIDs.empty() == false){
			ret[FileType].insert(fileIDs.begin(), fileIDs.end());
		}
	}

		break;

	}

	return ret;
}

void filterAndInsertData(core::ConstVariantsList & inputData, core::ConstVariantsList & outputData,
	const IHMDBLocalContextConstPtr localContext)
{
	auto it = inputData.begin();
	while (it != inputData.end()){
		if (localContext->transaction()->isMyData(*it) == false){
			it = inputData.erase(it);
		}
		else{
			outputData.push_back(*it);
			++it;
		}
	}
}

core::ConstVariantsList HMDBShallowCopyLocalContext::data(const DataType type,
	const hmdbServices::ID id, const bool recursive) const
{
	core::ConstVariantsList ret;

	std::map<DataType, std::set<hmdbServices::ID>> items;

	if (recursive == true){
		items = childItems(type, id, shallowCopyContext_->shallowCopy());
	}

	items[type].insert(id);

	for (auto it = items.begin(); it != items.end(); ++it){
		auto dIT = data_.find(type);
		// czy mam taką grupę obiektów
		if (dIT != data_.end()){
			//czy chcę dane wszystkich obiektów tego typu?
			if (it->second.find(-1) != it->second.end()){
				// dla wszystkich indeksów
				auto IT = dIT->second.begin();
				while(IT != dIT->second.end()){
					//filtrowanie danych które zostały wyładowane
					filterAndInsertData(IT->second, ret, localContext_);
					if (IT->second.empty() == true){
						IT = dIT->second.erase(IT);
					}
					else{
						++IT;
					}
				}
				//czy grupa pusta?
				if (dIT->second.empty() == true){
					data_.erase(dIT);
				}
			}
			else{
				auto IT = it->second.begin();
				
				while(IT != it->second.end()){
					auto tIT = dIT->second.find(*IT);
					if (tIT != dIT->second.end()){
						filterAndInsertData(tIT->second, ret, localContext_);

						if (tIT->second.empty() == true){
							dIT->second.erase(tIT);
							if (dIT->second.empty() == true){
								data_.erase(dIT);
							}							
						}
					}
					
					++IT;					
				}
			}
		}
	}

	return ret;
}

hmdbCommunication::StorageFileNames filterSupportedFiles(const hmdbCommunication::StorageFileNames & inFiles)
{
	auto pmr = plugin::getParserManagerReader();
	hmdbCommunication::StorageFileNames ret;

	for (auto f : inFiles){
		if (pmr->sourceIsAccepted(f.second.fileName) == true){
			ret.insert(f);
		}
	}

	return ret;
}

hmdbCommunication::StorageFileNames filterFiles(const hmdbCommunication::StorageFileNames & inFiles,
	const IHMDBStatusManager::TransactionConstPtr transaction, const DataStatus & status)
{
	hmdbCommunication::StorageFileNames ret;

	for (auto it = inFiles.begin(); it != inFiles.end(); ++it){
		auto fs = transaction->dataStatus(FileType, it->first);
		if (DataStatus::filter(fs, status) == true){
			ret.insert(*it);
		}
	}

	return ret;
}

class JointsInitializer : public core::IVariantInitializer
{
public:

	JointsInitializer(core::VariantConstPtr dataWrapper,
		core::VariantConstPtr modelWrapper)
		: dataWrapper(dataWrapper), modelWrapper(modelWrapper)
	{
	}

	virtual ~JointsInitializer()
	{
	}

	virtual void initialize(core::Variant * object)
	{
		acclaim::MotionDataConstPtr data;
		acclaim::SkeletonConstPtr model;
		if (dataWrapper->tryGet(data) == true && modelWrapper->tryGet(model) == true && data != nullptr && model != nullptr) {
			if (!data->frames.empty()) {
				kinematic::SkeletonPtr skeleton = utils::make_shared<kinematic::Skeleton>();
				if (!kinematic::Skeleton::convert(*model, *skeleton)) {
					throw std::runtime_error("Unable to convert skeleton");
				}
				SkeletonStatesPtr states = utils::make_shared<SkeletonStates>();
				states->frameTime = data->frameTime;
				auto firstFrame = data->frames[0];

				const auto amapping = acclaim::Skeleton::createMapping(model->bones);
				const auto mapping = kinematic::LinearizedSkeleton::createNonLeafMapping(*skeleton);
				const auto activeMapping = kinematic::SkeletonState::createAcclaimActiveMappingLocal(*skeleton, model->bones);
				const auto helperData = acclaim::Skeleton::helperMotionData(*model);

				for (auto& frame : data->frames) {
					auto sChange = kinematic::SkeletonState::convert(model->bones, amapping, frame.bonesData, activeMapping, helperData, model->units.isAngleInRadians());
					states->frames.push_back(sChange);
				}
				
				auto sws = utils::make_shared<SkeletonWithStates>();
				sws->skeleton = skeleton;
				sws->states = states;
				sws->nodesMapping = kinematic::LinearizedSkeleton::createCompleteMapping(*skeleton);
				sws->scale = 0.25;
				object->set(sws);
			} else {
				// 
			}
		}
	}
	
	virtual IVariantInitializer * clone() const
	{
		return new JointsInitializer(dataWrapper, modelWrapper);
	}

private:
	core::VariantConstPtr dataWrapper;
	core::VariantConstPtr modelWrapper;
};

void HMDBShallowCopyLocalContext::addPatientObject(const hmdbServices::MedicalShallowCopy::Patient * patient, PluginSubject::SubjectID subjectID)
{
	//generuję listę schorzeń
	std::vector<hmdbCommunication::Disorder> disorders;
	for (auto it = patient->disorders.begin(); it != patient->disorders.end(); ++it)	{
		hmdbCommunication::Disorder dis;
		dis.focus = it->second.focus;
		dis.diagnosisDate = hmdbServices::toString(it->second.diagnosisDate);
		dis.comments = it->second.comments;
		dis.name = it->second.disorder->name;

		disorders.push_back(dis);
	}

	PatientPtr pPtr(new Patient(subjectID, patient->name, patient->surname, patient->birthDate.toString(),
		Patient::decodeGender(patient->gender), utils::shared_ptr<const QPixmap>(), disorders));

	//dodaję do DM
	auto pOW = core::Variant::create<hmdbCommunication::IPatient>();
	pOW->set(pPtr);

	localContext_->transaction()->load(pOW);

	//zapamiętuje
	data_[PatientType][subjectID].push_back(pOW);
}

float getAntropometricValue(const std::string & attribute, const hmdbServices::MotionShallowCopy::Attrs & attrs, float defValue = 0.0)
{
	auto it = attrs.find(attribute);
	if (it != attrs.end()){
		return boost::lexical_cast<float>(it->second);
	}

	return defValue;
}

utils::shared_ptr<hmdbCommunication::AntropometricData> createAntropometricData(const hmdbServices::MotionShallowCopy::Attrs & attrs)
{
	auto antro = utils::shared_ptr<hmdbCommunication::AntropometricData>(new hmdbCommunication::AntropometricData());
	antro->bodyMass = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("BodyMass", attrs), "kg");
	antro->height = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("Height", attrs), "mm");
	antro->interAsisDistance = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("InterAsisDistance", attrs), "mm");

	antro->leftLegLength = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftLegLength", attrs), "mm");
	antro->rightLegLenght = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightLegLenght", attrs), "mm");

	antro->leftKneeWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftKneeWidth", attrs), "mm");
	antro->rightKneeWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightKneeWidth", attrs), "mm");

	antro->leftAnkleWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftAnkleWidth", attrs), "mm");
	antro->rightAnkleWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightAnkleWidth", attrs), "mm");

	antro->leftCircuitThigh = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftCircuitThigh", attrs), "mm");
	antro->rightCircuitThight = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightCircuitThight", attrs), "mm");

	antro->leftCircuitShank = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftCircuitShank", attrs), "mm");
	antro->rightCircuitShank = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightCircuitShank", attrs), "mm");

	antro->leftShoulderOffset = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftShoulderOffset", attrs), "mm");
	antro->rightShoulderOffset = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightShoulderOffset", attrs), "mm");

	antro->leftElbowWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftElbowWidth", attrs), "mm");
	antro->rightElbowWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightElbowWidth", attrs), "mm");

	antro->leftWristWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftWristWidth", attrs), "mm");
	antro->rightWristWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightWristWidth", attrs), "mm");

	antro->leftWristThickness = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftWristThickness", attrs), "mm");
	antro->rightWristThickness = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightWristThickness", attrs), "mm");

	antro->leftHandWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftHandWidth", attrs), "mm");
	antro->rightHandWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightHandWidth", attrs), "mm");

	antro->leftHandThickness = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftHandThickness", attrs), "mm");
	antro->rightHandThickness = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightHandThickness", attrs), "mm");

	return antro;
}

core::VariantPtr HMDBShallowCopyLocalContext::findObjectByType(const DataType type, const hmdbServices::ID id,
	const utils::TypeInfo & typeInfo) const
{
	core::VariantPtr ret;

	auto it = data_.find(type);
	if (it != data_.end()){
		auto IT = it->second.find(id);
		if (IT != it->second.end()){
			for (auto dIT = IT->second.begin(); dIT != IT->second.end(); ++dIT){
				auto ow = *dIT;
				if (ow->data()->isSupported(typeInfo) == true){
					ret = utils::const_pointer_cast<core::Variant>(ow);
					break;
				}
			}
		}			
	}

	return ret;
}

core::VariantPtr HMDBShallowCopyLocalContext::getSubject(const hmdbServices::ID id,
	utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
	const ShallowCopyConstPtr shallowCopy)
{
	core::VariantPtr subOW = findObjectByType(SubjectType, id, typeid(PluginSubject::ISubject));

	if (subOW == nullptr){

		PluginSubject::SubjectPtr subPtr;
		//tworzę subjecta
		subOW = subjectService->createSubject();

		//FIX dla linux RtR
		//subPtr = subOW->get();
		subOW->tryGet(subPtr);

		std::stringstream label;

		auto pIT = shallowCopy->medicalShallowCopy.patients.find(id);

		if (pIT == shallowCopy->medicalShallowCopy.patients.end()){
			label << "Subject " << id;
		}
		else{
			label << pIT->second->name << ", " << pIT->second->surname;
			//TODO
			addPatientObject(pIT->second, subPtr->getID());
		}

		subOW->setMetadata("label", label.str());

		//dodaję do DM
		localContext_->transaction()->load(subOW);

		//zapamiętuję mapowanie
		data_[SubjectType][id].push_back(subOW);
	}

	return subOW;
}

core::VariantPtr HMDBShallowCopyLocalContext::getSession(const std::pair<Indexes, MotionFiles> & sessionFiles,
	const IndexedData & loadedFilesData,
	PluginSubject::SubjectPtr subPtr,
	core::VariantPtr subOW,
	const hmdbServices::ID id,
	utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
	const ShallowCopyConstPtr shallowCopy)
{
	core::VariantPtr sOW = findObjectByType(SessionType, id, typeid(PluginSubject::ISession));

	if (sOW == nullptr){

		PluginSubject::SessionPtr sPtr;

		auto sIT = shallowCopy->motionShallowCopy.sessions.find(id);

		if (sIT == shallowCopy->motionShallowCopy.sessions.end()){
			PLUGIN_LOG_DEBUG("Inconsistent subject hierarchy");
			return sOW;
		}

		auto s = sIT->second;

		//tworzę sesję
		//generuję zbiór ow dla sesji
		core::ConstVariantsList sessionObjects;
		for (auto fIT = sessionFiles.first.begin(); fIT != sessionFiles.first.end(); ++fIT){
			//pobieram obiekty

			auto it = loadedFilesData.find(*fIT);

			if (it != loadedFilesData.end()){
				sessionObjects.insert(sessionObjects.end(), it->second.begin(), it->second.end());
			}
			else{
				PLUGIN_LOG_DEBUG("Empty file in hierarchy");
			}
		}

		//dane antropometryczne
		auto antro = createAntropometricData(s->performerConf->attrs);
		//tworze OW dla danych antropometrycznych
		auto antroOW = core::Variant::create<hmdbCommunication::AntropometricData>();
		antroOW->set(antro);

		sessionObjects.push_back(antroOW);

		sOW = subjectService->createSession(subOW);

		//FIX dla linux RtR
		//sPtr = sOW->get();
		sOW->tryGet(sPtr);

		for (auto it = sessionObjects.begin(); it != sessionObjects.end(); ++it){
			sPtr->addData(*it);
		}

		sOW->setMetadata("label", s->sessionName);
		sOW->setMetadata("EMGConf", boost::lexical_cast<std::string>(s->emgConf));
		sOW->setMetadata("data", hmdbServices::toString(s->sessionDate));
		if (s->groupAssigment != nullptr){
			sOW->setMetadata("groupID", boost::lexical_cast<std::string>(s->groupAssigment->sessionGroupID));
			auto sgIT = shallowCopy->motionMetaData.sessionGroups.find(s->groupAssigment->sessionGroupID);
			if (sgIT != shallowCopy->motionMetaData.sessionGroups.end()){
				sOW->setMetadata("groupName", sgIT->second.sessionGroupName);
			}
		}

		//dodaję do DM
		auto t = localContext_->transaction();
		t->load(antroOW);
		t->load(sOW);

		//zapamiętuję mapowanie
		data_[SessionType][id].push_back(sOW);
		data_[SessionType][id].push_back(antroOW);
		subPtr->addSession(sOW);
	}

	return sOW;
}

core::VariantPtr createJointsAngles(const core::ConstVariantsList objects, PluginSubject::SessionPtr session)
{
	core::VariantPtr ret;

	//sprawdzamy joint angles - jeżeli nie ma budujemy i dodajemy do DM
	core::VariantConstPtr dataWrapper;
	core::VariantConstPtr modelWrapper;
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if ((*it)->data()->isSupported(typeid(SkeletonWithStates))) {
			return ret;
		}
		else if ((*it)->data()->isSupported(typeid(acclaim::MotionData))) {
			dataWrapper = *it;
			break;
		}
	}

	core::VariantsCollection modelWrappers(typeid(acclaim::Skeleton), false);
	session->getObjects(modelWrappers);	

	if (modelWrappers.empty() == false){
		modelWrapper = modelWrappers.front();

		if (dataWrapper && modelWrapper) {
			ret = core::Variant::create<SkeletonWithStates>();			
			ret->setInitializer(core::VariantInitializerPtr(new JointsInitializer(dataWrapper, modelWrapper)));
		}
	}	

	return ret;
}

core::VariantPtr HMDBShallowCopyLocalContext::getMotion(const Indexes & motionFiles,
	const IndexedData & loadedFilesData,
	PluginSubject::SessionPtr sPtr,
	core::VariantPtr sOW,
	const hmdbServices::ID id,
	utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
	const ShallowCopyConstPtr shallowCopy)
{
	core::VariantPtr mOW = findObjectByType(MotionType, id, typeid(PluginSubject::IMotion));
	
	if (mOW == nullptr){

		PluginSubject::MotionPtr mPtr;

		auto mIT = shallowCopy->motionShallowCopy.trials.find(id);

		if (mIT == shallowCopy->motionShallowCopy.trials.end()){
			PLUGIN_LOG_DEBUG("Broken subject hierarchy");
			return mOW;
		}

		auto m = mIT->second;

		//tworzę sesję
		//generuję zbiór ow dla motiona
		core::ConstVariantsList motionObjects;
		for (auto fIT = motionFiles.begin(); fIT != motionFiles.end(); ++fIT){

			auto it = loadedFilesData.find(*fIT);

			if (it != loadedFilesData.end()){
				motionObjects.insert(motionObjects.end(), it->second.begin(), it->second.end());
			}
		}

		core::VariantPtr jointsWrapper = createJointsAngles(motionObjects, sPtr);

		if (jointsWrapper != nullptr){
			motionObjects.push_back(jointsWrapper);
		}		

		mOW = subjectService->createMotion(sOW);
		//FIX dla linux RtR
		//mPtr = mOW->get();
		mOW->tryGet(mPtr);

		for (auto it = motionObjects.begin(); it != motionObjects.end(); ++it){
			mPtr->addData(*it);
		}


		if (mPtr->hasObject(typeid(VideoChannel), false) && mPtr->hasObject(typeid(c3dlib::MovieDelays), false)) {
			core::VariantsCollection videoCollection(typeid(VideoChannel), false);
			core::VariantsCollection movieDelaysCollection(typeid(c3dlib::MovieDelays), false);
			mPtr->getObjects(videoCollection);
			mPtr->getObjects(movieDelaysCollection);
			if (movieDelaysCollection.size() == 1) {
				c3dlib::MovieDelaysConstPtr delays = (*(movieDelaysCollection.begin()))->get();
				for (auto it = videoCollection.begin(); it != videoCollection.end(); ++it) {
					core::VariantPtr wrp = utils::const_pointer_cast<core::Variant>(*it);
					std::string path;
					if (wrp->getMetadata("core/source", path)) {
						for (const auto& delay : *delays) {
							if (path.find(delay.first) != std::string::npos) {
								wrp->setMetadata("movieDelay", boost::lexical_cast<std::string>(delay.second));
							}
						}
					}
				}
			}
			else {
				PLUGIN_LOG_ERROR("Wrong movie delays");
			}
		}

		if (jointsWrapper != nullptr){
			//metadane
			jointsWrapper->setMetadata("name", mPtr->getLocalName() + " joints");
			jointsWrapper->setMetadata("source", "hmdbCommunication->motion->" + mPtr->getLocalName());
		}

		mOW->setMetadata("label", m->trialName);

		auto t = localContext_->transaction();
		t->load(mOW);

		//zapamiętuję mapowanie
		data_[MotionType][id].push_back(mOW);

		if (jointsWrapper != nullptr){

			data_[MotionType][id].push_back(jointsWrapper);
			t->load(jointsWrapper);
		}

		sPtr->addMotion(mOW);
	}

	return mOW;
}

HMDBShallowCopyLocalContext::SubjectFiles HMDBShallowCopyLocalContext::groupDataInHierarchy(const IndexedData & loadedFilesData,
	const ShallowCopyConstPtr shallowCopy)
{
	SubjectFiles subjectHierarchy;

	auto itEND = loadedFilesData.end();
	for (auto it = loadedFilesData.begin(); it != itEND; ++it){
		if (it->first < 0){
			auto s = shallowCopy->motionShallowCopy.sessions.find(-it->first);

			if (s != shallowCopy->motionShallowCopy.sessions.end()){
				subjectHierarchy[s->second->performerConf->performer->performerID][s->second->sessionID].first.insert(it->first);
			}
			else{
				//TODO
				//INFO o nieobsługiwanym pliku
			}
		}
		else{
			auto fileIT = shallowCopy->motionShallowCopy.files.find(it->first);
			if (fileIT == shallowCopy->motionShallowCopy.files.end()){
				//TODO
				//INFO o nieobsługiwanym pliku
				continue;
			}

			if (fileIT->second->isSessionFile() == true){
				subjectHierarchy[fileIT->second->session->performerConf->performer->performerID][fileIT->second->session->sessionID].first.insert(it->first);
			}
			else{
				subjectHierarchy[fileIT->second->trial->session->performerConf->performer->performerID][fileIT->second->trial->session->sessionID].second[fileIT->second->trial->trialID].insert(it->first);
			}
		}
	}

	return subjectHierarchy;
}

void HMDBShallowCopyLocalContext::loadSubjectHierarchy(const IndexedData & loadedFilesData)
{
	auto subjectService = core::queryService<PluginSubject::ISubjectService>(plugin::getServiceManager());

	if (subjectService == nullptr){
		return;
	}

	auto shallowCopy = shallowCopyContext_->shallowCopy();

	//buduje mapę hierarchii subject -> session -> motion -> files
	//na bazie tej mapy będę realizować hierarchię pluginu subject

	SubjectFiles subjectHierarchy = groupDataInHierarchy(loadedFilesData, shallowCopy);
	Indexes loadedFiles;

	for(const auto & val : loadedFilesData)
	{
		loadedFiles.insert(val.first);
	}

	auto transaction = localContext_->transaction();
	//auto hierarchyTransaction = mdm->hierarchyTransaction();
	for (auto subjectIT = subjectHierarchy.begin(); subjectIT != subjectHierarchy.end(); ++subjectIT){
		//tworzę subject jeżeli to konieczne!!

		core::VariantPtr subOW = getSubject(subjectIT->first, subjectService, shallowCopy);
		PluginSubject::SubjectPtr subPtr;
		subOW->tryGet(subPtr);

		//mam subjecta, mogę iść dalej do sesji
		for (auto sessionIT = subjectIT->second.begin(); sessionIT != subjectIT->second.end(); ++sessionIT){
			
			core::VariantPtr sOW = getSession(sessionIT->second,
				loadedFilesData, subPtr, subOW, sessionIT->first,
				subjectService, shallowCopy);
			PluginSubject::SessionPtr sPtr;
			sOW->tryGet(sPtr);

			//mam sesję - mogę iść dalej z motionami!!
			for (auto motionIT = sessionIT->second.second.begin(); motionIT != sessionIT->second.second.end(); ++motionIT){
				core::VariantPtr mOW = getMotion(motionIT->second,
					loadedFilesData, sPtr, sOW, motionIT->first,
					subjectService, shallowCopy);				
			}
		}

		auto hmdbSource = core::querySource<IHMDBSource>(plugin::getSourceManager());
		
		if (hmdbSource != nullptr){
			auto hierarchyTransaction = hdm->transaction();
			std::set<core::IHierarchyItemConstPtr> roots;
			for (int i = 0; i < hmdbSource->viewManager()->hierarchyPerspectivesCount(); ++i) {				
				auto root = hmdbSource->viewManager()->hierarchyPerspective(i)->getPerspective(subPtr);
				if (root) {
					int childCount = root->getNumChildren();
					for (int c = 0; c < childCount; ++c) {
						auto r = root->getChild(c);
						updateOrAddRoot(r, roots, hierarchyTransaction);
					}
				}
			}
			files2roots[loadedFiles] = roots;
		}
	}
}

hmdbCommunication::StorageFileNames filesToProcess(const DataType type,
	const hmdbServices::ID id, const hmdbCommunication::ShallowCopy & shallowCopy )
{
	auto files = hmdbCommunication::ShallowCopyUtils::files(type, id, shallowCopy);
	auto extraFiles = hmdbCommunication::ShallowCopyUtils::extraFiles(type, id, shallowCopy);

	files = utils::mergeOrdered(files, extraFiles);

	return files;
}

bool HMDBShallowCopyLocalContext::load(const DataType type,
	const hmdbServices::ID id)
{
	bool ret = false;

	auto files = filesToProcess(type, id, *shallowCopyContext_->shallowCopy());

	files = filterSupportedFiles(files);

	auto dsT = shallowCopyContext_->dataStatusManager()->transaction();
	files = filterFiles(files, dsT, DataStatus(DataStatus::Local, DataStatus::Unloaded));

	if (files.empty() == false){
		auto t = localContext_->transaction();
		IndexedData indexedData;
		Indexes loadedFiles;
		for (auto it = files.begin(); it != files.end(); ++it){
			t->load(it->second.fileName);
			dsT->updateDataStatus(FileType, it->first, DataStatus(DataStatus::Loaded));
			core::ConstVariantsList loadedData = t->data(it->second.fileName);
			if (loadedData.empty() == false){
				indexedData[it->first] = loadedData;
				loadedFiles.insert(it->first);
			}						
		}

		if (indexedData.empty() == false){
			loadSubjectHierarchy(indexedData);
		}
		else{
			PLUGIN_LOG_DEBUG("Files without data to load");
		}

		dsT->tryUpdate(shallowCopyContext_->shallowCopy());

		ret = true;
	}
	else{
		PLUGIN_LOG_DEBUG("No files to load");
	}

	return ret;
}

bool HMDBShallowCopyLocalContext::loadAll()
{
	return load(FileType, -1);
}

bool hasChild(core::IHierarchyItemConstPtr root, const QString& childName)
{
	for (int i = root->getNumChildren() - 1; i >= 0; --i) {
		if (root->getChild(i)->getName() == childName) {
			return true;
		}
	}
	return false;
}

void HMDBShallowCopyLocalContext::updateOrAddRoot(core::IHierarchyItemConstPtr root, std::set<core::IHierarchyItemConstPtr>& roots,
	core::IDataHierarchyManager::TransactionPtr hierarchyTransaction)
{
	auto it = name2root.find(root->getName());
	if (it != name2root.end()) {
		for (int i = root->getNumChildren() - 1; i >= 0; --i) {
			auto nroot = utils::const_pointer_cast<core::IHierarchyItem>(it->second);
			auto nchild = utils::const_pointer_cast<core::IHierarchyItem>(root->getChild(i));
			if (!hasChild(nroot, nchild->getName())) {
				nroot->appendChild(nchild);
				hierarchyTransaction->updateRoot(nroot);
			}
		}
	}
	else {
		name2root[root->getName()] = root;
		roots.insert(root);
		hierarchyTransaction->addRoot(root);
	}
}

bool HMDBShallowCopyLocalContext::unload(const DataType type,
	const hmdbServices::ID id)
{
	bool ret = false;

	auto files = filesToProcess(type, id, *shallowCopyContext_->shallowCopy());

	files = filterSupportedFiles(files);

	auto dsT = shallowCopyContext_->dataStatusManager()->transaction();
	files = filterFiles(files, dsT, DataStatus(DataStatus::Local, DataStatus::Loaded));

	if (files.empty() == false){
		Indexes unloadFiles;
		IndexedData indexedData;
		auto t = localContext_->transaction();
		for (auto it = files.begin(); it != files.end(); ++it){
			indexedData[it->first] = t->data(it->second.fileName);
			unloadFiles.insert(it->first);
			t->unload(it->second.fileName);
			dsT->updateDataStatus(FileType, it->first, DataStatus(DataStatus::Unloaded));			
		}

		if (unloadFiles.empty() == false){
			unloadSubjectHierarchy(indexedData);

			auto hmdbSource = core::querySource<IHMDBSource>(plugin::getSourceManager());

			if (hmdbSource != nullptr){
				auto hierarchyTransaction = hdm->transaction();
				auto entry = files2roots.find(unloadFiles);
				if (entry != files2roots.end()) {
					auto roots = entry->second;
					for (auto it = roots.begin(); it != roots.end(); ++it) {
						hierarchyTransaction->removeRoot(*it);
						name2root.erase((*it)->getName());
					}

					files2roots.erase(entry);
				}
				else{
					auto locFiles2roots = files2roots;

					//szukam po rootach czy ich przecięcie z aktualną grupą istnieje
					for (auto IT = locFiles2roots.begin(); IT != locFiles2roots.end(); ++IT){
						std::vector<int> inter(std::min(unloadFiles.size(), IT->first.size()));
						auto retIT = std::set_intersection(unloadFiles.begin(), unloadFiles.end(), IT->first.begin(), IT->first.end(), inter.begin());
						if (std::distance(inter.begin(), retIT) == IT->first.size()){
							auto roots = IT->second;
							for (auto it = roots.begin(); it != roots.end(); ++it) {
								hierarchyTransaction->removeRoot(*it);
								name2root.erase((*it)->getName());
							}

							files2roots.erase(IT->first);
						}
					}
				}
			}

		}		

		dsT->tryUpdate(shallowCopyContext_->shallowCopy());

		ret = true;
	}
	else{
		PLUGIN_LOG_DEBUG("No files to unload");
	}

	return ret;
}

bool HMDBShallowCopyLocalContext::unloadAll()
{
	return unload(FileType, -1);
}

void HMDBShallowCopyLocalContext::unloadSubjectHierarchy(const IndexedData & unloadedFiles)
{
	auto shallowCopy = shallowCopyContext_->shallowCopy();

	//buduje mapę hierarchii subject -> session -> motion -> files
	//na bazie tej mapy będę realizować hierarchię pluginu subject

	SubjectFiles subjectHierarchy = groupDataInHierarchy(unloadedFiles, shallowCopy);

	auto transaction = localContext_->transaction();
	for (auto subjectIT = subjectHierarchy.begin(); subjectIT != subjectHierarchy.end(); ++subjectIT){
		//tworzę subject jeżeli to konieczne!!

		core::VariantPtr subOW = findObjectByType(SubjectType, subjectIT->first, typeid(PluginSubject::ISubject));
		PluginSubject::SubjectPtr subPtr;
		subOW->tryGet(subPtr);

		//mam subjecta, mogę iść dalej do sesji
		for (auto sessionIT = subjectIT->second.begin(); sessionIT != subjectIT->second.end(); ++sessionIT){

			core::VariantPtr sOW = findObjectByType(SessionType, sessionIT->first, typeid(PluginSubject::ISession));
			PluginSubject::SessionPtr sPtr;
			sOW->tryGet(sPtr);

			//mam sesję - mogę iść dalej z motionami!!
			for (auto motionIT = sessionIT->second.second.begin(); motionIT != sessionIT->second.second.end(); ++motionIT){
				core::VariantPtr mOW = findObjectByType(MotionType, motionIT->first, typeid(PluginSubject::IMotion));
				PluginSubject::MotionPtr mPtr;
				mOW->tryGet(mPtr);

				//czy usunalem calego motiona?
				if (motionIT->second.size() == shallowCopy->motionShallowCopy.trials.find(motionIT->first)->second->files.size()){
					auto data = data_[MotionType][motionIT->first];
					auto t = localContext_->transaction();
					for (auto it = data.begin(); it != data.end(); ++it){
						 t->unload(*it);
					}

					data_[MotionType].erase(motionIT->first);
					if (data_[MotionType].empty() == true){
						data_.erase(MotionType);
					}

					sPtr->removeMotion(mOW);
				}
				else{
					for (auto it = motionIT->second.begin(); it != motionIT->second.end(); ++it){
						auto data = unloadedFiles.find(*it)->second;
						for (auto dIT = data.begin(); dIT != data.end(); ++dIT){
							mPtr->removeData(*dIT);
						}
					}
				}
			}

			core::ConstVariantsList motions;
			sPtr->getMotions(motions);

			if (motions.empty() == true){

				auto data = data_[SessionType][sessionIT->first];
				auto t = localContext_->transaction();
				for (auto it = data.begin(); it != data.end(); ++it){
					t->unload(*it);
				}

				data_[SessionType].erase(sessionIT->first);
				if (data_[SessionType].empty() == true){
					data_.erase(SessionType);
				}

				subPtr->removeSession(sOW);
			}
		}

		core::ConstVariantsList sessions;
		subPtr->getSessions(sessions);

		if (sessions.empty() == true){

			auto data = data_[SubjectType][subjectIT->first];
			auto t = localContext_->transaction();
			for (auto it = data.begin(); it != data.end(); ++it){
				t->unload(*it);
			}

			data_[SubjectType].erase(subjectIT->first);
			if (data_[SubjectType].empty() == true){
				data_.erase(SubjectType);
			}

			auto it = data_.find(PatientType);
			if (it != data_.end()){
				auto pIT = it->second.find(subPtr->getID());
				if (pIT != it->second.end()){

					auto data = pIT->second;
					for (auto d : data){
						t->unload(d);
					}

					it->second.erase(pIT);
				}

				if (it->second.empty() == true){
					data_.erase(it);
				}
			}
		}
	}
}

IHMDBShallowCopyDataContextPtr HMDBShallowCopyLocalContext::shallowCopyContext()
{
	return shallowCopyContext_;
}

IHMDBShallowCopyDataContextConstPtr HMDBShallowCopyLocalContext::shallowCopyContext() const
{
	return shallowCopyContext_;
}

IHMDBLocalContextPtr HMDBShallowCopyLocalContext::localContext()
{
	return localContext_;
}

IHMDBLocalContextConstPtr HMDBShallowCopyLocalContext::localContext() const
{
	return localContext_;
}

HMDBShallowCopyRemoteContext::HMDBShallowCopyRemoteContext(IHMDBShallowCopyDataContextPtr shallowCopyContext,
	IHMDBRemoteContextPtr remoteContext) : shallowCopyContext_(shallowCopyContext),
	remoteContext_(remoteContext)
{

}

HMDBShallowCopyRemoteContext::~HMDBShallowCopyRemoteContext()
{

}

std::list<IHMDBRemoteContext::DownloadOperationPtr> HMDBShallowCopyRemoteContext::prepareDownload(const DataType type,
	const hmdbServices::ID id, const bool recursive, const bool forceDownload)
{
	return std::list<IHMDBRemoteContext::DownloadOperationPtr>();
}

HMDBShallowCopyRemoteContext::SynchronizeOperationPtr HMDBShallowCopyRemoteContext::prepareSynchronization(IHMDBStoragePtr storage)
{
	auto since = hmdbServices::DateTime::now();
	//dla pewności przesuwam się w przyszłość
	//jak nie mam shallow to ściągam wszystko więc przyrost mi nie jest potrzebny
	since.setYear(since.getYear() + 1);

	if (shallowCopyContext_ != nullptr){
		if (shallowCopyContext_->shallowCopy() != nullptr){
			since = shallowCopyContext_->shallowCopy()->motionShallowCopy.timestamp;
		}
	}

	auto sDownloads = remoteContext()->prepareSynchronization(since);
	return SynchronizeOperationPtr(new hmdbCommunication::SynchronizeOperation(sDownloads, storage));
}

void updateIncrementalShallowCopyStatus(const hmdbServices::IncrementalBranchShallowCopy::ShallowCopy & ishallowCopy,
	const DataStatus & dataStatus, IHMDBStatusManager::TransactionPtr transaction)
{
	for (auto it = ishallowCopy.files.begin(); it != ishallowCopy.files.end(); ++it){
		transaction->updateDataStatus(FileType, (*it).fileID, dataStatus);
	}

	for (auto it = ishallowCopy.trials.begin(); it != ishallowCopy.trials.end(); ++it){
		transaction->updateDataStatus(MotionType, (*it).trialID, dataStatus);
	}

	for (auto it = ishallowCopy.sessions.begin(); it != ishallowCopy.sessions.end(); ++it){
		transaction->updateDataStatus(SessionType, (*it).sessionID, dataStatus);
	}

	for (auto it = ishallowCopy.performers.begin(); it != ishallowCopy.performers.end(); ++it){
		transaction->updateDataStatus(SubjectType, (*it).performerID, dataStatus);
	}
}

void HMDBShallowCopyRemoteContext::synchronize(const ShallowCopyConstPtr shallowCopy,
	const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy)
{
	shallowCopyContext_->setShallowCopy(shallowCopy);
	if (incrementalBranchShallowCopy != nullptr){
		auto t = shallowCopyContext_->dataStatusManager()->transaction();
		updateIncrementalShallowCopyStatus(incrementalBranchShallowCopy->added, DataStatus(DataStatus::Remote, DataStatus::Unloaded, DataStatus::Valid), t);
		updateIncrementalShallowCopyStatus(incrementalBranchShallowCopy->modified, DataStatus(DataStatus::Outdated), t);
		//TODO
		//co robic z takimi danymi?
		updateIncrementalShallowCopyStatus(incrementalBranchShallowCopy->removedLocaly, DataStatus(DataStatus::Outdated), t);
		updateIncrementalShallowCopyStatus(incrementalBranchShallowCopy->removedGlobaly, DataStatus(DataStatus::Outdated), t);
	}
}

void HMDBShallowCopyRemoteContext::synchronize(const SynchronizeOperationPtr downloadOperations)
{	
	downloadOperations->start();
	try {
		downloadOperations->wait();		
	} catch (std::exception& e) {
		PLUGIN_LOG_WARNING(e.what());
	} catch (...) {
		
	}
		
	if (downloadOperations->status() == threadingUtils::IOperation::Finished){		
		auto sc = downloadOperations->shallowCopy();
		auto isc = downloadOperations->incrementalBranchShallowCopy();
		synchronize(sc, isc);
	}
}

IHMDBShallowCopyDataContextPtr HMDBShallowCopyRemoteContext::shallowCopyContext()
{
	return shallowCopyContext_;
}

IHMDBShallowCopyDataContextConstPtr HMDBShallowCopyRemoteContext::shallowCopyContext() const
{
	return shallowCopyContext_;
}

IHMDBRemoteContextPtr HMDBShallowCopyRemoteContext::remoteContext()
{
	return remoteContext_;
}

IHMDBRemoteContextConstPtr HMDBShallowCopyRemoteContext::remoteContext() const
{
	return remoteContext_;
}

HMDBShallowCopyContext::HMDBShallowCopyContext(IHMDBShallowCopyDataContextPtr scdc,
	IHMDBShallowCopyLocalContextPtr sclc,
	IHMDBShallowCopyRemoteContextPtr scrc)
	: dataContext_(scdc), localContext_(sclc), remoteContext_(scrc)
{

}

HMDBShallowCopyContext::~HMDBShallowCopyContext()
{

}

IHMDBShallowCopyDataContextPtr HMDBShallowCopyContext::shallowCopyDataContext()
{
	return dataContext_;
}

IHMDBShallowCopyDataContextConstPtr HMDBShallowCopyContext::shallowCopyDataContext() const
{
	return dataContext_;
}

IHMDBShallowCopyLocalContextPtr HMDBShallowCopyContext::shallowCopyLocalContext()
{
	return localContext_;
}

IHMDBShallowCopyLocalContextConstPtr HMDBShallowCopyContext::shallowCopyLocalContext() const
{
	return localContext_;
}

IHMDBShallowCopyRemoteContextPtr HMDBShallowCopyContext::shallowCopyRemoteContext()
{
	return remoteContext_;
}

IHMDBShallowCopyRemoteContextConstPtr HMDBShallowCopyContext::shallowCopyRemoteContext() const
{
	return remoteContext_;
}
