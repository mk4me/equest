/********************************************************************
	created:  2012/04/26
	created:  26:4:2012   23:56
	filename: ArgumentTracker.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_DATAACCESSOR__ARGUMENTTRACKER_H__
#define HEADER_GUARD_DATAACCESSOR__ARGUMENTTRACKER_H__

#include <utils/Utils.h>
#include <utils/SmartPtr.h>
#include <atomic>

namespace dataaccessor {
	//! Interfejs do odczytu czasu timera
	template<typename ArgumentType>
	class IArgumentTrackerReader
	{
	public:
		using ArgumentTrackerReaderType = IArgumentTrackerReader<ArgumentType>;
		using ArgumentTrackerReaderPtr = utils::shared_ptr<ArgumentTrackerReaderType>;
		using ArgumentTrackerReaderConstPtr = utils::shared_ptr<const ArgumentTrackerReaderType>;

	public:
		//! Destruktor wirtualny
		virtual ~IArgumentTrackerReader() {}
		//! \return Aktualny czas timera
		virtual ArgumentType currentArgument() const = 0;
	};

	//! Interfejs timera dla kanału - ustawia czas i czyta czas
	template<typename ArgumentType>
	class IArgumentTracker : public IArgumentTrackerReader<ArgumentType>
	{
	public:
		using ArgumentTrackerType = IArgumentTracker<ArgumentType>;
		using ArgumentTrackerPtr = utils::shared_ptr<ArgumentTrackerType>;
		using ArgumentTrackerConstPtr = utils::shared_ptr<const ArgumentTrackerType>;

	public:
		//! \param time Aktualny czas timera
		virtual void setCurrentArgument(const ArgumentType argument) = 0;
	};

	//! \tparam ArgumentType Typ śledzonego argumentu
	template<typename ArgumentType>
	//! Implementacja trackera
	class ArgumentTracker : public IArgumentTracker<ArgumentType>
	{
	public:
		//! Konstruktor domyślny
		ArgumentTracker() : currentArgument_(ArgumentType()) {}

		//! Konstruktor
		//! \param tracker Tracker z którego kopiujemy aktualną wartość
		template<class U,
			ENABLE_IF(std::is_convertible<U, ArgumentType>::value)>
		ArgumentTracker(const IArgumentTrackerReader<U> & tracker) : currentArgument_(tracker.currentArgument()) {}

		//! Konstruktor
		//! \tparam U Typ innego argumentu który kopiujemy
		template<class U,
			ENABLE_IF(std::is_convertible<U, ArgumentType>::value)>
		//! \param argument Aktualna wartośc argumentu
		ArgumentTracker(const U & argument) : currentArgument_(argument) {}

		//! Destruktor wirtualny
		virtual ~ArgumentTracker() {}

		//! \param argument Aktualna wartośc argumentu
		virtual void setCurrentArgument(const ArgumentType & argument) override
		{
			currentArgument_ = argument;
		}

		//! \return Aktualna wartośc argumentu
		virtual ArgumentType currentArgument() const override
		{
			return currentArgument_;
		}

	private:
		//! Aktualna wartośc argumentu
		std::atomic<ArgumentType> currentArgument_;
	};
}

#endif	//	HEADER_GUARD_DATAACCESSOR__ARGUMENTTRACKER_H__
