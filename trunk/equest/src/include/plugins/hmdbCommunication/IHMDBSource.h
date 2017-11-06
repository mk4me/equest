/********************************************************************
	created:  2014/05/22	14:16:35
	filename: IHMDBSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCE_H__

#include <string>
#include <corelib/Variant.h>
#include <QtGui/QIcon>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBSourceContext.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>

class QWidget;

namespace hmdbCommunication
{
	class IShallowCopyFilterManager;
	class IHMDBSourceViewManager;


	class IHMDBSource
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBSource() {}

		//! \param storage Miejsce sk³¹dowania danych
		//! \param user Nazwa u¿ytkownika
		//! \param password	Has³o u¿ytkownika
		//! \param session Sesja us³ug zdalnych
		//! \return Kontekst Ÿród³a dancyh
		virtual const IHMDBSourceContextPtr createSourceContext(IHMDBStoragePtr storage,
			const std::string & user, const std::string & password,
			IHMDBSessionPtr session = nullptr) = 0;		

		//! \param sourceContext Kontekst Ÿród³a danych
		//! \return Konteks p³ytkiej kopii bazy danych
		virtual const IHMDBShallowCopyContextPtr createShallowCopyContext(IHMDBSourceContextPtr sourceContext) = 0;

		//! \param url Adres serwisu gdzie zak³adamy konto
		//! \param login Nazwa u¿ytkownika dla którego chcemy za³o¿yc konto
		//! \param password Has³o nowego konta
		//! \param name Imiê nowego u¿ytkownika
		//! \param surname Nazwisko nowego u¿ytkownika
		//! \param email Email nowego u¿ytkownika
		//! \param multi Czy rejestrowaæ w bazie ruchu i medycznej
		//! \param caPath Œcie¿ka certyfikatu do uwie¿ytelnienia us³ugi
		virtual void registerAccount(const std::string & url,
			const std::string & login, const std::string & password,
			const std::string & name, const std::string & surname,
			const std::string & email, const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param login Nazwa u¿ytkownika dla którego chcemy przeprowadziæ aktywacjê
		//! \param activationCode Kod aktywacyjny
		//! \param multi Czy rejestrowaæ w bazie ruchu i medycznej
		//! \param caPath Œcie¿ka certyfikatu do uwie¿ytelnienia us³ugi
		virtual void activateAccount(const std::string & url,
			const std::string & login, const std::string & activationCode,
			const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		//! \param url Adres serwisu gdzie aktywujemy konto
		//! \param email Adres email zwi¹zany z kontem na który wys³ane zostan¹ dalsze instrukcje
		//! \param multi Czy rejestrowaæ w bazie ruchu i medycznej
		//! \param caPath Œcie¿ka certyfikatu do uwie¿ytelnienia us³ugi
		virtual void resetPassword(const std::string & url, const std::string & email,
			const bool multi = true,
			const std::string & caPath = std::string()) = 0;

		//! \return Manager widowkow
		virtual IHMDBSourceViewManager * viewManager() = 0;
		//! \return Manager widowkow
		virtual const IHMDBSourceViewManager * viewManager() const = 0;

		//! \param data Dane dla których szukamy kontekstu Ÿród³a
		//! \return Konteks Ÿród³a dla danych
		virtual const IHMDBSourceContextPtr sourceContextForData(const core::VariantConstPtr data) const = 0;
		//! \param data Dane dla których szukamy kontekstu p³ytkiej kopii
		//! \return Konteks p³ytkiej kopii dla danych
		virtual const IHMDBShallowCopyContextPtr shallowContextForData(const core::VariantConstPtr data) const = 0;
		//! \param data Dane dla których szukamy kontekstu Ÿród³a
		//! \return Konteks Ÿród³a dla danych
		virtual const IHMDBSourceContextPtr sourceContextForData(const void * data) const = 0;
		//! \param data Dane dla których szukamy kontekstu p³ytkiej kopii
		//! \return Konteks p³ytkiej kopii dla danych
		virtual const IHMDBShallowCopyContextPtr shallowContextForData(const void * data) const = 0;

		//! \param sourceContext Konteks Ÿród³a dla danych
		virtual void registerSourceContext(const IHMDBSourceContextPtr sourceContext) = 0;
		//! \param shallowContext Konteks p³ytkiej kopii dla danych		
		virtual void registerShallowContext(const IHMDBShallowCopyContextPtr shallowContext) = 0;
		//! \return Iloœæ zarejestrowanych kontekstów p³ytkiej kopii bazy danych
		virtual unsigned int shallowContextsCount() const = 0;
		//! \param idx Indeks kontekstu
		//! \return Kontekst p³ytkiej kopii lub nullptr je¿eli nie ma
		virtual const IHMDBShallowCopyContextPtr shallowContext(const unsigned int idx) = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCE_H__