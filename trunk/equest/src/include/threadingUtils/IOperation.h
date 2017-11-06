/********************************************************************
	created:  2014/05/23	12:46:46
	filename: IOperation.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__IOPERATION_H__
#define __HEADER_GUARD_THREADINGUTILS__IOPERATION_H__

#include <string>

namespace threadingUtils
{
	//! Interfejs pozwalaj¹cy przerywaæ dzia³anie operacji
	class IAbortable
	{
	public:
		//! Destruktor wirtualny
		virtual ~IAbortable() {}
		//! Metoda anuluje aktualne zadanie
		virtual void abort() = 0;
	};

	//! Interfejs operacji
	class IOperation
	{
	public:
		//! Aktualny stan wykonania operacji
		enum Status {
			Initialized,//! Utworzono operacjê
			Pending,	//! Czeka na wykonanie
			Running,	//! W trakcie
			Aborted,	//! Przerwano
			Finished,	//! Zakoñczono powodzeniem
			Error,		//! Zakoñczono b³êdem
		};
	public:
		//! Destruktor wirtualny
		virtual ~IOperation() {}
		//! Metoda rozpoczyna operacjê
		virtual void start() = 0;
		//! Metoda czeka do zakoñczenia operacji
		virtual void wait() = 0;
		//! \return Stan operacji
		virtual Status status() const = 0;
		//! \return Opis b³êdu
		virtual std::string error() const = 0;
	};

	//! Interfejs operacji która mozna przerwaæ
	class IAbortableOperation : public IOperation,
		public IAbortable
	{
	public:
		//! Destruktor wirtualny
		virtual ~IAbortableOperation() {}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__IOPERATION_H__