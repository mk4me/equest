#include "FileStoremanWS.h"
#include <hmdbserviceslib/FileStoremanWS.h>

using namespace hmdbCommunication;

SimpleFileStoremanWS::SimpleFileStoremanWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

SimpleFileStoremanWS::~SimpleFileStoremanWS()
{
}

void SimpleFileStoremanWS::downloadComplete(const int id, const std::string & path)
{
	hmdbServices::SimpleFileStoremanWS(creator()).downloadComplete(id, path);
}

hmdbServices::xmlWsdl::FileData SimpleFileStoremanWS::retrieve(const int id)
{
	return hmdbServices::SimpleFileStoremanWS(creator()).retrieve(id);
}

PhotoStoremanWS::PhotoStoremanWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

PhotoStoremanWS::~PhotoStoremanWS()
{
}

void PhotoStoremanWS::downloadComplete(const int id, const std::string & path)
{
	hmdbServices::PhotoStoremanWS(creator()).downloadComplete(id, path);
}

hmdbServices::xmlWsdl::FileData PhotoStoremanWS::retrieve(const int id)
{
	return hmdbServices::PhotoStoremanWS(creator()).retrieve(id);
}

ShallowStoremanWS::ShallowStoremanWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

ShallowStoremanWS::~ShallowStoremanWS()
{
}

std::string ShallowStoremanWS::getShallowCopy()
{
	return hmdbServices::ShallowStoremanWS(creator()).getShallowCopy();
}

std::string ShallowStoremanWS::getMetadata()
{
	return hmdbServices::ShallowStoremanWS(creator()).getMetadata();
}

void ShallowStoremanWS::downloadComplete(const std::string & path)
{
	hmdbServices::ShallowStoremanWS(creator()).downloadComplete(path);
}

std::string ShallowStoremanWS::getShallowCopyIncrement(const std::string & dateTime)
{
	return hmdbServices::ShallowStoremanWS(creator()).getShallowCopyIncrement(dateTime);
}

std::string ShallowStoremanWS::getShallowCopyBranchesIncrement(const std::string & dateTime)
{
	return hmdbServices::ShallowStoremanWS(creator()).getShallowCopyBranchesIncrement(dateTime);
}

MotionFileStoremanWS::MotionFileStoremanWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

MotionFileStoremanWS::~MotionFileStoremanWS()
{
}

void MotionFileStoremanWS::downloadComplete(const int fileID, const std::string & path)
{
	hmdbServices::MotionFileStoremanWS(creator()).downloadComplete(fileID, path);
}

void MotionFileStoremanWS::downloadComplete(const std::string & path)
{
	hmdbServices::MotionFileStoremanWS(creator()).downloadComplete(path);
}

hmdbServices::xmlWsdl::FileData MotionFileStoremanWS::retrieve(const int fileID)
{
	return hmdbServices::MotionFileStoremanWS(creator()).retrieve(fileID);
}

std::string MotionFileStoremanWS::getShallowCopy()
{
	return hmdbServices::MotionFileStoremanWS(creator()).getShallowCopy();
}

std::string MotionFileStoremanWS::getShallowCopyIncrement(const std::string & dateTime)
{
	return hmdbServices::MotionFileStoremanWS(creator()).getShallowCopyIncrement(dateTime);
}

std::string MotionFileStoremanWS::getMetadata()
{
	return hmdbServices::MotionFileStoremanWS(creator()).getMetadata();
}

int MotionFileStoremanWS::storeSessionFile(const int sessionID,
	const std::string & path, const std::string & description,
	const std::string & filename)
{
	return hmdbServices::MotionFileStoremanWS(creator()).storeSessionFile(sessionID,
		path, description, filename);
}

void MotionFileStoremanWS::storeSessionFiles(const int sessionID,
	const std::string & path, const std::string & description)
{
	hmdbServices::MotionFileStoremanWS(creator()).storeSessionFiles(sessionID,
		path, description);
}

int MotionFileStoremanWS::storeTrialFile(const int trialID,
	const std::string & path, const std::string & description,
	const std::string& filename)
{
	return hmdbServices::MotionFileStoremanWS(creator()).storeTrialFile(trialID,
		path, description, filename);
}

void MotionFileStoremanWS::storeTrialFiles(const int trialID,
	const std::string & path, const std::string & description)
{
	hmdbServices::MotionFileStoremanWS(creator()).storeTrialFiles(trialID,
		path, description);
}

int MotionFileStoremanWS::storeMeasurementConfFile(const int mcID,
	const std::string & path, const std::string & description,
	const std::string & filename)
{
	return hmdbServices::MotionFileStoremanWS(creator()).storeMeasurementConfFile(mcID,
		path, description, filename);
}

void MotionFileStoremanWS::storeMeasurementConfFiles(const int mcID,
	const std::string & path, const std::string & description)
{
	hmdbServices::MotionFileStoremanWS(creator()).storeMeasurementConfFiles(mcID,
		path, description);
}

void MotionFileStoremanWS::replaceFile(int fileID, const std::string& path, const std::string& filename)
{
	hmdbServices::MotionFileStoremanWS(creator()).replaceFile(fileID,
		path, filename);
}

std::string MotionFileStoremanWS::getShallowCopyBranchesIncrement(const std::string & dateTime)
{
	return hmdbServices::MotionFileStoremanWS(creator()).getShallowCopyBranchesIncrement(dateTime);
}

MedicalFileStoremanWS::MedicalFileStoremanWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

MedicalFileStoremanWS::~MedicalFileStoremanWS()
{
}

void MedicalFileStoremanWS::downloadComplete(const int fileID,
	const std::string & path)
{
	hmdbServices::MedicalFileStoremanWS(creator()).downloadComplete(fileID,
		path);
}

void MedicalFileStoremanWS::downloadComplete(const std::string & path)
{
	hmdbServices::MedicalFileStoremanWS(creator()).downloadComplete(path);
}

hmdbServices::xmlWsdl::FileData MedicalFileStoremanWS::retrieve(const int fileID)
{
	return hmdbServices::MedicalFileStoremanWS(creator()).retrieve(fileID);
}

std::string MedicalFileStoremanWS::getShallowCopy()
{
	return hmdbServices::MedicalFileStoremanWS(creator()).getShallowCopy();
}

std::string MedicalFileStoremanWS::getMetadata()
{
	return hmdbServices::MedicalFileStoremanWS(creator()).getMetadata();
}

std::string MedicalFileStoremanWS::getShallowCopyIncrement(const std::string & dateTime)
{
	return hmdbServices::MedicalFileStoremanWS(creator()).getShallowCopyIncrement(dateTime);
}

std::string MedicalFileStoremanWS::getShallowCopyBranchesIncrement(const std::string & dateTime)
{
	return hmdbServices::MedicalFileStoremanWS(creator()).getShallowCopyBranchesIncrement(dateTime);
}
