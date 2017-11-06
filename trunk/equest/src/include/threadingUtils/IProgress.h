/********************************************************************
	created:  2013/06/18
	created:  18:6:2013   10:36
	filename: IProgress.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__IPROGRESS_H__
#define HEADER_GUARD_THREADINGUTILS__IPROGRESS_H__

namespace threadingUtils {
	//! Interfejs pozwalający określić postęp pracy
	//! Postęp podawany jest od 0.0 do 1.0;
	class INormalizedProgress
	{
	public:
		//! Destruktor wirtualny
		virtual ~INormalizedProgress() {}
		//! \return Postęp realizacji przetwarzania
		virtual float normalizedProgress() const = 0;
	};

	//! Interfejs pozwalający określić procentowy postęp pracy
	//! Postęp podawany jest w procentach od 0% do 100%;
	class IPercentageProgress
	{
	public:
		//! Destruktor wirtualny
		virtual ~IPercentageProgress() {}
		//! \return Postęp realizacji przetwarzania od 0% do 100%
		virtual float percentageProgress() const = 0;
	};

	//! Interfejs pozwalający określić szacowany czas do końca zadania oraz czas jaki upłynął od momentu jego wystartowania
	//! Czas podawany jest w sekundach
	class ITimeProgress
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITimeProgress() {}
		//! \return Czas [s] jaki upłynął od rozpoczęcia zadania
		virtual float elapsed() const = 0;
		//! \return Szacowany czas [s] jaki został do zakończenia zadania
		virtual float remaining() const = 0;
	};

	//! Interfejs nadzorujący zadanie z postępem - pozwala ustawiać postęp oraz odczytywać czy zadanie ma zostac przerwane
	class IProgressSupervisor
	{
	public:
		//! Destruktor wirtualny
		virtual ~IProgressSupervisor() {}
		//! \return Czy należy przerwać operację
		virtual bool aborted() const = 0;
		//! \param progress Postęp od 0.0 do 1.0
		virtual void setProgress(const float progress) = 0;
	};
}

#endif	//	HEADER_GUARD_THREADINGUTILS__IPROGRESS_H__
