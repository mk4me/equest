#include "CommunicationPCH.h"
#include "CURLFTPStatus.h"

using namespace hmdbCommunication;

CURLFTPStatus::CURLFTPStatus()
: status_(threadingUtils::IOperation::Initialized)
{

}

CURLFTPStatus::~CURLFTPStatus()
{

}

const threadingUtils::IOperation::Status CURLFTPStatus::status() const
{
	return status_;
}

void CURLFTPStatus::setStatus(const threadingUtils::IOperation::Status status)
{
	status_ = status;
}

void CURLFTPStatus::setError(const std::string & error)
{
	error_ = error;
}

const std::string & CURLFTPStatus::error() const
{
	return error_;
}