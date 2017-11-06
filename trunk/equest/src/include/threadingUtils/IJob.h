/********************************************************************
	created:  2014/10/28	13:12:42
	filename: IJob.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__IJOB_H__
#define __HEADER_GUARD_THREADINGUTILS__IJOB_H__

namespace threadingUtils
{
	//! Interfejs klasy realizuj¹cej zadanie
	class IJob
	{
	public:
		//! Typ wyliczeniowy opisuj¹cy stan zadania
		enum Status {
			Initialized,	//! Zadanie zainicjalizowane
			Pending,		//! Zadanie oczekuj¹ce na wykonanie (wystartowano, mo¿na czekaæ na zadaniu)
			Running,		//! Zadanie w trakcie wykonania
			Finished,		//! Zadanie zakoñczone poprawnie
			Cancelled,		//! Zadanie przerwane (anulowano)
			Failed			//! Zadanie zakoñczone niepowodzeniem
		};

	public:
		//! Destruktor wirtualny
		virtual ~IJob() {}
		//! Metoda startuj¹ca zadanie, mo¿e rzucaæ wyj¹tkami
		virtual void start() = 0;
		//! Metoda czekaj¹ca na zakoñczenie zadania
		virtual void wait() = 0;
		//! Metoda pozwalaj¹ca zadaniu wykonywaæ siê dalej w tle
		virtual void detach() = 0;
		//! Metoda anuluj¹ca zadanie
		virtual void cancel() = 0;
		//! \return Czy zadanie mo¿na jeszcze anulowaæ
		virtual bool cancellable() const = 0;
		//! \return Aktualny status zadania
		virtual Status status() const = 0;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__IJOB_H__