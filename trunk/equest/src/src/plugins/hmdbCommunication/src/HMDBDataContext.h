/********************************************************************
	created:  2014/07/11	17:36:28
	filename: HMDBDataContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBDATACONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBDATACONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBDataContext.h>

namespace hmdbCommunication
{
	class IHMDBStorage;

	class HMDBDataContext : public IHMDBDataContext
	{
	public:
		//! \param storage Storage do zapisu plików z danymi i p³ytkiej kopii bazy danych
		//! \param userName Skrót u¿ytkownika na potrzby obs³ugi p³ytkiej kopii
		//! \param userHash Nazwa u¿ytkownika
		HMDBDataContext(IHMDBStoragePtr storage, const std::string & userName,
			const std::string & userHash);
		//! Destruktor wirtualny
		virtual ~HMDBDataContext();
		//! \return Obiekt zarz¹dzaj¹cy œci¹gniêtymi danymi
		virtual const IHMDBStorageConstPtr storage() const;
		//! \return Obiekt zarz¹dzaj¹cy œci¹gniêtymi danymi
		virtual const IHMDBStoragePtr storage();
		//! \return User hash na potrzeby obs³ugi p³ytkiej kopii bazy danych w storage
		virtual const std::string userName() const;
		//! \return User hash na potrzeby obs³ugi p³ytkiej kopii bazy danych w storage
		virtual const std::string userHash() const;
		//! \return P³ytka kopia bazy danych
		virtual const ShallowCopyPtr createShallowCopy() const;
		//! \return Czy p³ytka kopia wystêpuje
		virtual const bool shallowCopyExists() const;

	private:
		//! Storage do zapisu plików z danymi i p³ytkiej kopii bazy danych
		IHMDBStoragePtr storage_;
		//! Skrót u¿ytkownika na potrzby obs³ugi p³ytkiej kopii
		const std::string userHash_;
		//! Nazwa u¿ytkownika
		const std::string userName_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBDATACONTEXT_H__