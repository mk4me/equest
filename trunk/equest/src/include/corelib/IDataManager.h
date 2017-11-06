/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   20:08
    filename: IDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAMANAGER_H__
#define HEADER_GUARD___IDATAMANAGER_H__

#include <corelib/ITransaction.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/BaseDataTypes.h>

namespace core {
	//! Interfejs do danych domenowych i zarz¹dzania nimi w aplikacji
	class IDataManager : public IDataManagerReader
	{
	public:

		class IOperations : public IDataManagerReader::IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}

			//! \data Dane wchodz¹ce pod kontrolê DM
			//! rzuca wyj¹tkiem jeœli nie uda siê dodaæ danych
			virtual void addData(const VariantPtr & data) = 0;
			//! Dane usuwane z DM
			//! rzuca wyj¹tkiem jeœli nie uda siê usun¹æ danych
			virtual void removeData(const VariantConstPtr & data) = 0;

			//! \param data Dane do dodaniwa w DM
			//! \return Czy uda³o siê poprawnie dodaæ dane do DM
			virtual const bool tryAddData(const VariantPtr & data) = 0;
			//! \param data Dane do dodaniwa w DM
			//! \return Czy uda³o siê poprawnie usun¹æ dane do DM
			virtual const bool tryRemoveData(const VariantConstPtr & data) = 0;

			//! \param data Aktualizowane dane
			//! \param newData Nowa wartoœæ danych
			virtual void updateData(const VariantConstPtr & data, const VariantConstPtr & newData) = 0;

			virtual const bool tryUpdateData(const VariantConstPtr & data, const VariantConstPtr & newData) = 0;
		};

		//TODO - aldrei alexandrescu i generatory liniowe klas
		class ITransaction : public core::ITransaction, public IOperations
		{
		public:
			virtual ~ITransaction() {}
		};

		//! Typ transakcji na danych domenowych - dzia³a w oparciu o RAII -> próbuje "commitowaæ" zmiany przy koñcu trwania ¿ycia obiektu transakcji
		typedef utils::shared_ptr<ITransaction> TransactionPtr;

	public:

		virtual ~IDataManager() {}

		//! \return Nowa transakcja danych domenowych
		virtual TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___IDATAMANAGER_H__
