/********************************************************************
	created:  2014/05/30	15:27:20
	filename: HMDBService.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSERVICE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSERVICE_H__

#include <plugins/hmdbCommunication/IHMDBService.h>
#include <corelib/IService.h>
#include <corelib/IThreadPool.h>
#include "CURLFTPManager.h"
#include <networkUtils/CURLManager.h>
#define NOMINMAX
#include <xmlpull/XmlUtils.h>

namespace hmdbCommunication
{
	class HMDBService : public plugin::IService, public IHMDBService
	{

		UNIQUE_ID("{E8801231-BACA-42C6-9A8E-706F561A563F}")
		CLASS_DESCRIPTION("HMDB Service", "HMDB Service")

	public:

		static void curlEnableLog(CURL * curl);
		static void curlDisableLog(CURL * curl);		

		static const networkUtils::IWSDLServicePtr createSecureWSDL(
			const utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		static const networkUtils::IWSDLServicePtr createUnsecureWSDL(
			const utils::shared_ptr<XmlUtils::CURLExecutor> executor, const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		//! Konstruktor domyœlny
		HMDBService();
		//! Destruktor wirtualny
		virtual ~HMDBService();

		//! \param motionServicesUrl Adres us³ug danych ruchu
		//! \param medicalServicesUrl Adres us³ug danych medycznych
		//! \param user Nazwa uzytkownika
		//! \param password Has³o uzytkownika
		//! \param motionDataUrl Adres danych ruchu
		//! \param motionDataUrl Adres danych medycznych
		//! \return Sesja serwisów danych ruchu, nullptr je¿eli nie uda³o siê uwierzytelniæ
		//! u¿ytkownika w us³ugach danych ruchu i medycznych
		virtual const IHMDBSessionPtr createSession(const std::string & motionServicesUrl,
			const std::string & medicalServicesUrl,
			const std::string & user,
			const std::string & password,
			const std::string & motionDataUrl,
			const std::string & medicalDataUrl,
			const utils::Filesystem::Path & CAPath,
			const networkUtils::SSLHostVerification hostVerification);

		//! \param motion Czy sesja tylko dla motion? Jeœli false to tylko medical
		//! \param servicesUrl Adres us³ug danych ruchu		
		//! \param user Nazwa uzytkownika
		//! \param password Has³o uzytkownika
		//! \param dataUrl Adres danych ruchu		
		//! \return Sesja serwisów danych ruchu, nullptr je¿eli nie uda³o siê uwierzytelniæ
		//! u¿ytkownika w us³ugach danych ruchu i medycznych
		virtual const IHMDBSessionPtr createSession(const bool motion,
			const std::string & servicesUrl,
			const std::string & user,
			const std::string & password,
			const std::string & dataUrl,
			const utils::Filesystem::Path & CAPath,
			const networkUtils::SSLHostVerification hostVerification);

		virtual const networkUtils::IWSDLServicePtr createHMDBService(
			const std::string & url,
			const std::string & user,
			const std::string & password,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		virtual const networkUtils::IWSDLServicePtr createHMDBSystemService(
			const std::string & url,
			const utils::Filesystem::Path & CAPath = utils::Filesystem::Path(),
			const networkUtils::SSLHostVerification hostVerification = networkUtils::HVNone,
			const utils::Filesystem::Path & schemaPath = utils::Filesystem::Path());

		//! \param url Adres serwera do pingowania
		//! \param caPath Œcie¿ka ewentualnego certyfikatu SSL je¿eli wymagany
		//! \return Czy serwer odpowiedzia³
		virtual const bool serverOnline(const std::string & url,
			const unsigned int timeout = 250,
			const utils::Filesystem::Path & caPath = utils::Filesystem::Path()) const;

		//! \param session Sesja us³ug bazy danych ruchu któr¹ chcemy obs³ugiwaæ
		virtual void attach(IHMDBSessionPtr session);

		//! \param session Sesja us³ug bazy danych ruchu któr¹ chcemy obs³ugiwaæ
		virtual void detach(IHMDBSessionPtr session);

		//-----------------------------------------------------------------

		//! Inicjalizacja us³ugi. Nastêpuje ju¿ po wczytaniu pluginów i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich us³ug.
		virtual void init(core::ISourceManager * sourceManager,
			core::IVisualizerManager * visualizerManager,
			core::IDataManager * memoryDataManager,
			core::IStreamDataManager * streamDataManager,
			core::IFileDataManager * fileDataManager,
			core::IDataHierarchyManager * hierarchyManager);

		//! PóŸna inicjalizacja us³ug, nastêpuje po wczytaniu i inicjalizacji wszystkich us³ug
		//! \return Czy us³uga poprawnie zainicjalizowana czy nie - mo¿e brakuje innych us³ug zale¿nych i sama nie ma jak dzia³aæ
		//! Dla false us³uga zostanie wy³adowana
		virtual const bool lateInit();

		//! Metoda powinna w bezpieczny sposób zwalniac zasoby, maj¹c na uwadze ¿e niekoniecznie wszystkie us³ugi i zasoby pobrane z zewn¹trz s¹ jeszcze dostêpne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, które uruchomi³ serwis, mo¿e tez zwalniac pamieæ przydzielon¹ dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezale¿niaj¹cy dana us³ugê od pozosta³ych us³ug i przygotowuj¹cy ja na usuniêcie
		virtual void finalize();

		//! Metoda aktualizuj¹ca pochodzi z w¹tku UI! Powinny tu byæ realizowane lekkie operacje odœwie¿ania widgetów!!
		//! Jako parametr dostajemy przyrost czasu jaki mina³ od poprzedniego wywo³ania
		virtual void update(double deltaTime);

		//! Us³uga nie musi mieæ wizualnej reprezentacji.
		//! \return Widget tworzony przez us³ugê b¹dŸ NULL.
		virtual QWidget* getWidget();

		//! Metoda dostarcza pomocniczych widgetów serwisu
		//! \return Lista pomocniczych widgetów serwisu
		virtual QWidgetList getPropertiesWidgets();

	private:
		//! Widget
		QWidget * mainWidget;
		//! W¹tek us³ug
		core::Thread servicesThread;
		//! W¹tek danych
		core::Thread dataThread;
		//! Manager us³ug
		networkUtils::CURLManagerPtr servicesManager;
		//! Manager danych
		utils::shared_ptr<CURLFTPManager> dataManager;

		utils::shared_ptr<XmlUtils::CURLExecutor> serviceCurlExecutor;

		//! Obiekt synchronizuj¹cy
		std::recursive_mutex sync_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSERVICE_H__
