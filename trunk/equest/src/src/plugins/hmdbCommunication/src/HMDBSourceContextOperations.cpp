#include "CommunicationPCH.h"
#include "HMDBSourceContextOperations.h"
#include <utils/Filesystem.h>
#include <functional>
#include <corelib/IPlugin.h>
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <hmdbserviceslib/MetadataParser.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <hmdbserviceslib/IncrementalBranchShallowCopyParser.h>
#include <hmdbserviceslib/ShallowCopyParser.h>
#include <hmdbserviceslib/DateTimeUtils.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <plugins/hmdbCommunication/DataStatus.h>
#include <utils/Utils.h>
#include <numeric>
#include <fstream>
#include <corelib/PluginCommon.h>
#include <loglib/Exceptions.h>


using namespace hmdbCommunication;

HMDBStorageProgress::HMDBStorageProgress() : progress_(0.0), aborted_(false)
{

}

HMDBStorageProgress::~HMDBStorageProgress()
{

}

void HMDBStorageProgress::setProgress(const float progress)
{
	UTILS_ASSERT(progress >= progress_);
	progress_ = progress;
}

void HMDBStorageProgress::setError(const std::string & error)
{
	error_ = error;
}

bool HMDBStorageProgress::aborted() const
{
	return aborted_;
}

float HMDBStorageProgress::progress() const
{
	return progress_;
}

std::string HMDBStorageProgress::error() const
{
	return error_;
}

void HMDBStorageProgress::abort()
{
	aborted_ = true;
}


HMDBCompondStorageProgress::HMDBCompondStorageSubProgress::HMDBCompondStorageSubProgress(HMDBCompondStorageProgress * parent)
	: parent(parent), progress_(0.0)
{

}

HMDBCompondStorageProgress::HMDBCompondStorageSubProgress::~HMDBCompondStorageSubProgress()
{

}

void HMDBCompondStorageProgress::HMDBCompondStorageSubProgress::setError(const std::string & error)
{
	error_ = error;
}

void HMDBCompondStorageProgress::HMDBCompondStorageSubProgress::setProgress(const float progress)
{
	UTILS_ASSERT(progress >= progress_);
	progress_ = progress;
}

bool HMDBCompondStorageProgress::HMDBCompondStorageSubProgress::aborted() const
{
	return parent->aborted();
}

float HMDBCompondStorageProgress::HMDBCompondStorageSubProgress::progress() const
{
	return progress_;
}

std::string HMDBCompondStorageProgress::HMDBCompondStorageSubProgress::error() const
{
	return error_;
}

HMDBCompondStorageProgress::HMDBCompondStorageProgress()
	: aborted_(false)
{

}

HMDBCompondStorageProgress::~HMDBCompondStorageProgress()
{

}

void HMDBCompondStorageProgress::setSize(const unsigned int size)
{
	subProgresses.reserve(size);
	for (int i = 0; i < size; ++i){
		subProgresses.push_back(utils::shared_ptr<HMDBCompondStorageSubProgress>(new HMDBCompondStorageSubProgress(this)));
	}
}

HMDBCompondStorageProgress::HMDBCompondStorageSubProgress * HMDBCompondStorageProgress::get(const unsigned int idx)
{
	return subProgresses[idx].get();
}

bool HMDBCompondStorageProgress::aborted() const
{
	return aborted_;
}

float HMDBCompondStorageProgress::progress() const
{
	return ((subProgresses.empty() == true) ? 0.0 : std::accumulate(subProgresses.begin(), subProgresses.end(), 0.0, [](float total, utils::shared_ptr<HMDBCompondStorageSubProgress> d) { return total + d->progress(); }) / subProgresses.size());
}

std::string HMDBCompondStorageProgress::error() const
{
	for (auto it = subProgresses.begin(); it != subProgresses.end(); ++it){
		auto e = (*it)->error();
		if (e.empty() == false){
			return e;
		}
	}

	return std::string();
}

void HMDBCompondStorageProgress::abort()
{
	aborted_ = true;
}

DownloadHelper::DownloadHelper(IHMDBStoragePtr storage, const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads)
	: downloads(downloads), storage(storage), released(false)
{

}

void DownloadHelper::download()
{
	PLUGIN_LOG_DEBUG("Downloading " << downloads.size() << " files.");
	for(auto & d : downloads)
	{
		d->start();
	};
}

void DownloadHelper::wait()
{
	for (auto & d : downloads)
	{
		d->wait();
	};
}

void DownloadHelper::abort()
{
	storeProgress.abort();
	for (auto & d : downloads)
	{
		d->abort();
	};
}

float DownloadHelper::progress() const
{
	const auto pd = std::accumulate(downloads.begin(), downloads.end(), 0.0, [](float total, IHMDBRemoteContext::DownloadOperationPtr d) { return total + d->normalizedProgress(); }) / downloads.size();
	return (pd + storeProgress.progress()) / 2.0;
}

std::list<IHMDBRemoteContext::DownloadOperationPtr> filterCompleteDownloads(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads)
{
	std::list<IHMDBRemoteContext::DownloadOperationPtr> ret;

	for (auto & d : downloads)
	{
		if (d->fileDownloaded() == true){
			ret.push_back(d);
		}
	};

	return ret;
}

void DownloadHelper::filterDownloaded()
{
	downloaded_ = filterCompleteDownloads(downloads);
	PLUGIN_LOG_DEBUG("Prepaered " << downloads.size() << " files for store");
}

const std::list<IHMDBRemoteContext::DownloadOperationPtr> & DownloadHelper::downloaded() const
{
	return downloaded_;
}

void DownloadHelper::store()
{	
	if (downloaded_.empty() == true){
		throw loglib::runtime_error("Downloads failed. Could not synchronize");
	}

	auto t = storage->transaction();
	const auto s = downloaded_.size();

	storeProgress.setSize(s);

	int i = 0;
	for (auto & d : downloaded_)
	{
		if (storeProgress.aborted() == true){
			return;
		}

		try{
			auto progress = storeProgress.get(i++);
			t->set(d->fileID().fileName, d->stream(), progress);
			if (progress->error().empty() == false){
				PLUGIN_LOG_ERROR("Failed storing file: " << d->fileID().fileName << " with error " << progress->error());
			}
		}
		catch (std::exception & e){
			PLUGIN_LOG_ERROR("Failed storing file: " << d->fileID().fileName << " with error " << e.what());
		}
		catch (...){
			PLUGIN_LOG_ERROR("Failed storing file: " << d->fileID().fileName << " with UNKNOWN error");
		}
	}
}

void DownloadHelper::release()
{
	if (released == false){
		released = true;
		for (auto & d : downloads)
		{
			try{
				if (d->fileDownloaded() == true){
					d->release();
				}
			}
			catch (std::exception & e){
				PLUGIN_LOG_ERROR("Failed to release download: " << e.what());
			}
			catch (...){
				PLUGIN_LOG_ERROR("Failed to release download");
			}
		};
	}
}

DownloadHelper::~DownloadHelper()
{
	release();
}

CompoundOperation::CompoundOperation(const ExecutionPolicy ep,
	const ExecutionSchema es,
	const Operations & suboperations)
	: operations(suboperations),
	ep(ep), es(es),
	status_(threadingUtils::IOperation::Initialized)
{

}

CompoundOperation::~CompoundOperation()
{
	abort();
	wait();
}

unsigned int CompoundOperation::size() const
{
	return operations.size();
}

IHMDBRemoteContext::OperationConstPtr CompoundOperation::operation(const unsigned int idx) const
{
	return operations[idx];
}

IHMDBRemoteContext::OperationPtr CompoundOperation::operation(const unsigned int idx)
{
	return operations[idx];
}

ICompoundOperation::ExecutionPolicy CompoundOperation::executionPolicy() const
{
	return ep;
}

ICompoundOperation::ExecutionSchema CompoundOperation::executionSchema() const
{
	return es;
}

void CompoundOperation::start()
{
	if (status_ != threadingUtils::IOperation::Initialized){
		return;
	}

	status_ = threadingUtils::IOperation::Pending;

	if (ep == ICompoundOperation::Any){

		if (es == ICompoundOperation::Serial){
			job = plugin::getJobManager()->create("HMDBSource", "Compound operation", &CompoundOperation::callAnySerial, this);
		}
		else{
			job = plugin::getJobManager()->create("HMDBSource", "Compound operation", &CompoundOperation::callAnyParallel, this);
		}

	}
	else{

		if (es == ICompoundOperation::Serial){
			job = plugin::getJobManager()->create("HMDBSource", "Compound operation", &CompoundOperation::callAllSerial, this);
		}
		else{
			job = plugin::getJobManager()->create("HMDBSource", "Compound operation", &CompoundOperation::callAllParallel, this);
		}
	}

	job.start();
}

void CompoundOperation::abort()
{
	if (status_ != threadingUtils::IOperation::Aborted &&
		status_ != threadingUtils::IOperation::Error &&
		status_ != threadingUtils::IOperation::Finished){

		status_ = threadingUtils::IOperation::Aborted;

		for (auto it = operations.begin(); it != operations.end(); ++it){
			(*it)->abort();			
		}
	}
}

void CompoundOperation::callAnySerial()
{
	if (status_ == threadingUtils::IOperation::Pending){
		status_ = threadingUtils::IOperation::Running;

		for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Running; ++it){
			(*it)->start();
			(*it)->wait();
		}
	}
}

void CompoundOperation::callAnyParallel()
{
	for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Pending; ++it){
		(*it)->start();
	}

	status_ = threadingUtils::IOperation::Running;
}

void CompoundOperation::callAllSerial()
{
	if (status_ == threadingUtils::IOperation::Pending){
		status_ = threadingUtils::IOperation::Running;

		for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Running; ++it){
			(*it)->start();
			(*it)->wait();

			if ((*it)->status() == threadingUtils::IOperation::Error){
				status_ = threadingUtils::IOperation::Error;
				errorOperation = (*it);
				break;
			}
		}
	}
}

void CompoundOperation::observe()
{
	bool finish = false;
	while (finish == false){		

		int opFinished = 0;

		for (auto it = operations.begin(); it != operations.end() && status_ == threadingUtils::IOperation::Running; ++it){
			auto s = (*it)->status();

			if (s == threadingUtils::IOperation::Error){
				if (errorOperation == nullptr){
					errorOperation = (*it);
					status_ = threadingUtils::IOperation::Error;
				}
			}
			else if (s == threadingUtils::IOperation::Aborted){
				status_ = threadingUtils::IOperation::Aborted;
			}
			else if (s == threadingUtils::IOperation::Finished){
				++opFinished;
			}
		}

		if (status_ != threadingUtils::IOperation::Running || opFinished == operations.size()){
			finish = true;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	if (status_ == threadingUtils::IOperation::Error){
		for (auto it = operations.begin(); it != operations.end(); ++it){
			(*it)->abort();
		}
	}
}

void CompoundOperation::callAllParallel()
{
	if (status_ == threadingUtils::IOperation::Pending){
		callAnyParallel();
		observe();
	}
}

void CompoundOperation::wait()
{
	if (status_ != threadingUtils::IOperation::Pending &&
		status_ != threadingUtils::IOperation::Running){
		return;
	}

	for (auto it = operations.begin(); it != operations.end(); ++it){
		(*it)->wait();
	}

	if (workerThread.joinable() == true){
		workerThread.join();		
	}
}

threadingUtils::IOperation::Status CompoundOperation::compoundStatus() const
{
	threadingUtils::IOperation::Status s = IOperation::Initialized;

	int pi = 0;
	int ri = 0;
	int fi = 0;

	for (auto it = operations.begin(); it != operations.end(); ++it)
	{
		auto ss = (*it)->status();
		if (ss == IOperation::Aborted || ss == IOperation::Error){
			s = ss;
			break;
		}
		else if (ss == IOperation::Finished)
		{
			++fi;
		}
		else if (ss == IOperation::Running){
			++ri;
		}
		else if (ss == IOperation::Pending){
			++pi;
		}
	}

	if (s == IOperation::Initialized){
		if (pi == operations.size()){
			s = IOperation::Pending;
		}else if (fi == operations.size()){
			s = IOperation::Finished;
		}else if(ri > 0){	
			s = IOperation::Running;	
		}
	}

	return s;
}

threadingUtils::IOperation::Status CompoundOperation::status() const
{
	if (status_ == threadingUtils::IOperation::Running){
		status_ = compoundStatus();		
	}

	return status_;
}

std::string CompoundOperation::error() const
{
	if (errorOperation == nullptr){
		errorOperation = findErrorOperation();
	}


	return errorOperation == nullptr ? std::string() : errorOperation->error();
}

IHMDBRemoteContext::OperationPtr CompoundOperation::findErrorOperation() const
{
	IHMDBRemoteContext::OperationPtr ret;

	for (auto it = operations.begin(); it != operations.end(); ++it){
		if ((*it)->status() == IOperation::Error){
			ret = (*it);
			break;
		}
	}

	return ret;
}

UploadOperation::UploadOperation(const std::string & fileName,
	IHMDBFtp::TransferPtr transfer,
	HMDBStore store,
	const std::string & fileDescription,
	const std::string & subdirectory)
	: fileName(fileName), store(store), transfer(transfer),
	fileDescription(fileDescription),
	subdirectory(subdirectory), fileID_(-1),
	job(plugin::getJobManager()->create("HMDBSource", "Upload operation", &UploadOperation::upload, this))
{	

}

UploadOperation::~UploadOperation()
{

}

hmdbServices::ID UploadOperation::fileID() const
{
	return fileID_;
}

void UploadOperation::upload()
{
	if (status() != threadingUtils::IOperation::Aborted){
		transfer->start();
		transfer->wait();
	}

	if (status() != threadingUtils::IOperation::Aborted){
		fileID_ = store(fileName, fileDescription, subdirectory);
		progress_ = 1.0f;
		status_ = threadingUtils::IOperation::Finished;
	}
}

void UploadOperation::start()
{	
	job.start();
}

void UploadOperation::wait()
{
	job.wait();
}

void UploadOperation::abort()
{
	status_ = Aborted;
	job.cancel();
}

UploadOperation::Status UploadOperation::status() const
{
	if (status_ == Aborted){
		return status_;
	}

	//return fOp.status();
}

std::string UploadOperation::error() const
{
	//return fOp.error();
	return "";
}

float UploadOperation::normalizedProgress() const
{
	return ((float)progress_ + transfer->normalizedProgress()) / 2.0;
}

std::string retrieveData(hmdbServices::IBasicStoremanWS * service, const int id)
{
	return service->retrieve(id).fileLocation;
}

std::string retrievePerspective(hmdbServices::IShallowStoremanWS * service, const int id,
	const hmdbServices::DateTime & since = hmdbServices::DateTime::now())
{
	std::string ret;
	switch (id)
	{
	case ShallowCopyUtils::ShallowCopyFileID:
		ret = service->getShallowCopy();
		break;

	case ShallowCopyUtils::MetadataFileID:
		ret = service->getMetadata();
		break;

	case ShallowCopyUtils::IncrementalShallowCopyFileID:
		ret = service->getShallowCopyBranchesIncrement(hmdbServices::toString(since));
		break;

	case ShallowCopyUtils::IncrementalMetadataFileID:
		UTILS_ASSERT((false), "Brak operacji w HMDB");
		break;

	default:
		UTILS_ASSERT((false), "Blad identyfikatora pliku perspektywy danych");
		break;
	}

	return ret;
}

PrepareHMDB::PrepareHMDB(hmdbServices::IFileStoremanWS * fs,
	const hmdbServices::ID fileID,
	const hmdbServices::DateTime & since)
	: fs(fs), fileID_(fileID), since(since)
{
	
}

PrepareHMDB::~PrepareHMDB()
{
	clearHMDB();
}

std::string PrepareHMDB::prepareFilePath()
{
	if (fileID_ > -1){
		preparedFilePath_ = retrieveData(fs, fileID_);
	}
	else{
		preparedFilePath_ = retrievePerspective(fs, fileID_, since);
	}

	return preparedFilePath_;
}

//! \return Czyści baze danych ruchu
void PrepareHMDB::clearHMDB()
{
	if (preparedFilePath_.empty() == false){
		const auto tmpPath = preparedFilePath_;
		std::string().swap(preparedFilePath_);
		if (fileID_ > -1){
			dynamic_cast<hmdbServices::IBasicStoremanWS*>(fs)->downloadComplete(fileID_, tmpPath);
		}
		else{
			dynamic_cast<hmdbServices::IShallowStoremanWS*>(fs)->downloadComplete(tmpPath);
		}
	}
}

TmpFileTransferIO::TmpFileTransferIO()
{

}

TmpFileTransferIO::~TmpFileTransferIO()
{
	release();
}

IHMDBStorageOperations::OStreamPtr TmpFileTransferIO::prepareOutput()
{
	int i = 0;
	while (utils::Filesystem::pathExists(tmpFilePath = plugin::getPaths()->getTempFilePath()) == true && i++ < 10) {}

	if (utils::Filesystem::pathExists(tmpFilePath) == false){

		stream.reset(new std::fstream(tmpFilePath.string(), std::ios_base::out));

		if (stream->is_open() == false){
			stream.reset();
		}
		else{
			stream->close();
			stream.reset(new std::fstream(tmpFilePath.string(), std::ios_base::in | std::ios_base::out | std::ios_base::binary));
			if (stream->is_open() == false){
				stream.reset();
			}
		}
	}

	return stream;
}

void TmpFileTransferIO::closeOutput()
{
	if (stream)	{
		stream->close();
		stream.reset();
	} else {
		PLUGIN_LOG_DEBUG("TmpeFileTransferIO : stream problem");
	}

}

void TmpFileTransferIO::release()
{
	if (stream != nullptr){
		stream->close();
		stream.reset();
	}
	if (tmpFilePath.empty() == false){
		utils::Filesystem::deleteFile(tmpFilePath);
		tmpFilePath = utils::Filesystem::Path();
	}
}

IHMDBStorageOperations::IStreamPtr TmpFileTransferIO::openInput()
{
	stream = utils::make_shared<std::fstream>(tmpFilePath.string(), std::ios_base::in | std::ios_base::binary);
	if (stream->is_open() == false){
		PLUGIN_LOG_ERROR("Failed opening file " << tmpFilePath << " with error " << strerror(errno) << "(" << errno << ")");
	}

	auto ret = stream;

	stream.reset();

	return ret;
}

void TmpFileTransferIO::closeInput()
{
	if (stream != nullptr){
		stream->close();
		stream.reset();
	}
}

MemoryTransferIO::MemoryTransferIO()
{

}

MemoryTransferIO::~MemoryTransferIO()
{

}

IHMDBStorageOperations::OStreamPtr MemoryTransferIO::prepareOutput()
{
	stream.reset(new std::stringstream);
	return stream;
}

void MemoryTransferIO::closeOutput()
{
	stream->flush();
}

IHMDBStorageOperations::IStreamPtr MemoryTransferIO::openInput()
{
	stream->seekg(0, std::ios::beg);

	return stream;
}

void MemoryTransferIO::closeInput()
{
	release();
}

void MemoryTransferIO::release()
{
	stream.reset();
}

FileDownload::FileDownload(const IHMDBRemoteContext::FileDescriptor & fileToDownload,
	utils::shared_ptr<PrepareHMDB> prepareHMDB,
	utils::shared_ptr<ITransferIO> transferIO,
	IHMDBFtp * ftp)
	: prepareHMDB(prepareHMDB), transferIO(transferIO), ftp(ftp),
	downloaded_(false), fileID_(fileToDownload),
	job(plugin::getJobManager()->create("HMDBSource", "File download", &FileDownload::download, this)),
	progress_(0)
{	
	
}

FileDownload::~FileDownload()
{
	release();
}

void FileDownload::start()
{	
	job.start();
}

void FileDownload::wait()
{
	//plugin::getJobManager()->waitForOtherJob(job);
	job.wait();
}

void FileDownload::abort()
{
	if (status() != FileDownload::Aborted){
		status_ = FileDownload::Aborted;
		job.cancel();
	}
}

FileDownload::Status FileDownload::status() const
{
	if (status_ == FileDownload::Aborted){
		return status_;
	}

	//return fOp.status();
}

std::string FileDownload::error() const
{
	//return fOp.error();
	return "";
}

float FileDownload::normalizedProgress() const
{
	return ((float)progress_ + (transfer != nullptr ? transfer->normalizedProgress() : 0.0)) / 5.0;
}

IHMDBRemoteContext::FileDescriptor FileDownload::fileID() const
{
	return fileID_;
}

bool FileDownload::fileDownloaded() const
{
	return downloaded_;
}

IHMDBStorageOperations::IStreamPtr FileDownload::stream() const
{
	return transferIO->openInput();
}

void FileDownload::release()
{
	transferIO->release();
}

void FileDownload::download()
{
	try{
		if (status_ == FileDownload::Aborted){
			return;
		}

		auto const filePath = prepareHMDB->prepareFilePath();

		if (filePath.empty() == true){
			throw loglib::runtime_error("Empty file path to retrieve");
		}
		++progress_;

		if (status_ == FileDownload::Aborted){
			prepareHMDB->clearHMDB();
			++progress_;
			return;
		}

		auto output = transferIO->prepareOutput();

		if (output == nullptr){
			prepareHMDB->clearHMDB();
			++progress_;
			throw loglib::runtime_error("Uninitialized output stream");
		}
		++progress_;

		if (status_ == FileDownload::Aborted){
			output.reset();
			transferIO->closeOutput();
			release();
			++progress_;
			prepareHMDB->clearHMDB();
			++progress_;
			return;
		}

		transfer = ftp->prepareGet(filePath, output, fileID_.fileSize);

		output.reset();

		if (transfer == nullptr){
			PLUGIN_LOG_ERROR("Failed preparing ftp for file:" << fileID_.fileName);
			transferIO->closeOutput();
			release();
			++progress_;
			prepareHMDB->clearHMDB();
			++progress_;
			throw loglib::runtime_error("Uninitialized data transfer");
		}

		transfer->start();
		transfer->wait();
		//PLUGIN_LOG_DEBUG("Finished downloading file:" << fileID_.fileName);

		transferIO->closeOutput();

		PLUGIN_LOG_DEBUG("File " << fileID_.fileName << " processed " << transfer->processed() << " / " << transfer->size());

		if (transfer->status() == threadingUtils::IOperation::Finished){
			downloaded_ = true;
			PLUGIN_LOG_DEBUG("Finished downloading file:" << fileID_.fileName);
		}
		else{
			release();
			PLUGIN_LOG_ERROR("Failed downloading file:" << fileID_.fileName << " with status " << transfer->status() <<
				((transfer->status() == threadingUtils::IOperation::Error) ? (" error: " + transfer->error()) : "" ));
		}
		try{
			++progress_;
			prepareHMDB->clearHMDB();
			++progress_;
		}
		catch (std::exception & e)
		{
			PLUGIN_LOG_ERROR("Failed clearing HMDB file " << filePath << " with error " << e.what());
		}
		catch (...)
		{
			PLUGIN_LOG_ERROR("Failed clearing HMDB file " << filePath << " with UNKNOWN error");
		}
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR("Critical fail downloading file:" << fileID_.fileName << " with error: " << e.what());
	}
	catch (...){
		PLUGIN_LOG_ERROR("Critical fail downloading file:" << fileID_.fileName);
	}
}

MultipleFilesDownloadAndStore::MultipleFilesDownloadAndStore(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
	IHMDBStoragePtr storage) : status_(Initialized), downloadHelper(storage, downloads),
	job(plugin::getJobManager()->create("HMDBSource", "Files download", &MultipleFilesDownloadAndStore::download, this))
{

}

MultipleFilesDownloadAndStore::~MultipleFilesDownloadAndStore()
{

}

void MultipleFilesDownloadAndStore::start()
{
	if (status_ != Initialized){
		return;
	}

	try{				
		job.start();
		
	}
	catch (std::exception & e){
		error_ = e.what();
		status_ = Error;
		throw;
	}
	catch (...){
		error_ = "UNKNOWN error while launching multiple files download";
		status_ = Error;
		throw;
	}
}

void MultipleFilesDownloadAndStore::wait()
{
	job.wait();
}

void MultipleFilesDownloadAndStore::abort()
{
	if (status_ != Error && status_ != Aborted){
		status_ = Aborted;
		downloadHelper.abort();
	}
}

MultipleFilesDownloadAndStore::Status MultipleFilesDownloadAndStore::status() const
{
	return status_;
}

std::string MultipleFilesDownloadAndStore::error() const
{
	return error_;
}

float MultipleFilesDownloadAndStore::normalizedProgress() const
{
	return downloadHelper.progress();
}

void MultipleFilesDownloadAndStore::download()
{	
	try{
		utils::Cleanup release(std::bind(&DownloadHelper::release, &downloadHelper));

		if (status_ == Aborted){
			return;
		}

		status_ = Running;

		downloadHelper.download();
		downloadHelper.wait();

		if (status_ == Aborted){
			return;
		}		
		downloadHelper.filterDownloaded();		
		downloadHelper.store();

		if (status_ == Aborted){
			return;
		}
		
		downloadFinished(downloadHelper.downloaded());
		if (status_ == Running){
			status_ = Finished;
		}
	}
	catch (std::exception & e){
		error_ = e.what();
		status_ = Error;
	}
	catch (...){
		error_ = "UNKNOWN error while downloading multiple files";
		status_ = Error;
	}
}

MultipleFilesDownloadStoreAndStatusUpdate::MultipleFilesDownloadStoreAndStatusUpdate(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
	IHMDBStoragePtr storage, IHMDBStatusManagerPtr statusManager, ShallowCopyConstPtr shallowCopy)
	: MultipleFilesDownloadAndStore(downloads, storage), statusManager(statusManager), shallowCopy(shallowCopy)
{

}

MultipleFilesDownloadStoreAndStatusUpdate::~MultipleFilesDownloadStoreAndStatusUpdate()
{

}

void MultipleFilesDownloadStoreAndStatusUpdate::downloadFinished(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & finishedDownloads)
{
	if (finishedDownloads.empty() == true){
		return;
	}

	auto t = statusManager->transaction();

	for(auto d : finishedDownloads)
	{		
		t->updateDataStatus(FileType, d->fileID().id.fileID, DataStatus(DataStatus::Local, DataStatus::UnknownUsage, DataStatus::Valid));
	};

	if (shallowCopy != nullptr){
		statusManager->transaction()->tryUpdate(shallowCopy);
	}
}

ShallowCopyConstPtr ExtractShallowcopy::shallowCopy() const
{
	return shallowCopy_;
}

IncrementalBranchShallowCopyConstPtr ExtractShallowcopy::incrementalBranchShallowCopy() const
{
	return incrementalShallowCopy_;
}

void extractShallowCopy(const hmdbServices::ID shallowID, const IHMDBRemoteContext::DataReference dataReference,
	ShallowCopy & shallowCopy, hmdbServices::IncrementalBranchShallowCopy & incShallowCopy,
	std::istream & stream)
{

	if (dataReference == IHMDBRemoteContext::Motion){

		switch (shallowID)
		{
		case ShallowCopyUtils::IncrementalMetadataFileID:
			UTILS_ASSERT((false), "Incremental metadata not supported for motion database");
			break;

		case ShallowCopyUtils::IncrementalShallowCopyFileID:
			hmdbServices::IncrementalBranchShallowCopyParser::parseFile(stream, incShallowCopy);
			break;

		case ShallowCopyUtils::ShallowCopyFileID:
			hmdbServices::MotionShallowCopyParser::parseFile(stream, shallowCopy.motionShallowCopy);
			break;

		case ShallowCopyUtils::MetadataFileID:
			hmdbServices::MotionMetadataParser::parseFile(stream, shallowCopy.motionMetaData);
			break;
		}
	}
	else{
		switch (shallowID)
		{
		case ShallowCopyUtils::IncrementalMetadataFileID:
			UTILS_ASSERT((false), "Incremental metadata not supported for medical database");
			break;

		case ShallowCopyUtils::IncrementalShallowCopyFileID:
			UTILS_ASSERT((false), "Incremental shallowcopy not supported for medical database");
			break;

		case ShallowCopyUtils::ShallowCopyFileID:
			hmdbServices::MedicalShallowCopyParser::parseFile(stream, shallowCopy.medicalShallowCopy);
			break;

		case ShallowCopyUtils::MetadataFileID:
			hmdbServices::MedicalMetadataParser::parseFile(stream, shallowCopy.medicalMetaData);
			break;
		}
	}
}

void ExtractShallowcopy::extract(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads, IHMDBStorageProgress * progress)
{
	if (downloads.empty() == true){		
		return;
	}

	ShallowCopyPtr locSh(new ShallowCopy);
	IncrementalBranchShallowCopyPtr locIncSh(new hmdbServices::IncrementalBranchShallowCopy);	

	const auto progressStep = 1.0 / (float)downloads.size();
	int currentStep = 1;
	progress->setProgress(0.0);

	for(auto & d : downloads)
	{
		if (progress->aborted() == true){
			return;
		}

		try{

			auto stream = d->stream();
			extractShallowCopy(d->fileID().id.fileID, d->fileID().id.dataReference,
				*locSh, *locIncSh, *stream);

			progress->setProgress(progressStep * currentStep++);
		}
		catch (std::exception & e){

		}
	};

	if (progress->aborted() == false){
		progress->setProgress(1.0);
		shallowCopy_ = locSh;
		incrementalShallowCopy_ = locIncSh;
	}
}

SynchronizeOperationImpl::SynchronizeOperationImpl(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
		IHMDBStoragePtr storage) : MultipleFilesDownloadAndStore(downloads, storage)
{

}

SynchronizeOperationImpl::~SynchronizeOperationImpl()
{

}

void SynchronizeOperationImpl::abort()
{
	extractProgress.abort();
	MultipleFilesDownloadAndStore::abort();
}

float SynchronizeOperationImpl::normalizedProgress() const
{
	return (MultipleFilesDownloadAndStore::normalizedProgress() + extractProgress.progress()) / 2.0;
}

ShallowCopyConstPtr SynchronizeOperationImpl::shallowCopy() const
{
	return shallowCopy_;
}

IncrementalBranchShallowCopyConstPtr SynchronizeOperationImpl::incrementalBranchShallowCopy() const
{
	return incrementalShallowCopy_;
}

void SynchronizeOperationImpl::downloadFinished(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & finishedDownloads)
{
	ExtractShallowcopy eSc;
	
	eSc.extract(finishedDownloads, &extractProgress);	
	if (extractProgress.aborted() == false && extractProgress.error().empty() == true){
		shallowCopy_ = eSc.shallowCopy();
		incrementalShallowCopy_ = eSc.incrementalBranchShallowCopy();
	}
}

SynchronizeOperation::SynchronizeOperation(const std::list<IHMDBRemoteContext::DownloadOperationPtr> & downloads,
	IHMDBStoragePtr storage) : op(downloads, storage)
{
	
}

SynchronizeOperation::~SynchronizeOperation()
{

}

void SynchronizeOperation::start()
{
	op.start();
}

void SynchronizeOperation::wait()
{
	op.wait();
}

void SynchronizeOperation::abort()
{
	op.abort();
}

SynchronizeOperation::Status SynchronizeOperation::status() const
{
	return op.status();
}

std::string SynchronizeOperation::error() const
{
	return op.error();
}

float SynchronizeOperation::normalizedProgress() const
{
	return op.normalizedProgress();
}

ShallowCopyConstPtr SynchronizeOperation::shallowCopy() const
{
	return op.shallowCopy();
}

IncrementalBranchShallowCopyConstPtr SynchronizeOperation::incrementalBranchShallowCopy() const
{
	return op.incrementalBranchShallowCopy();
}
