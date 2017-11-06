/********************************************************************
	created:  2014/05/21	13:24:59
	filename: IHMDBSourceContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCECONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCECONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>

namespace hmdbCommunication
{
	//! Interfejs opisuj¹cy kontekst Ÿród³a danych
	class IHMDBSourceContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBSourceContext() {}
		
		//! Kontekst lokalny
		virtual const IHMDBLocalContextPtr localContext() = 0;
		virtual const IHMDBLocalContextConstPtr localContext() const = 0;

		//! Kontekst zdalny
		virtual const IHMDBRemoteContextPtr remoteContext() = 0;
		virtual const IHMDBRemoteContextConstPtr remoteContext() const = 0;

		//! Kontekst danych
		virtual const IHMDBDataContextPtr dataContext() = 0;
		virtual const IHMDBDataContextConstPtr dataContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBSourceContext);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCECONTEXT_H__