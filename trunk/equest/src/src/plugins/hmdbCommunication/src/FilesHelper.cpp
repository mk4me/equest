#include "CommunicationPCH.h"
#include "FilesHelper.h"
#include "DataSourceStatusManager.h"
#include <corelib/IParserManagerReader.h>
#include <corelib/PluginCommon.h>

using namespace hmdbCommunication;
using namespace hmdbServices;

void FilesHelper::getDisorderFiles(int disorderID, const ShallowCopy & shallowCopy, std::set<int> & files)
{
	auto disorderIT = shallowCopy.medicalShallowCopy.disorders.find(disorderID);
	if (disorderIT == shallowCopy.medicalShallowCopy.disorders.end()){
		throw std::runtime_error("Disorder not found");
	}

	getFiles(disorderIT->second, shallowCopy, files);
}

void FilesHelper::getPatientFiles(int patientID, const ShallowCopy & shallowCopy, std::set<int> & files)
{
	auto patientIT = shallowCopy.medicalShallowCopy.patients.find(patientID);

	if (patientIT == shallowCopy.medicalShallowCopy.patients.end()){
		throw std::runtime_error("Patient not found");
	}

	getFiles(patientIT->second, files);
}

void FilesHelper::getSubjectFiles(int subjectID, const ShallowCopy & shallowCopy, std::set<int> & files)
{
	auto subjectIT = shallowCopy.motionShallowCopy.performers.find(subjectID);
	if (subjectIT == shallowCopy.motionShallowCopy.performers.end()){
		throw std::runtime_error("Subject not found");
	}

	getFiles(subjectIT->second, files);
}

void FilesHelper::getSessionSpecificFiles(int sessionID, const ShallowCopy & shallowCopy, std::set<int> & files)
{
	auto sessionIT = shallowCopy.motionShallowCopy.sessions.find(sessionID);
	if (sessionIT == shallowCopy.motionShallowCopy.sessions.end()){
		throw std::runtime_error("Session not found");
	}

	getSpecificFiles(sessionIT->second, files);
}

void FilesHelper::getSessionFiles(int sessionID, const ShallowCopy & shallowCopy, std::set<int> & files)
{
	auto sessionIT = shallowCopy.motionShallowCopy.sessions.find(sessionID);
	if (sessionIT == shallowCopy.motionShallowCopy.sessions.end()){
		throw std::runtime_error("Session not found");
	}

	getFiles(sessionIT->second, files);
}

void FilesHelper::getMotionFiles(int motionID, const ShallowCopy & shallowCopy, std::set<int> & files)
{
	auto motionIT = shallowCopy.motionShallowCopy.trials.find(motionID);
	if (motionIT == shallowCopy.motionShallowCopy.trials.end()){
		throw std::runtime_error("Session not found");
	}

	getFiles(motionIT->second, files);
}

void FilesHelper::filterFiles(const std::set<int> & inFiles, DataStorage storage, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager)
{
	std::vector<int> localFilteredFiles;
	auto filesITEnd = inFiles.end();
	for (auto fileIT = inFiles.begin(); fileIT != filesITEnd; ++fileIT){
		if (filesStatusManager.fileStorage(*fileIT) == storage){
			localFilteredFiles.push_back(*fileIT);
		}
	}

	filteredFiles.insert(localFilteredFiles.begin(), localFilteredFiles.end());
}

void FilesHelper::filterFiles(const std::set<int> & inFiles, DataUsage usage, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager)
{
	std::vector<int> localFilteredFiles;
	auto filesITEnd = inFiles.end();
	for (auto fileIT = inFiles.begin(); fileIT != filesITEnd; ++fileIT){
		if (filesStatusManager.fileUsage(*fileIT) == usage){
			localFilteredFiles.push_back(*fileIT);
		}
	}

	filteredFiles.insert(localFilteredFiles.begin(), localFilteredFiles.end());
}

void FilesHelper::filterFiles(const std::set<int> & inFiles, const DataStatus & status, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager)
{
	std::vector<int> localFilteredFiles;
	auto filesITEnd = inFiles.end();
	for (auto fileIT = inFiles.begin(); fileIT != filesITEnd; ++fileIT){
		const auto & s = filesStatusManager.fileStatus(*fileIT);
		if (s.storage() == status.storage() && s.usage() == status.usage()){
			localFilteredFiles.push_back(*fileIT);
		}
	}

	filteredFiles.insert(localFilteredFiles.begin(), localFilteredFiles.end());
}

void FilesHelper::filterFiles(const std::set<int> & inFiles, std::set<int> & filteredFiles, const FileStatusManager & filesStatusManager)
{
	std::vector<int> localFilteredFiles;
	auto filesITEnd = inFiles.end();
	for (auto fileIT = inFiles.begin(); fileIT != filesITEnd; ++fileIT){
		if (plugin::getParserManagerReader()->sourceIsAccepted(filesStatusManager.filePath(*fileIT).string()) == true){
			localFilteredFiles.push_back(*fileIT);
		}
	}

	filteredFiles.insert(localFilteredFiles.begin(), localFilteredFiles.end());
}

void FilesHelper::getFiles(const MedicalShallowCopy::Disorder * disorder, const ShallowCopy & shallowCopy, std::set<int> & files)
{
	auto patientsByDisordersIT = shallowCopy.medicalShallowCopy.patientsByDisorder.find(disorder->disorderID);
	if (patientsByDisordersIT != shallowCopy.medicalShallowCopy.patientsByDisorder.end()){
		auto patientsITEnd = patientsByDisordersIT->second.end();
		for (auto patientIT = patientsByDisordersIT->second.begin(); patientIT != patientsITEnd; ++patientIT){
			getFiles(*patientIT, files);
		}
	}
}

void FilesHelper::getFiles(const MedicalShallowCopy::Patient * patient, std::set<int> & files)
{
	if (patient->performer != nullptr){
		getFiles(patient->performer, files);
	}
}

void FilesHelper::getFiles(const MotionShallowCopy::Performer * subject, std::set<int> & files)
{
	auto perfConfsITEnd = subject->performerConfs.end();
	for (auto perfConfIT = subject->performerConfs.begin(); perfConfIT != perfConfsITEnd; ++perfConfIT){
		getFiles(perfConfIT->second->session, files);
	}
}

void FilesHelper::getFiles(const MotionShallowCopy::Session * session, std::set<int> & files)
{
	getSpecificFiles(session, files);

	auto motionsITEnd = session->trials.end();
	for (auto motionIT = session->trials.begin(); motionIT != motionsITEnd; ++motionIT){
		getFiles(motionIT->second, files);
	}
}

void FilesHelper::getFiles(const MotionShallowCopy::Trial * motion, std::set<int> & files)
{
	auto filesITEnd = motion->files.end();
	for (auto fileIT = motion->files.begin(); fileIT != filesITEnd; ++fileIT){
		files.insert(fileIT->first);
	}
}

void FilesHelper::getSpecificFiles(const hmdbServices::MotionShallowCopy::Session * session, std::set<int> & files)
{
	auto filesITEnd = session->files.end();
	for (auto fileIT = session->files.begin(); fileIT != filesITEnd; ++fileIT){
		files.insert(fileIT->first);
	}
}

const unsigned long long FilesHelper::filesSize(const std::set<int> & filesIDs, const hmdbCommunication::ShallowCopy & shallowCopy)
{
	unsigned long long ret = 0;	

	for (auto it = filesIDs.begin(); it != filesIDs.end(); ++it){
		ret += fileSize(*it, shallowCopy);
	}

	return ret;
}

const unsigned long long FilesHelper::fileSize(const int fileID, const hmdbCommunication::ShallowCopy & shallowCopy)
{
	auto fileIT = shallowCopy.motionShallowCopy.files.find(fileID);
	if (fileIT == shallowCopy.motionShallowCopy.files.end()){
		throw std::runtime_error("File does not exist in ShallowCopy");
	}

	return fileIT->second->fileSize;
}