#include "CommunicationPCH.h"
#include "HMDBRemoteContext.h"
#include "HMDBSourceContextOperations.h"
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include "MemoryStorage.h"

using namespace hmdbCommunication;

HMDBRemoteContext::HMDBRemoteContext(IHMDBSessionPtr session, const std::string & userHash)
	: session_(session), userHash(userHash)
{

}

HMDBRemoteContext::~HMDBRemoteContext()
{

}

IHMDBSessionPtr HMDBRemoteContext::session()
{
	return session_;
}

IHMDBSessionConstPtr HMDBRemoteContext::session() const
{
	return session_;
}

std::list<IHMDBRemoteContext::DownloadOperationPtr> HMDBRemoteContext::prepareSynchronization(const hmdbServices::DateTime & since)
{	
	std::list<IHMDBRemoteContext::DownloadOperationPtr> downloads;	

	if (session_->medicalFilestoreman() != nullptr && session_->medicalFtp() != nullptr){

		IHMDBRemoteContext::FileDescriptor fd;
		fd.id.dataReference = IHMDBRemoteContext::Medical;
		fd.fileSize = 0;
		
		{
			fd.id.fileID = ShallowCopyUtils::MetadataFileID;
			fd.fileName = ShallowCopyUtils::shallowCopyName(userHash, fd.id.dataReference, ShallowCopyUtils::Meta);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->medicalFilestoreman(), fd.id.fileID, since));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(fd, pftdOp, ptoOp, session_->medicalFtp())));			
		}

		{
			fd.id.fileID = ShallowCopyUtils::ShallowCopyFileID;
			fd.fileName = ShallowCopyUtils::shallowCopyName(userHash, fd.id.dataReference, ShallowCopyUtils::Shallow);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->medicalFilestoreman(), fd.id.fileID, since));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(fd, pftdOp, ptoOp, session_->medicalFtp())));			
		}

	}

	if (session_->motionFilestoreman() != nullptr && session_->motionFtp() != nullptr){
		IHMDBRemoteContext::FileDescriptor fd;
		fd.id.dataReference = IHMDBRemoteContext::Motion;
		fd.fileSize = 0;

		{
			fd.id.fileID = ShallowCopyUtils::MetadataFileID;
			fd.fileName = ShallowCopyUtils::shallowCopyName(userHash, fd.id.dataReference, ShallowCopyUtils::Meta);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->motionFilestoreman(), fd.id.fileID, since));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(fd, pftdOp, ptoOp, session_->motionFtp())));
		}

		{
			fd.id.fileID = ShallowCopyUtils::ShallowCopyFileID;
			fd.fileName = ShallowCopyUtils::shallowCopyName(userHash, fd.id.dataReference, ShallowCopyUtils::Shallow);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->motionFilestoreman(), fd.id.fileID, since));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(fd, pftdOp, ptoOp, session_->motionFtp())));			
		}

		{
			fd.id.fileID = ShallowCopyUtils::IncrementalShallowCopyFileID;
			fd.fileName = ShallowCopyUtils::shallowCopyName(userHash, fd.id.dataReference, ShallowCopyUtils::IncrementalShallow);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			//auto since2 = hmdbServices::DateTime(2015, 9, 1);
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->motionFilestoreman(), fd.id.fileID, since));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(fd, pftdOp, ptoOp, session_->motionFtp())));			
		}
	}	

	return downloads;
}

HMDBRemoteContext::DownloadOperationPtr HMDBRemoteContext::prepareFileDownload(const FileDescriptor & fd)
{
	HMDBRemoteContext::DownloadOperationPtr ret;

	hmdbServices::IFileStoremanWS * fsWS = nullptr;
	IHMDBFtp * ftp = nullptr;

	if (fd.id.dataReference == IHMDBRemoteContext::Medical){
		if (session_->medicalFilestoreman() != nullptr && session_->medicalFtp() != nullptr){
			fsWS = session_->medicalFilestoreman();
			ftp = session_->medicalFtp();
		}
	}
	else{
		if (session_->motionFilestoreman() != nullptr && session_->motionFtp() != nullptr){
			fsWS = session_->motionFilestoreman();
			ftp = session_->motionFtp();
		}		
	}

	if (fsWS != nullptr && ftp != nullptr){
		utils::shared_ptr<TmpFileTransferIO> ptoOp(new TmpFileTransferIO());
		utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(fsWS, fd.id.fileID));
		ret.reset(new FileDownload(fd, pftdOp, ptoOp, ftp));
	}

	return ret;
}

IHMDBFtp::TransferPtr HMDBRemoteContext::prepareFileUpload(const std::string & fileName,
	const std::string & path, IHMDBStorageOperations::IStreamPtr source,
	const DataReference dataReference, const hmdbServices::FileSize fileSize)
{

	if (dataReference == Motion){
		return session_->motionFtp()->preparePut(path + "/" + fileName, source, fileSize);
	}
	else{
		return session_->medicalFtp()->preparePut(path + "/" + fileName, source, fileSize);
	}
}