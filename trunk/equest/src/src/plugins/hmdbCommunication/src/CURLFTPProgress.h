/********************************************************************
	created:  2014/05/28	20:36:21
	filename: CURLFTPProgress.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__

#include <networkUtils/CURLFTPHelper.h>

namespace hmdbCommunication
{
	class CURLFTPProgress : public networkUtils::ICURLFTPProgress
	{
	public:
		//! Domyslny konsturktor
		CURLFTPProgress();
		//! Destruktor wirtualny
		virtual ~CURLFTPProgress();
		//! Przerywa operacjê
		void abort();
		//! \return Czy operacja ma byæ przerwana
		virtual bool aborted() const;
		//! \param progress Postêp operacji
		virtual void setProgress(const float progress);
		//! \param processedData Ilosc przetworzonych danych [bytes]
		virtual void setProcessedData(const std::size_t processedData);
		//! \return Postêp operacji
		float progress() const;
		//! \return Ilosæ do tej pory przetworzonych danych
		std::size_t processedData() const;

	private:
		//! Czy operacja ma byæ przerwana
		volatile bool aborted_;
		//! Postêp operacji
		volatile float progress_;
		//! Ilosc przetworzonych danych
		volatile std::size_t processedData_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CURLFTPPROGRESS_H__