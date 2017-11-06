/********************************************************************
    created:  2013/01/16
    created:  16:1:2013   12:00
    filename: ITransaction.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ITRANSACTION_H__
#define HEADER_GUARD___ITRANSACTION_H__

namespace core {

	//! Interfejs transakcji dla managerów. Powinien dzia³aæ w oparciu o RAII - jak ScopedLock,
	//! czyli przy tworzeniu próbujemy blokowaæ zasoby potrzebny do zapewnienia izolacji transakcji,
	//! przy niszczeniu commitujemy, lub w dowolnym momencie rollbackujemy. Rollbackowanie powinno zwalniaæ zasoby
	//! u¿yte do zapewnienia izolacji transakcji.
	class ITransaction
	{
	public:

		virtual ~ITransaction() {}

		//! Manualne commitowanie aktualnych zmian, nie podlegaj¹ one poŸniejszym rollbackom
		//virtual void commit() = 0;

		//! Cofa zmiany w ramach transakcji
		virtual void rollback() = 0;
		//! \return Czy transakcja zosta³a wycofana np. w wyniku b³êdu jednej z operacji transakcji lub
		//! rêcznie przez u¿ytkownika w wyniku zewnêtrznych czynników
		virtual const bool isRolledback() const = 0;
	};

}

#endif	//	HEADER_GUARD___ITRANSACTION_H__
