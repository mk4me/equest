/********************************************************************
	created:  2014/06/02	14:28:10
	filename: HMDBSourceContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBSourceContext.h>

namespace hmdbCommunication
{
	class HMDBSourceContext : public IHMDBSourceContext
	{
	public:
		//! Konstruktor
		//! \param dataContext Kontekst danych
		//! \param localContext Kontekst danych lokalnych
		//! \param remoteContext Kontekst zdalnych danych i operacji
		HMDBSourceContext(IHMDBDataContextPtr dataContext,
			IHMDBLocalContextPtr localContext,
			IHMDBRemoteContextPtr remoteContext);
		//! Destruktor wirtualny
		virtual ~HMDBSourceContext();

		//! Kontekst lokalny
		virtual const IHMDBLocalContextPtr localContext();
		virtual const IHMDBLocalContextConstPtr localContext() const;

		//! Kontekst zdalny
		virtual const IHMDBRemoteContextPtr remoteContext();
		virtual const IHMDBRemoteContextConstPtr remoteContext() const;

		//! Kontekst danych
		virtual const IHMDBDataContextPtr dataContext();
		virtual const IHMDBDataContextConstPtr dataContext() const;

	private:
		//! Aktualna p³ytka kopia bazy danych
		IHMDBDataContextPtr dataContext_;
		//! Sesja us³ug bazy danych
		IHMDBLocalContextPtr localContext_;
		//! Storage dla danych
		IHMDBRemoteContextPtr remoteContext_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXT_H__