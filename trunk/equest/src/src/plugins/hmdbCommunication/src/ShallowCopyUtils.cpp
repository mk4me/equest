#include "CommunicationPCH.h"
#include "ShallowCopyUtils.h"

#include <hmdbserviceslib/ShallowCopyParser.h>
#include <hmdbserviceslib/MetadataParser.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <regex>

using namespace hmdbCommunication;

void ShallowCopyUtils::createShallowCopy(hmdbCommunication::ShallowCopy & shallowCopy, std::istream * motionShallowCopy,
	std::istream * motionMetadata, std::istream * medicalShallowCopy,
	std::istream * medicalMetadata)
{
	using namespace hmdbServices;

	hmdbCommunication::ShallowCopy tmpShallowCopy;

	//parsujemy
	MotionShallowCopyParser::parseFile(*motionShallowCopy, tmpShallowCopy.motionShallowCopy);
	MedicalShallowCopyParser::parseFile(*medicalShallowCopy, tmpShallowCopy.medicalShallowCopy);
	MotionMetadataParser::parseFile(*motionMetadata, tmpShallowCopy.motionMetaData);
	MedicalMetadataParser::parseFile(*medicalMetadata, tmpShallowCopy.medicalMetaData);

	//mamy teoretycznie wszystko - teraz próbuje połączyć obie bazy
	auto subjectsITEnd = tmpShallowCopy.motionShallowCopy.performers.end();	
	for (auto & p : tmpShallowCopy.medicalShallowCopy.patients){
		auto subjectIT = tmpShallowCopy.motionShallowCopy.performers.find(p.second->motionPerformerID);
		if (subjectIT != subjectsITEnd){
			p.second->performer = subjectIT->second;
			subjectIT->second->patient = p.second;
		}
	}

	// jeśli wszystko się powiodło podmieniamy wartości
	std::swap(shallowCopy, tmpShallowCopy);
}

ShallowCopyPtr ShallowCopyUtils::extractShallowCopy(const std::string & userHash,
	const IHMDBStorage::TransactionConstPtr storage)
{
	ShallowCopyPtr ret(new ShallowCopy);

	auto stream = shallowCopyStream(userHash, IHMDBRemoteContext::Medical, Shallow, storage);
	if (stream != nullptr){
		hmdbServices::MedicalShallowCopyParser::parseFile(*stream, ret->medicalShallowCopy);
	}

	stream = shallowCopyStream(userHash, IHMDBRemoteContext::Medical, Meta, storage);
	if (stream != nullptr){
		hmdbServices::MedicalMetadataParser::parseFile(*stream, ret->medicalMetaData);
	}

	stream = shallowCopyStream(userHash, IHMDBRemoteContext::Motion, Shallow, storage);
	if (stream != nullptr){
		hmdbServices::MotionShallowCopyParser::parseFile(*stream, ret->motionShallowCopy);
	}

	stream = shallowCopyStream(userHash, IHMDBRemoteContext::Motion, Meta, storage);
	if (stream != nullptr){
		hmdbServices::MotionMetadataParser::parseFile(*stream, ret->motionMetaData);
	}

	return ret;
}

bool ShallowCopyUtils::checkShallowCopyIntegrity(const ShallowCopy & shallowCopy)
{
	//TODO
	//sprawdzać integralność danych w płytkiej kopii bazy danych
	return true;
}

bool ShallowCopyUtils::shallowCopyRequiresRefresh(const ShallowCopy & shallowCopy,
	const hmdbServices::DateTime & currentVersion)
{
	return ( (shallowCopy.motionShallowCopy.timestamp < currentVersion) ? true : false);
}

bool ShallowCopyUtils::shallowCopyInStorage(const std::string & userHash,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType,
	const IHMDBStorage::TransactionConstPtr storage)
{
	return storage->exists(shallowCopyName(userHash, dataReference, shallowType));
}

void ShallowCopyUtils::removeAllShallowCopies(IHMDBStorage::TransactionPtr storage)
{
	auto scfs = allShallowCopiesNames(storage);

	for (const auto & k : scfs){
		storage->remove(k);
	}
}

void ShallowCopyUtils::storeShallowCopy(const std::string & userHash,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType, IHMDBStorageOperations::IStreamPtr stream,
	IHMDBStorage::TransactionPtr storage)
{
	const auto scname = shallowCopyName(userHash, dataReference, shallowType);
	if (scname.empty() == false){
		storage->set(scname, stream);
	}
}

IHMDBStorageOperations::IStreamPtr ShallowCopyUtils::shallowCopyStream(const std::string & userHash,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType,
	const IHMDBStorage::TransactionConstPtr storage)
{
	return storage->get(shallowCopyName(userHash, dataReference, shallowType));
}

std::string ShallowCopyUtils::shallowCopyName(const std::string & userHash,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType)
{
	std::string ret("m");

	if (dataReference == IHMDBRemoteContext::Medical){
		ret += "ed";
	}

	ret += "_" + userHash + "_";

	switch (shallowType)
	{
	case Shallow:
		ret += "s";
		break;

	case Meta:
		ret += "m";
		break;

	case IncrementalShallow:
		ret += "incs";
		break;

	case IncrementalMeta:
		ret += "incm";
		break;

	default:
		std::string().swap(ret);
		break;
	}

	if (ret.empty() == false){
		ret += ".xml.shallow";
	}

	return ret;
}

std::list<std::string> ShallowCopyUtils::allShallowCopiesNames(IHMDBStorage::TransactionConstPtr storage)
{
	static const std::regex r("^(med|m)_[[::alpha::]*?_[inc](s|m)\.xml\.shallow$");

	std::list<std::string> ret;

	const auto sfiles = storage->keys();

	for (const auto & f : sfiles){
		std::smatch what;
		if (std::regex_match(f, what, r) == true){

			ret.push_back(f);
		}
	}

	return ret;
}

std::list<std::string> ShallowCopyUtils::filterShallowCopiesNames(const std::list<std::string> & src,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType)
{
	std::string start;

	switch (dataReference){
	case IHMDBRemoteContext::Motion:
		start = "m";
		break;
	case IHMDBRemoteContext::Medical:
		start = "med";
		break;
	}

	std::string end;

	switch (shallowType)
	{
	case ShallowCopyUtils::Shallow:
		end = "s";
		break;
	case ShallowCopyUtils::Meta:
		end = "m";
		break;
	case ShallowCopyUtils::IncrementalShallow:
		end = "incs";
		break;
	case ShallowCopyUtils::IncrementalMeta:
		end = "incm";
		break;
	}

	std::regex r("^" + start + "_[[::alpha::]*?_" + end + "\.xml\.shallow$");

	std::list<std::string> ret;

	for (const auto & f : src){
		std::smatch what;
		if (std::regex_match(f, what, r) == true){

			ret.push_back(f);
		}
	}

	return ret;
}

std::list<std::string> ShallowCopyUtils::removeUserShallowCopiesNames(const std::list<std::string> & src,
	const std::string & userHash)
{
	std::list<std::string> ret;

	for (const auto & f : src){

		auto s = f.find("_") + 1;
		auto e = f.rfind("_");

		if (f.substr(s, e - s) != userHash){
			ret.push_back(f);
		}
	}

	return ret;
}

std::string ShallowCopyUtils::userHash(const std::string & user, const std::string & password)
{
	std::string hashTmp(user + ":" + password);
	std::string hash;
	hash.resize(hashTmp.size());

	for (int i = 0; i < hashTmp.size(); ++i){
		hash[i] = hashTmp[(i + 5) % hashTmp.size()];
	}

	return hash;
}

ShallowCopyUtils::StorageUserFiles ShallowCopyUtils::groupedShallowCopiesNames(IHMDBStorage::TransactionConstPtr storage)
{
	StorageUserFiles ret;

	const auto sfiles = allShallowCopiesNames(storage);

	for (const auto & f : sfiles){
		IHMDBRemoteContext::FileDescriptor fd;
		fd.fileSize = -1;
		fd.fileName = f;
		fd.id.dataReference = IHMDBRemoteContext::Motion;
		int fileID = 0;

		auto spos = f.find("med_");
		if (spos == 0){
			fd.id.dataReference = IHMDBRemoteContext::Medical;
			spos = 4;
		}
		else{
			spos = 2;
		}

		auto epos = f.find_last_of(".xml.shallow") - 1;

		if (epos == 'm'){
			fd.id.fileID = MetadataFileID;
		}
		else{
			fd.id.fileID = ShallowCopyFileID;
		}

		const std::string userHash(f.substr(spos, epos - spos - 1));

		ret[userHash][fileID] = fd;
	}

	return ret;
}

void extractFiles(StorageFileNames & storageFiles,
	const hmdbServices::MotionShallowCopy::Files & files)
{
	for (const auto & f : files){
		IHMDBRemoteContext::FileDescriptor fd;
		fd.id.fileID = f.first;
		fd.fileName = f.second->fileName;
		fd.fileSize = f.second->fileSize;
		fd.id.dataReference = IHMDBRemoteContext::Motion;
		storageFiles[f.first] = fd;
	}
}

void extractFiles(StorageFileNames & storageFiles,
	const hmdbServices::MotionShallowCopy::Trial * motion)
{
	extractFiles(storageFiles, motion->files);
}

void extractFiles(StorageFileNames & storageFiles,
	const hmdbServices::MotionShallowCopy::Session * session, const bool recursive)
{
	extractFiles(storageFiles, session->files);

	if (recursive == true){
		for (const auto & t : session->trials){
			extractFiles(storageFiles, t.second);
		}
	}
}

void extractFiles(StorageFileNames & storageFiles,
	const hmdbServices::MotionShallowCopy::Performer * subject, const bool recursive)
{
	if (recursive == true){
		for (const auto & pc : subject->performerConfs){
			extractFiles(storageFiles, pc.second->session, true);
		}
	}
}

void extractFiles(StorageFileNames & storageFiles,
	const hmdbServices::MedicalShallowCopy::Patient * patient, const bool recursive)
{
	if (recursive == true && patient->performer != nullptr){
		extractFiles(storageFiles, patient->performer, true);
	}
}

StorageFileNames ShallowCopyUtils::files(const ShallowCopy & shallowCopy)
{
	StorageFileNames ret;

	extractFiles(ret, shallowCopy.motionShallowCopy.files);

	return ret;
}

StorageFileNames ShallowCopyUtils::files(const DataType dataType,
	const ShallowCopy & shallowCopy, const bool recursive)
{
	StorageFileNames ret;

	switch (dataType){
	case FileType:
		ret = files(shallowCopy);
		break;

	case MotionType:

		for (const auto & t : shallowCopy.motionShallowCopy.trials){
			extractFiles(ret, t.second);
		}

		break;

	case SessionType:

		for (const auto & s : shallowCopy.motionShallowCopy.sessions){
			extractFiles(ret, s.second, recursive);
		}

		break;

	case SubjectType:

		for (const auto & p : shallowCopy.motionShallowCopy.performers){
			extractFiles(ret, p.second, recursive);
		}
		break;

	case PatientType:

		for (const auto & p : shallowCopy.medicalShallowCopy.patients){
			extractFiles(ret, p.second, recursive);
		}
		break;
	}

	return ret;
}

StorageFileNames ShallowCopyUtils::files(const DataType dataType,
	const hmdbServices::ID id, const ShallowCopy & shallowCopy, const bool recursive)
{
	StorageFileNames ret;

	switch (dataType)
	{
	case FileType:
	{
		auto it = shallowCopy.motionShallowCopy.files.find(id);
		if (it != shallowCopy.motionShallowCopy.files.end()){
			IHMDBRemoteContext::FileDescriptor fd;
			fd.id.fileID = id;
			fd.fileName = it->second->fileName;
			fd.fileSize = it->second->fileSize;
			fd.id.dataReference = IHMDBRemoteContext::Motion;
			ret.insert(StorageFileNames::value_type(id,fd));
		}
	}
		break;

	case MotionType:
	{
		auto it = shallowCopy.motionShallowCopy.trials.find(id);
		if (it != shallowCopy.motionShallowCopy.trials.end()){
			extractFiles(ret, it->second);
		}
	}
		break;

	case SessionType:
	{
		auto it = shallowCopy.motionShallowCopy.sessions.find(id);
		if (it != shallowCopy.motionShallowCopy.sessions.end()){
			extractFiles(ret, it->second, recursive);
		}
	}
		break;

	case SubjectType:
	{
		auto it = shallowCopy.motionShallowCopy.performers.find(id);
		if (it != shallowCopy.motionShallowCopy.performers.end()){
			extractFiles(ret, it->second, recursive);
		}
	}
		break;

	case PatientType:
	{
		auto it = shallowCopy.medicalShallowCopy.patients.find(id);
		if (it != shallowCopy.medicalShallowCopy.patients.end()){
			extractFiles(ret, it->second, recursive);
		}
	}
		break;
	}

	return ret;
}

StorageFileNames ShallowCopyUtils::extraFiles(const DataType dataType,
	const ShallowCopy & shallowCopy)
{
	StorageFileNames ret;

	switch (dataType)
	{
	case FileType:
	{
		for (const auto & f : shallowCopy.motionShallowCopy.files){
			if (f.second->session != nullptr){
				extractFiles(ret, f.second->session, false);
			}
		}
	}
		break;

	case MotionType:
	{
		for (const auto & t : shallowCopy.motionShallowCopy.trials){
			if (t.second->session != nullptr){
				extractFiles(ret, t.second->session, false);
			}
		}
	}
		break;
	}

	return ret;
}

StorageFileNames ShallowCopyUtils::extraFiles(const DataType dataType,
	const hmdbServices::ID id, const ShallowCopy & shallowCopy)
{
	StorageFileNames ret;

	switch (dataType)
	{
	case FileType:
	{
		auto it = shallowCopy.motionShallowCopy.files.find(id);
		if (it != shallowCopy.motionShallowCopy.files.end()){
			if (it->second->session != nullptr){
				extractFiles(ret, it->second->session, false);
			}
		}
	}
		break;

	case MotionType:
	{
		auto it = shallowCopy.motionShallowCopy.trials.find(id);
		if (it != shallowCopy.motionShallowCopy.trials.end()){
			if (it->second->session != nullptr){
				extractFiles(ret, it->second->session, false);
			}
		}
	}
		break;
	}

	return ret;
}