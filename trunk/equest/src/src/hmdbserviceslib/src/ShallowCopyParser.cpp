#include <hmdbserviceslib/ShallowCopyParser.h>
#include <utils/Debug.h>
#include "XMLHelper.h"
#include <hmdbserviceslib/DateTimeUtils.h>
#include <hmdbserviceslib/Entity.h>
#include <utils/StreamTools.h>

namespace hmdbServices
{

	void extractFiles(tinyxml2::XMLElement * element, MotionShallowCopy::Files & outputFiles, MotionShallowCopy::Files & srcFiles,
		MotionShallowCopy::Session * session, MotionShallowCopy::Trial * trial)
	{
		//Session files
		auto files_element = element->FirstChildElement("Files");
		if (files_element != nullptr) {
			auto file_element = files_element->FirstChildElement("File");
			while (file_element != nullptr) {
				MotionShallowCopy::File * file = nullptr;

				int fileID = -1;

				XMLHelper::extractAttributeValue(file_element, "FileID", fileID);

				auto fileIT = srcFiles.find(fileID);

				if (fileIT == srcFiles.end()){
					file = new MotionShallowCopy::File;
					file->fileID = fileID;
					srcFiles.insert({ fileID, file });
				}
				else{
					file = fileIT->second;
				}

				XMLHelper::extractShallowCopyFile(file_element, *file);
				file->trial = trial;
				file->session = session;

				outputFiles.insert({ fileID, file });

				file_element = file_element->NextSiblingElement();
			}
		}
	}

	bool MotionShallowCopyParser::parseFile(std::istream & document, MotionShallowCopy::ShallowCopy & shallowCopy)
	{
		auto s = utils::StreamTools::read(document);

		tinyxml2::XMLDocument xmlDocument;

		xmlDocument.Parse(s.c_str());

		if (xmlDocument.Error()) {
			UTILS_ASSERT(false, "Blad wczytania pliku MotionShallowCopy");
			return false;
		}

		auto element = xmlDocument.FirstChildElement();
		if (element == nullptr) {
			UTILS_ASSERT(false, "Blad wczytania z pliku MotionShallowCopy");
			return false;
		}		

		//data utworzenia płytkiej kopii

		std::string date;
		XMLHelper::extractAttributeValue(element, "LastModified", date);		

		//mamy date to ja rozpakowujemy
		shallowCopy.timestamp = toTime(date);

		//Performers
		auto performers_element = element->FirstChildElement("Performers");
		if (performers_element != nullptr) {
			auto performer_element = performers_element->FirstChildElement("Performer");
			while (performer_element != nullptr) {
				//newMotionShallowCopy::Performer * performer = new MotionShallowCopy::Performer;
				MotionShallowCopy::Performer * performer = nullptr;

				int perfID = -1;
				XMLHelper::extractAttributeValue(performer_element, "PerformerID", perfID);				

				auto perfIT = shallowCopy.performers.find(perfID);
				if (perfIT == shallowCopy.performers.end()){
					performer = new MotionShallowCopy::Performer;
					performer->performerID = perfID;
					shallowCopy.performers[perfID] = performer;
					performer->patient = nullptr;
				}
				else{
					performer = perfIT->second;
				}

				XMLHelper::extractShallowCopyAttributes(performer_element, performer->attrs);				

				performer_element = performer_element->NextSiblingElement();
			}
		}

		//Sessions
		auto sessions_element = element->FirstChildElement("Sessions");
		if (sessions_element != nullptr) {
			auto session_element = sessions_element->FirstChildElement("Session");
			while (session_element != nullptr) {				
				MotionShallowCopy::Session * session = nullptr;

				int sessionID = -1;
				XMLHelper::extractAttributeValue(session_element, "SessionID", sessionID);				

				auto sessionIT = shallowCopy.sessions.find(sessionID);
				if (sessionIT == shallowCopy.sessions.end()){
					session = new MotionShallowCopy::Session;
					session->sessionID = sessionID;
					shallowCopy.sessions[sessionID] = session;
				}
				else{
					session = sessionIT->second;
				}

				session->performerConf = nullptr;
				session->groupAssigment = nullptr;

				XMLHelper::extractAttributeValue(session_element, "UserID", session->userID);
				XMLHelper::extractAttributeValue(session_element, "LabID", session->labID);
				XMLHelper::extractAttributeValue(session_element, "MotionKind", session->motionKind);
				
				{
					std::string sessionDate;
					XMLHelper::extractAttributeValue(session_element, "SessionDate", sessionDate);					
					session->sessionDate = toTime(sessionDate);
				}

				XMLHelper::extractAttributeValue(session_element, "SessionName", session->sessionName);
				XMLHelper::extractAttributeValue(session_element, "Tags", session->tags);
				XMLHelper::extractAttributeValue(session_element, "SessionDescription", session->sessionDescription);
				XMLHelper::extractAttributeValue(session_element, "EMGConf", session->emgConf);				

				XMLHelper::extractShallowCopyAttributes(session_element, session->attrs);				


				extractFiles(session_element, session->files, shallowCopy.files, session, nullptr);

				session_element = session_element->NextSiblingElement();
			}
		}
		//GroupAssignments
		auto group_assignments_element = element->FirstChildElement("GroupAssignments");
		if (group_assignments_element != nullptr) {
			auto group_assignment_element = group_assignments_element->FirstChildElement("GroupAssignment");
			while (group_assignment_element != nullptr) {
				MotionShallowCopy::GroupAssigment * group_assignment = nullptr;

				int sessionGroupID = -1;
				XMLHelper::extractAttributeValue(group_assignment_element, "SessionGroupID", sessionGroupID);

				auto groupAssigmentIT = shallowCopy.groupAssigments.find(sessionGroupID);

				if (groupAssigmentIT == shallowCopy.groupAssigments.end()){
					group_assignment = new MotionShallowCopy::GroupAssigment;
					group_assignment->sessionGroupID = sessionGroupID;
					shallowCopy.groupAssigments[sessionGroupID] = group_assignment;
				}
				else{
					group_assignment = groupAssigmentIT->second;
				}

				int sessionID = -1;
				XMLHelper::extractAttributeValue(group_assignment_element, "SessionID", sessionID);				

				auto sessionIT = shallowCopy.sessions.find(sessionID);

				if (sessionIT != shallowCopy.sessions.end()){
					sessionIT->second->groupAssigment = group_assignment;
					group_assignment->sessions[sessionIT->first] = sessionIT->second;
				}

				group_assignment_element = group_assignment_element->NextSiblingElement();
			}
		}
		//Trials
		auto trials_element = element->FirstChildElement("Trials");
		if (trials_element != nullptr) {
			auto trial_element = trials_element->FirstChildElement("Trial");
			while (trial_element != nullptr) {				
				MotionShallowCopy::Trial * trial = nullptr;

				int sessionID = -1;
				int trialID = -1;
				XMLHelper::extractAttributeValue(trial_element, "TrialID", trialID);

				auto trialIT = shallowCopy.trials.find(trialID);

				if (trialIT == shallowCopy.trials.end()){
					trial = new MotionShallowCopy::Trial;
					trial->trialID = trialID;
					shallowCopy.trials[trialID] = trial;
				}
				else{
					trial = trialIT->second;
				}

				XMLHelper::extractAttributeValue(trial_element, "SessionID", sessionID);				
				trial->session = shallowCopy.sessions[sessionID];

				UTILS_ASSERT(trial->session != nullptr);

				trial->session->trials[trial->trialID] = trial;

				XMLHelper::extractAttributeValue(trial_element, "TrialName", trial->trialName);
				XMLHelper::extractAttributeValue(trial_element, "TrialDescription", trial->trialDescription);

				XMLHelper::extractShallowCopyAttributes(trial_element, trial->attrs);

				extractFiles(trial_element, trial->files, shallowCopy.files, nullptr, trial);

				trial_element = trial_element->NextSiblingElement();
			}
		}

		//PerformerConfs
		auto performer_consfs_element = element->FirstChildElement("PerformerConfs");
		if (performer_consfs_element != nullptr) {
			auto performer_consf_element = performer_consfs_element->FirstChildElement("PerformerConf");
			while (performer_consf_element != nullptr) {				
				MotionShallowCopy::PerformerConf * performerConf = nullptr;

				int perfConfID = -1;
				XMLHelper::extractAttributeValue(performer_consf_element, "PerformerConfID", perfConfID);				

				auto perfConfIT = shallowCopy.performerConfs.find(perfConfID);

				if (perfConfIT == shallowCopy.performerConfs.end()){
					performerConf = new MotionShallowCopy::PerformerConf;
					performerConf->performerConfID = perfConfID;
					shallowCopy.performerConfs[performerConf->performerConfID] = performerConf;
				}
				else{
					performerConf = perfConfIT->second;
				}

				int id = -1;
				XMLHelper::extractAttributeValue(performer_consf_element, "SessionID", id);

				performerConf->session = shallowCopy.sessions[id];
				UTILS_ASSERT(performerConf->session != nullptr);

				XMLHelper::extractAttributeValue(performer_consf_element, "PerformerID", id);				

				performerConf->performer = shallowCopy.performers[id];
				UTILS_ASSERT(performerConf->performer != nullptr);

				//update sesji
				performerConf->session->performerConf = performerConf;
				//update performera
				performerConf->performer->performerConfs[performerConf->performerConfID] = performerConf;

				XMLHelper::extractShallowCopyAttributes(performer_consf_element, performerConf->attrs);

				performer_consf_element = performer_consf_element->NextSiblingElement();
			}
		}

		return true;
	}

	bool MedicalShallowCopyParser::parseFile(std::istream & document, MedicalShallowCopy::ShallowCopy & shallowCopy)
	{
		auto s = utils::StreamTools::read(document);

		tinyxml2::XMLDocument xmlDocument;

		xmlDocument.Parse(s.c_str());

		if (xmlDocument.Error()) {
			UTILS_ASSERT(false, "Blad wczytania pliku MotionShallowCopy");
			return false;
		}

		auto element = xmlDocument.FirstChildElement();
		if (element == nullptr) {
			UTILS_ASSERT(false, "Blad wczytania z pliku MotionShallowCopy");
			return false;
		}

		//Słowniki
		auto dictionary_elements = element->FirstChildElement("Dictionaries");
		if (dictionary_elements != nullptr){
			//Disorders
			auto disorder_elements = dictionary_elements->FirstChildElement("Disorders");
			if (disorder_elements != nullptr){
				auto disorder_element = disorder_elements->FirstChildElement("Disorder");
				while (disorder_element != nullptr){
					int disorderID = -1;
					XMLHelper::extractAttributeValue(disorder_element, "DisorderID", disorderID);					

					auto it = shallowCopy.disorders.find(disorderID);

					if (it == shallowCopy.disorders.end()){
						it = shallowCopy.disorders.insert({ disorderID, new MedicalShallowCopy::Disorder }).first;
						it->second->disorderID = disorderID;
					}

					XMLHelper::extractAttributeValue(disorder_element, "DisorderName", it->second->name);

					disorder_element = disorder_element->NextSiblingElement();
				}
			}

			//TODO
			//ExamGroups <- aktualnie niewykorzystywane
		}

		//Patients
		auto patient_elements = element->FirstChildElement("Patients");
		if (patient_elements != nullptr) {
			auto patient_element = patient_elements->FirstChildElement("Patient");
			while (patient_element != nullptr) {
				MedicalShallowCopy::Patient * patient = nullptr;

				int patientID = -1;
				XMLHelper::extractAttributeValue(patient_element, "PatientID", patientID);				

				auto perfIT = shallowCopy.patients.find(patientID);
				if (perfIT == shallowCopy.patients.end()){
					patient = new MedicalShallowCopy::Patient;
					patient->patientID = patientID;
					shallowCopy.patients[patientID] = patient;
					patient->performer = nullptr;
				}
				else{
					patient = perfIT->second;
				}

				XMLHelper::extractAttributeValue(patient_element, "BDRPerformerID", patient->motionPerformerID);
				XMLHelper::extractAttributeValue(patient_element, "FirstName", patient->name);
				XMLHelper::extractAttributeValue(patient_element, "LastName", patient->surname);
				XMLHelper::extractAndConvertAttributeValue<xmlWsdl::Gender>(patient_element, "Gender", patient->gender);

				{
					std::string birthDate;
					XMLHelper::extractAttributeValue(patient_element, "BirthDate", birthDate);					
					patient->birthDate = toTime(birthDate);
				}

				patient_element = patient_element->NextSiblingElement();
			}
		}

		//DisordersOccurences
		auto disorder_elements = element->FirstChildElement("DisorderOccurences");
		if (disorder_elements != nullptr) {
			auto disorder_element = disorder_elements->FirstChildElement("DisorderOccurence");
			while (disorder_element != nullptr) {

				int patientID = -1;
				XMLHelper::extractAttributeValue(disorder_element, "PatientID", patientID);				

				int disorderID = -1;
				XMLHelper::extractAttributeValue(disorder_element, "DisorderID", disorderID);

				auto & disorder = shallowCopy.patients[patientID]->disorders[disorderID];
				shallowCopy.patients[patientID]->disorders[disorderID].disorder = shallowCopy.disorders.find(disorderID)->second;
				XMLHelper::extractAttributeValue(disorder_element, "Focus", disorder.focus);				

				{
					std::string diagnosisDate;
					XMLHelper::extractAttributeValue(disorder_element, "DiagnosisDate", diagnosisDate);
					disorder.diagnosisDate = toTime(diagnosisDate);					
				}

				XMLHelper::extractAttributeValue(disorder_element, "Comments", disorder.comments);				

				disorder_element = disorder_element->NextSiblingElement();
			}
		}

		return true;
	}
}