#include "CommunicationPCH.h"
#include "HMDBStatusManager.h"
#include <corelib/PluginCommon.h>
#include <corelib/IStreamManagerReader.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>

using namespace hmdbCommunication;

class HMDBStatusManagerTransaction : public threadingUtils::TransactionImplHelper<IHMDBStatusManager::transaction_type, HMDBStatusManager::TransactionSharedState>
{
public:

	HMDBStatusManagerTransaction(HMDBStatusManager::SharedStatePtr sharedState) : ImplType(sharedState)
	{
		
	}

	virtual ~HMDBStatusManagerTransaction()
	{

	}
	
	virtual const DataStatus dataStatus(const DataType dataType,
		const hmdbServices::ID id) const
	{
		DataStatus ret;
		auto it = sharedState().statuses.find(dataType);
		if (it != sharedState().statuses.end()){
			auto sit = it->second.find(id);
			if (sit != it->second.end()){
				ret = sit->second;
			}
		}

		return ret;
	}
	
	virtual void setDataStatus(const DataType dataType,
		const hmdbServices::ID id, const DataStatus & dataStatus)
	{
		sharedState().statuses[dataType][id] = dataStatus;
		sharedState().updateData[dataType].insert(id);
	}

	virtual void updateDataStatus(const DataType dataType,
		const hmdbServices::ID id, const DataStatus & dataStatus)
	{
		auto it = sharedState().statuses.find(dataType);
		if (it != sharedState().statuses.end()){
			auto IT = it->second.find(id);
			if (IT != it->second.end()){

				if (dataStatus.storage() != DataStatus::UnknownStorage){
					IT->second.setStorage(dataStatus.storage());
				}

				if (dataStatus.usage() != DataStatus::UnknownUsage){
					IT->second.setUsage(dataStatus.usage());
				}

				if (dataStatus.validity() != DataStatus::UnknownValidity){
					IT->second.setValidity(dataStatus.validity());
				}

				sharedState().updateData[dataType].insert(id);
				return;
			}
		}

		setDataStatus(dataType, id, dataStatus);
	}

	virtual void removeDataStatus(const DataType dataType,
		const hmdbServices::ID id)
	{
		auto it = sharedState().statuses.find(dataType);
		if (it != sharedState().statuses.end()){
			it->second.erase(id);
		}
	}


	virtual void removeDataStatus(const DataType dataType)
	{
		sharedState().statuses.erase(dataType);
	}

	virtual void tryUpdate(const ShallowCopyConstPtr shallowCopy)
	{
		if (shallowCopy != nullptr && sharedState().updateData.empty() == false){

			auto fud = fullUpdateData(shallowCopy);

			auto it = fud.find(MotionType);

			if (it != fud.end()){
				for (auto mit = it->second.begin(); mit != it->second.end(); ++mit){
					auto ds = sharedState().statuses[MotionType][*mit];
					auto motion = shallowCopy->motionShallowCopy.trials.find(*mit);

					if (motion == shallowCopy->motionShallowCopy.trials.end()){
						continue;
					}

					auto nds = DataStatus(ds.validity());
					for (auto fit = motion->second->files.begin(); fit != motion->second->files.end() && nds != DataStatus::PartialStatus; ++fit){
						nds |= sharedState().statuses[FileType][fit->first];
					}

					sharedState().statuses[MotionType][*mit] = nds;
				}
			}

			it = fud.find(SessionType);

			if (it != fud.end()){
				for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
					auto ds = sharedState().statuses[SessionType][*sit];
					auto session = shallowCopy->motionShallowCopy.sessions.find(*sit);

					if (session == shallowCopy->motionShallowCopy.sessions.end()){
						continue;
					}

					auto nds = DataStatus(ds.validity());
					for (auto fit = session->second->files.begin(); fit != session->second->files.end() && nds != DataStatus::PartialStatus; ++fit){
						nds |= sharedState().statuses[FileType][fit->first];
					}

					for (auto tit = session->second->trials.begin(); tit != session->second->trials.end() && nds != DataStatus::PartialStatus; ++tit){
						nds |= sharedState().statuses[MotionType][tit->first];
					}

					sharedState().statuses[SessionType][*sit] = nds;
				}
			}

			it = fud.find(SubjectType);

			if (it != fud.end()){
				for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
					auto ds = sharedState().statuses[SubjectType][*sit];
					auto subject = shallowCopy->motionShallowCopy.performers.find(*sit);

					if (subject == shallowCopy->motionShallowCopy.performers.end()){
						continue;
					}

					auto nds = DataStatus(ds.validity());
					for (auto pit = subject->second->performerConfs.begin(); pit != subject->second->performerConfs.end() && nds != DataStatus::PartialStatus; ++pit){
						nds |= sharedState().statuses[SessionType][pit->second->session->sessionID];
					}

					sharedState().statuses[SubjectType][*sit] = nds;
				}
			}

			it = fud.find(PatientType);

			if (it != fud.end()){
				for (auto pit = it->second.begin(); pit != it->second.end(); ++pit){
					auto ds = sharedState().statuses[PatientType][*pit];
					auto patient = shallowCopy->medicalShallowCopy.patients.find(*pit);
					auto nds = DataStatus(ds.validity());
					sharedState().statuses[PatientType][*pit] = patient->second->performer != nullptr ? nds | sharedState().statuses[SubjectType][patient->second->performer->performerID] : ds;
				}
			}

			HMDBStatusManager::UpdateData().swap(sharedState().updateData);
		}
	}

	virtual void rebuild(const IHMDBStorageConstPtr storage, const ShallowCopyConstPtr shallowCopy)
	{
		HMDBStatusManager::Statuses locStatuses;

		{
			auto t = plugin::getStreamDataManagerReader()->transaction();
			auto keys = storage->transaction()->keys();

			for (auto it = shallowCopy->motionShallowCopy.files.begin();
				it != shallowCopy->motionShallowCopy.files.end(); ++it){

				auto st = keys.find(it->second->fileName) != keys.end() ? DataStatus::Local : DataStatus::Remote;
				auto usage = DataStatus::Unloaded;
				auto sName = core::IStreamManagerReader::streamName(storage->protocol(), it->second->fileName);
				if (t->isManaged(sName) == true){
					usage = DataStatus::PartiallyLoaded;
					if (t->isLoadedCompleately(sName) == true){
						usage = DataStatus::Loaded;
					}
				}

				auto validity = DataStatus::Valid;

				auto sIT = sharedState().statuses[FileType].find(it->first);
				if (sIT != sharedState().statuses[FileType].end()){
					validity = sIT->second.validity();
				}

				locStatuses[FileType][it->first] = DataStatus(st, usage, validity);
			}
		}

		for (auto it = shallowCopy->motionShallowCopy.trials.begin();
			it != shallowCopy->motionShallowCopy.trials.end(); ++it){

			auto validity = DataStatus::UnknownValidity;
			auto mIT = sharedState().statuses[MotionType].find(it->first);
			if (mIT != sharedState().statuses[MotionType].end()){
				validity = mIT->second.validity();
			}

			DataStatus mDs(validity);

			for (auto fit = it->second->files.begin();
				fit != it->second->files.end() && mDs != DataStatus::PartialStatus; ++fit){
				mDs |= locStatuses[FileType][fit->first];
			}

			locStatuses[MotionType][it->first] = mDs;
		}

		for (auto it = shallowCopy->motionShallowCopy.sessions.begin();
			it != shallowCopy->motionShallowCopy.sessions.end(); ++it){

			auto validity = DataStatus::UnknownValidity;
			auto mIT = sharedState().statuses[SessionType].find(it->first);
			if (mIT != sharedState().statuses[SessionType].end()){
				validity = mIT->second.validity();
			}

			DataStatus mDs(validity);

			for (auto fit = it->second->files.begin();
				fit != it->second->files.end() && mDs != DataStatus::PartialStatus; ++fit){
				mDs |= locStatuses[FileType][fit->first];
			}

			for (auto fit = it->second->trials.begin();
				fit != it->second->trials.end() && mDs != DataStatus::PartialStatus; ++fit){
				mDs |= locStatuses[MotionType][fit->first];
			}

			locStatuses[SessionType][it->first] = mDs;
		}

		for (auto it = shallowCopy->motionShallowCopy.performers.begin();
			it != shallowCopy->motionShallowCopy.performers.end(); ++it){

			auto validity = DataStatus::UnknownValidity;
			auto mIT = sharedState().statuses[SubjectType].find(it->first);
			if (mIT != sharedState().statuses[SubjectType].end()){
				validity = mIT->second.validity();
			}

			DataStatus mDs(validity);

			for (auto fit = it->second->performerConfs.begin();
				fit != it->second->performerConfs.end() && mDs != DataStatus::PartialStatus; ++fit){
				mDs |= locStatuses[SessionType][fit->second->session->sessionID];
			}

			locStatuses[SubjectType][it->first] = mDs;
		}

		for (auto it = shallowCopy->medicalShallowCopy.patients.begin();
			it != shallowCopy->medicalShallowCopy.patients.end(); ++it){

			auto validity = DataStatus::UnknownValidity;
			auto mIT = sharedState().statuses[PatientType].find(it->first);
			if (mIT != sharedState().statuses[PatientType].end()){
				validity = mIT->second.validity();
			}

			DataStatus mDs(validity);

			if (it->second->performer != nullptr){
				auto sit = locStatuses[SubjectType].find(it->second->performer->performerID);
				if (sit != locStatuses[SubjectType].end()){
					mDs |= sit->second;
				}
			}

			locStatuses[PatientType][it->first] = mDs;
		}


		//teraz brakujace wpisy
		for (auto it = sharedState().statuses.begin(); it != sharedState().statuses.end(); ++it){

			if (it->second.empty() == false){
				locStatuses[it->first];
				auto dIT = locStatuses.find(it->first);

				for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
					if (dIT->second.find(sit->first) == dIT->second.end()){
						dIT->second.insert(*sit);
					}
				}
			}
		}

		//teraz podmieniam
		locStatuses.swap(sharedState().statuses);
		HMDBStatusManager::UpdateData().swap(sharedState().updateData);
	}

private:

	const HMDBStatusManager::UpdateData fullUpdateData(const ShallowCopyConstPtr shallowCopy)
	{
		HMDBStatusManager::UpdateData lud(sharedState().updateData);

		auto it = sharedState().updateData.find(FileType);
		if (it != sharedState().updateData.end()){
			for (auto fit = it->second.begin(); fit != it->second.end(); ++fit){
				auto fIT = shallowCopy->motionShallowCopy.files.find(*fit);
				if (fIT != shallowCopy->motionShallowCopy.files.end()){
					if (fIT->second->isSessionFile() == true){
						lud[SessionType].insert(fIT->second->session->sessionID);
						sharedState().updateData[SessionType].insert(fIT->second->session->sessionID);
					}
					else{
						lud[MotionType].insert(fIT->second->trial->trialID);
						sharedState().updateData[MotionType].insert(fIT->second->trial->trialID);
					}
				}
			}
		}

		it = sharedState().updateData.find(MotionType);
		if (it != sharedState().updateData.end()){
			for (auto mit = it->second.begin(); mit != it->second.end(); ++mit){
				auto mIT = shallowCopy->motionShallowCopy.trials.find(*mit);
				if (mIT != shallowCopy->motionShallowCopy.trials.end()){
					lud[SessionType].insert(mIT->second->session->sessionID);
					sharedState().updateData[SessionType].insert(mIT->second->session->sessionID);
				}
			}
		}

		it = sharedState().updateData.find(SessionType);
		if (it != sharedState().updateData.end()){
			for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
				auto sIT = shallowCopy->motionShallowCopy.sessions.find(*sit);
				if (sIT != shallowCopy->motionShallowCopy.sessions.end()){
					lud[SubjectType].insert(sIT->second->performerConf->performer->performerID);
					sharedState().updateData[SubjectType].insert(sIT->second->performerConf->performer->performerID);
				}
			}
		}

		it = sharedState().updateData.find(SubjectType);
		if (it != sharedState().updateData.end()){
			for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
				auto sIT = shallowCopy->motionShallowCopy.performers.find(*sit);
				if (sIT != shallowCopy->motionShallowCopy.performers.end()
					&& sIT->second->patient != nullptr){

					lud[PatientType].insert(sIT->second->patient->patientID);
				}
			}
		}

		return lud;
	}
};

HMDBStatusManager::HMDBStatusManager() : sharedState(utils::make_shared<TransactionSharedState>())
{

}

HMDBStatusManager::~HMDBStatusManager()
{

}

HMDBStatusManager::TransactionPtr HMDBStatusManager::transaction()
{
	return HMDBStatusManager::TransactionPtr(new HMDBStatusManagerTransaction(sharedState));
}

HMDBStatusManager::TransactionConstPtr HMDBStatusManager::transaction() const
{
	return HMDBStatusManager::TransactionConstPtr(new HMDBStatusManagerTransaction(sharedState));
}