/********************************************************************
	created:  2014/06/02	13:28:35
	filename: HMDBSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCE_H__

#include <corelib/ISource.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <mutex>

class HMDBSourceWidget;

namespace hmdbCommunication
{
	class HMDBSource : public IHMDBSource, public plugin::ISource
	{

		UNIQUE_ID("{00001231-BACA-42C6-9A8E-706F561A563F}")
		CLASS_DESCRIPTION("HMDB Source", "HMDB Source")

	public:
		//! Konstruktor domyslny
		HMDBSource();
		//! Destruktor wirtualny
		virtual ~HMDBSource();

		//! Inicjalizacja ürÛd≥a. NastÍpuje juø po wczytaniu pluginÛw i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich ürÛde≥.
		virtual void init(core::IDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM,
			core::IDataHierarchyManager * hierarchyDM);

		//! Zobacz podobnπ metode dla IService
		//! PÛüna inicjalizacja ürÛd≥a, nastÍpuje po wczytaniu i inicjalizacji wszystkich innych ürÛde≥
		virtual bool lateInit();

		//! Metoda powinna w bezpieczny sposÛb zwalniac zasoby, majπc na uwadze øe niekoniecznie wszystkie us≥ugi i zasoby pobrane z zewnπtrz sπ jeszcze dostÍpne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, ktÛre uruchomi≥ serwis, moøe tez zwalniac pamieÊ przydzielonπ dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezaleøniajπcy dana us≥ugÍ od pozosta≥ych us≥ug i przygotowujπcy ja na usuniÍcie
		virtual void finalize();

		//! Metoda aktualizujπca pochodzi z wπtku UI! Powinny tu byÊ realizowane lekkie operacje odúwieøania widgetÛw!!
		//! Jako parametr dostajemy przyrost czasu jaki mina≥ od poprzedniego wywo≥ania
		virtual void update(double deltaTime);

		//! èrÛd≥o nie musi mieÊ wizualnej reprezentacji.
		//! \return Widget tworzony przez ürÛd≥o bπdü NULL.
		virtual QWidget* getWidget();
		//! \return Widget kontrolujπcy zachowanie us≥ugi/us≥ug zaleønych.
		virtual QWidget* getControlWidget();
		//! \return Widget dostarczajπcy opcji zwiπzanych z us≥ugπ/us≥ugami zaleønymi.
		virtual QWidget* getSettingsWidget();
		//! \param offeredTypes Typy oferowane przez to ürÛd≥o
		virtual void getOfferedTypes(utils::TypeInfoList & offeredTypes) const;

		//-----------------------------------------------------------------

		//! \param storage Miejsce sk≥πdowania danych
		//! \param user Nazwa uøytkownika
		//! \param password	Has≥o uøytkownika
		//! \param session Sesja us≥ug zdalnych
		//! \return Kontekst ürÛd≥a dancyh
		virtual const IHMDBSourceContextPtr createSourceContext(IHMDBStoragePtr storage,
			const std::string & user, const std::string & password,
			IHMDBSessionPtr session = IHMDBSessionPtr());

		//! \param sourceContext Kontekst ürÛd≥a danych
		//! \return Konteks p≥ytkiej kopii bazy danych
		virtual const IHMDBShallowCopyContextPtr createShallowCopyContext(IHMDBSourceContextPtr sourceContext);

		//! \param url Adres serwisu gdzie zak≥adamy konto
		//! \param login Nazwa uøytkownika dla ktÛrego chcemy za≥oøyc konto
		//! \param password Has≥o nowego konta
		//! \param name ImiÍ nowego uøytkownika
		//! \param surname Nazwisko nowego uøytkownika
		//! \param email Email nowego uøytkownika
		virtual void registerAccount(const std::string & url,
			const std::string & login, const std::string & password,
			const std::string & name, const std::string & surname,
			const std::string & email, const bool multi = true,
			const std::string & caPath = std::string());


		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param login Nazwa uøytkownika dla ktÛrego chcemy przeprowadziÊ aktywacjÍ
		//! \param activationCode Kod aktywacyjny
		virtual void activateAccount(const std::string & url,
			const std::string & login, const std::string & activationCode,
			const bool multi = true,
			const std::string & caPath = std::string());

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param email Adres email zwiπzany z kontem na ktÛry wys≥ane zostanπ dalsze instrukcje
		virtual void resetPassword(const std::string & url, const std::string & email,
			const bool multi = true,
			const std::string & caPath = std::string());

		//! \return Manager widowkow
		virtual IHMDBSourceViewManager * viewManager();
		//! \return Manager widowkow
		virtual const IHMDBSourceViewManager * viewManager() const;

		//! \param data Dane dla ktÛrych szukamy kontekstu ürÛd≥a
		//! \return Konteks ürÛd≥a dla danych
		virtual const IHMDBSourceContextPtr sourceContextForData(const core::VariantConstPtr data) const;
		//! \param data Dane dla ktÛrych szukamy kontekstu p≥ytkiej kopii
		//! \return Konteks p≥ytkiej kopii dla danych
		virtual const IHMDBShallowCopyContextPtr shallowContextForData(const core::VariantConstPtr data) const;
		//! \param data Dane dla ktÛrych szukamy kontekstu ürÛd≥a
		//! \return Konteks ürÛd≥a dla danych
		virtual const IHMDBSourceContextPtr sourceContextForData(const void * data) const;
		//! \param data Dane dla ktÛrych szukamy kontekstu p≥ytkiej kopii
		//! \return Konteks p≥ytkiej kopii dla danych
		virtual const IHMDBShallowCopyContextPtr shallowContextForData(const void * data) const;

		//! \param sourceContext Konteks ürÛd≥a dla danych		
		virtual void registerSourceContext(const IHMDBSourceContextPtr sourceContext);
		//! \param shallowContext Konteks p≥ytkiej kopii dla danych		
		virtual void registerShallowContext(const IHMDBShallowCopyContextPtr shallowContext);
		//! \return IloúÊ zarejestrowanych kontekstÛw p≥ytkiej kopii bazy danych
		virtual unsigned int shallowContextsCount() const;
		//! \param idx Indeks kontekstu
		//! \return Kontekst p≥ytkiej kopii lub nullptr jeøeli nie ma
		virtual const IHMDBShallowCopyContextPtr shallowContext(const unsigned int idx);

	private:
		//! Widget
		HMDBSourceWidget * mainWidget;
		//! Obiekt synchronizujπcy
		mutable std::recursive_mutex sync_;
		//! Lista kontekstÛw p≥ytkiej kopii
		mutable std::list<IHMDBSourceContextWeakPtr> sourceContexts_;
		//! Lista kontekstÛw p≥ytkiej kopii
		mutable std::list<IHMDBShallowCopyContextWeakPtr> shallowContexts_;

		//! Lista kontekstÛw danych
		//std::list<IHMDBSourceContext*> sourceContexts_;
		//! Manager pamiÍci
		core::IDataManager * memoryDM;
		//! Manager strumieni
		core::IStreamDataManager * streamDM;
		//! Manager danych
		core::IFileDataManager * fileDM;

		core::IDataHierarchyManager * hierarchyDM;
		//! Manager ustawieÒ widokÛw
		utils::shared_ptr<IHMDBSourceViewManager> viewManager_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCE_H__