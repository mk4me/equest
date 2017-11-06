/********************************************************************
	created:  2014/05/28	21:42:40
	filename: CURLFTPStatus.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPSTATUS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPSTATUS_H__

#include <threadingUtils/IOperation.h>

namespace hmdbCommunication
{
	class CURLFTPStatus
	{
	public:
		//! Konstruktor domyslny
		CURLFTPStatus();
		//! Destruktor
		~CURLFTPStatus();
		//! \return Status
		const threadingUtils::IOperation::Status status() const;
		//! \param status Status
		void setStatus(const threadingUtils::IOperation::Status status);
		//! \param error Opis b³êdu
		void setError(const std::string & error);
		//! \return Opis b³êdu
		const std::string & error() const;

	private:
		//! Status
		volatile threadingUtils::IOperation::Status status_;
		//! Opis b³êdu
		std::string error_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPSTATUS_H__