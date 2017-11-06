#include <hmdbserviceslib/FileStoremanWS.h>

namespace hmdbServices
{
	SimpleFileStoremanWS::SimpleFileStoremanWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	SimpleFileStoremanWS::~SimpleFileStoremanWS()
	{
	}

	void SimpleFileStoremanWS::downloadComplete(const int id, const std::string & path)
	{
		wsdlService->setOperation("DownloadComplete");
		wsdlService->setValue("fileID", id);
		wsdlService->setValue("path", path);
		wsdlService->invoke();
	}

	xmlWsdl::FileData SimpleFileStoremanWS::retrieve(const int id)
	{
		wsdlService->setOperation("RetrieveFile");
		wsdlService->setValue("fileID", id);
		wsdlService->invoke(true);

		xmlWsdl::FileData ret;
		wsdlService->getValue("FileLocation", ret.fileLocation);
		wsdlService->getValue("SubdirPath", ret.subdirPath);

		return ret;
	}

	PhotoStoremanWS::PhotoStoremanWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	PhotoStoremanWS::~PhotoStoremanWS()
	{
	}

	void PhotoStoremanWS::downloadComplete(const int id, const std::string & path)
	{
		wsdlService->setOperation("DownloadComplete");
		wsdlService->setValue("photoID", id);
		wsdlService->setValue("path", path);
		wsdlService->invoke();
	}

	xmlWsdl::FileData PhotoStoremanWS::retrieve(const int id)
	{
		wsdlService->setOperation("RetrievePhoto");
		wsdlService->setValue("photoID", id);
		wsdlService->invoke(true);

		xmlWsdl::FileData ret;
		wsdlService->getValue("FileLocation", ret.fileLocation);
		wsdlService->getValue("SubdirPath", ret.subdirPath);

		return ret;
	}

	ShallowStoremanWS::ShallowStoremanWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	ShallowStoremanWS::~ShallowStoremanWS()
	{
	}

	std::string ShallowStoremanWS::getShallowCopy()
	{
		wsdlService->setOperation("GetShallowCopy");
		wsdlService->invoke(true);

		std::string ret;
		wsdlService->getValue("GetShallowCopyResult", ret);

		return ret;
	}

	std::string ShallowStoremanWS::getMetadata()
	{
		wsdlService->setOperation("GetMetadata");
		wsdlService->invoke(true);

		std::string ret;
		wsdlService->getValue("GetMetadataResult", ret);

		return ret;
	}

	void ShallowStoremanWS::downloadComplete(const std::string & path)
	{
		SimpleFileStoremanWS(wsdlService).downloadComplete(0, path);
	}

	std::string ShallowStoremanWS::getShallowCopyIncrement(const std::string & dateTime)
	{
		wsdlService->setOperation("GetShallowCopyIncrement");
		wsdlService->setValue("since", dateTime);
		wsdlService->invoke(true);

		std::string ret;
		wsdlService->getValue("GetShallowCopyIncrementResult", ret);

		return ret;
	}

	std::string ShallowStoremanWS::getShallowCopyBranchesIncrement(const std::string & dateTime)
	{
		wsdlService->setOperation("GetShallowCopyBranchesIncrement");
		std::string date = dateTime;
		//std::string date = "2013-11-13T10:39:35Z";
		wsdlService->setValue("since", date);
		wsdlService->invoke(true);

		std::string ret;
		wsdlService->getValue("GetShallowCopyBranchesIncrementResult", ret);

		return ret;
	}

	MotionFileStoremanWS::MotionFileStoremanWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	MotionFileStoremanWS::~MotionFileStoremanWS()
	{
	}

	void MotionFileStoremanWS::downloadComplete(const int fileID, const std::string & path)
	{
		SimpleFileStoremanWS(wsdlService).downloadComplete(fileID, path);
	}

	void MotionFileStoremanWS::downloadComplete(const std::string & path)
	{
		ShallowStoremanWS(wsdlService).downloadComplete(path);
	}

	xmlWsdl::FileData MotionFileStoremanWS::retrieve(const int fileID)
	{
		return SimpleFileStoremanWS(wsdlService).retrieve(fileID);
	}

	std::string MotionFileStoremanWS::getShallowCopy()
	{
		return ShallowStoremanWS(wsdlService).getShallowCopy();
	}

	std::string MotionFileStoremanWS::getShallowCopyIncrement(const std::string & dateTime)
	{
		return ShallowStoremanWS(wsdlService).getShallowCopyIncrement(dateTime);
	}

	std::string MotionFileStoremanWS::getMetadata()
	{
		return ShallowStoremanWS(wsdlService).getMetadata();
	}

	int MotionFileStoremanWS::storeSessionFile(const int sessionID,
		const std::string & path, const std::string & description,
		const std::string & filename)
	{
		wsdlService->setOperation("StoreSessionFile");
		wsdlService->setValue("sessionId", sessionID);
		wsdlService->setValue("path", path);
		wsdlService->setValue("description", description);
		wsdlService->setValue("filename", filename);
		wsdlService->invoke(true);

		int val = -1;
		wsdlService->getValue("StoreSessionFileResult", val);

		return val;
	}

	void MotionFileStoremanWS::storeSessionFiles(const int sessionID,
		const std::string & path, const std::string & description)
	{
		wsdlService->setOperation("StoreSessionFiles");
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->setValue("path", path);
		wsdlService->setValue("description", description);
		wsdlService->invoke();
	}

	int MotionFileStoremanWS::storeTrialFile(const int trialID,
		const std::string & path, const std::string & description,
		const std::string& filename)
	{
		wsdlService->setOperation("StoreTrialFile");
		wsdlService->setValue("trialID", trialID);
		wsdlService->setValue("path", path);
		wsdlService->setValue("description", description);
		wsdlService->setValue("filename", filename);
		wsdlService->invoke(true);

		int val = -1;
		wsdlService->getValue("StoreTrialFileResult", val);

		return val;
	}

	void MotionFileStoremanWS::storeTrialFiles(const int trialID,
		const std::string & path, const std::string & description)
	{
		wsdlService->setOperation("StoreTrialFiles");
		wsdlService->setValue("trialID", trialID);
		wsdlService->setValue("path", path);
		wsdlService->setValue("description", description);
		wsdlService->invoke();
	}

	int MotionFileStoremanWS::storeMeasurementConfFile(const int mcID,
		const std::string & path, const std::string & description,
		const std::string & filename)
	{
		wsdlService->setOperation("StoreMeasurementConfFile");
		wsdlService->setValue("mcID", mcID);
		wsdlService->setValue("path", path);
		wsdlService->setValue("description", description);
		wsdlService->setValue("filename", filename);
		wsdlService->invoke(true);

		int val = -1;
		wsdlService->getValue("StoreMeasurementConfFileResult", val);

		return val;
	}

	void MotionFileStoremanWS::storeMeasurementConfFiles(const int mcID,
		const std::string & path, const std::string & description)
	{
		wsdlService->setOperation("StoreMeasurementConfFiles");
		wsdlService->setValue("mcID", mcID);
		wsdlService->setValue("path", path);
		wsdlService->setValue("description", description);
		wsdlService->invoke();
	}

	void MotionFileStoremanWS::replaceFile(int fileID, const std::string& path, const std::string& filename)
	{
		wsdlService->setOperation("ReplaceFile");
		wsdlService->setValue("fileID", fileID);
		wsdlService->setValue("path", path);
		wsdlService->setValue("filename", filename);
		wsdlService->invoke();
	}

	std::string MotionFileStoremanWS::getShallowCopyBranchesIncrement(const std::string & dateTime)
	{
		return ShallowStoremanWS(wsdlService).getShallowCopyBranchesIncrement(dateTime);
	}

	MedicalFileStoremanWS::MedicalFileStoremanWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	MedicalFileStoremanWS::~MedicalFileStoremanWS()
	{
	}

	void MedicalFileStoremanWS::downloadComplete(const int fileID,
		const std::string & path)
	{
		PhotoStoremanWS(wsdlService).downloadComplete(fileID, path);
	}

	void MedicalFileStoremanWS::downloadComplete(const std::string & path)
	{
		ShallowStoremanWS(wsdlService).downloadComplete(path);
	}

	xmlWsdl::FileData MedicalFileStoremanWS::retrieve(const int fileID)
	{
		return PhotoStoremanWS(wsdlService).retrieve(fileID);
	}

	std::string MedicalFileStoremanWS::getShallowCopy()
	{
		return ShallowStoremanWS(wsdlService).getShallowCopy();
	}

	std::string MedicalFileStoremanWS::getMetadata()
	{
		return ShallowStoremanWS(wsdlService).getMetadata();
	}

	std::string MedicalFileStoremanWS::getShallowCopyIncrement(const std::string & dateTime)
	{
		return ShallowStoremanWS(wsdlService).getShallowCopyIncrement(dateTime);
	}

	std::string MedicalFileStoremanWS::getShallowCopyBranchesIncrement(const std::string & dateTime)
	{
		return ShallowStoremanWS(wsdlService).getShallowCopyBranchesIncrement(dateTime);
	}
}