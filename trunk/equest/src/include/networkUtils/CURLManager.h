/********************************************************************
	created:  2014/05/15	10:31:26
	filename: CURLManager.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__
#define __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__

#include <networkUtils/Export.h>
#include <networkUtils/CURLUtils.h>
#include <string>
#include <future>

namespace networkUtils
{
	class NETWORKUTILS_EXPORT CURLManager
	{
	private:
		//! Forward declaration
		class CURLManagerImpl;
		friend class CURLManagerImpl;

	public:
		//! Domyœlny konstruktor
		CURLManager();
		//! \param multi Uchwyt dla obs³ugiwanych po³¹czeñ
		explicit CURLManager(CURLMPtr multi);
		//! Destruktor wirtualny
		virtual ~CURLManager();
		//! \param curl Skonfigurowany uchwyt do obs³u¿enia
		//! \param wait Obiekt pozwalaj¹cy zaczekaæ na zakoñæenie operacji curla
		std::future<CURLcode> addRequest(CURLPtr curl);
		//! \param curl Uchwyt do usuniêcia
		void removeRequest(CURLPtr curl);
		//! Metoda konczaca przetwarzanie
		void finalize();
		//! Metoda przetwarzaj¹ca zapytania do momentu przerwania finalize
		void run();

	private:
		//! Metoda wo³ana zaraz po zakoñczeniu pracy z danymi przez po³¹czenia
		virtual void onProcess() {}
		//! \param curl Uchwyt który w³aœnie dodano
		virtual void onAddRequest(CURLPtr curl) {}
		//! \param curl Uchwyt który w³aœnie usuniêto
		virtual void onRemoveRequest(CURLPtr curl) {}
		//! \param curl Uchwyt który zosta³ anulowany
		virtual void onCancelRequest(CURLPtr curl) {}
		//! \param curl Uchwyt który spowodowa³ b³¹d
		//! \param error Opis b³êdu
		virtual void onErrorRequest(CURLPtr curl, const std::string & error) {}
		//! \param curl Uchwyt który zakoñczy³ przetwarzanie danych poprawnie
		virtual void onFinishRequest(CURLPtr curl) {}

	private:
		//! Obiekt implementuj¹cy funkcjonalnoœc klasy
		utils::shared_ptr<CURLManagerImpl> impl;
	};

	DEFINE_SMART_POINTERS(CURLManager);
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLMANAGER_H__