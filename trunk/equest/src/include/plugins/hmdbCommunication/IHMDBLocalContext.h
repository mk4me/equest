/********************************************************************
	created:  2014/07/04	10:23:59
	filename: IHMDBLocalContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBLOCALCONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBLOCALCONTEXT_H__

#include <threadingUtils/ITTransaction.h>
#include <corelib/Variant.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>


namespace hmdbCommunication
{

	class IHMDBLocalContextOperations
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBLocalContextOperations() {}

		//! \return Czy dane o które pytamy pochodza z tego Ÿród³a
		virtual const bool isMyData(const core::VariantConstPtr data) const = 0;
		//! \return Czy dane o które pytamy pochodza z tego Ÿród³a
		virtual const bool isMyData(const void * data) const = 0;

		//! \param fileName Nazwa pliku ze storage
		//! \return Dane skojarzone z tym plikiem
		virtual const core::ConstVariantsList data(const std::string & fileName) const = 0;

		//! \param fileName Plik jaki bêdê ³adowaæ ze storage do StreamManagera
		//! \return Czy uda³o siê za³adowac plik
		virtual const bool load(const std::string & fileName) = 0;
		//! \param fileName Plik jaki bêdê wy³adowywaæ ze StreamManagera
		//! \return Czy uda³o siê wy³adowac plik
		virtual const bool unload(const std::string & fileName) = 0;
		//! \param fileName Plik o który pytam czy jest za³¹dowany
		//! \return Czy plik jest za³adowany
		virtual const bool isLoaded(const std::string & fileName) const = 0;

		//! \param data Dane jakie bêdê ³adowa³ do MemoryManagera
		//! \return Czy uda³o siê za³adowac dane
		virtual const bool load(const core::VariantPtr data) = 0;
		//! \param data Dane jakie bêdê wy³adowa³ z MemoryManagera
		//! \return Czy uda³o siê wy³adowac dane
		virtual const bool unload(const core::VariantConstPtr data) = 0;
	};

	class IHMDBLocalContext : public threadingUtils::ITTransaction<IHMDBLocalContextOperations>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBLocalContext() {}

		//! \return Kontekst danych
		virtual const IHMDBDataContextPtr dataContext() = 0;
		//! \return Kontekst danych
		virtual const IHMDBDataContextConstPtr dataContext() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBLocalContext);
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBLOCALCONTEXT_H__