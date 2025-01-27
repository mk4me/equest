/********************************************************************
	created:  2015/08/17	11:13:52
	filename: CurrentValueExtractor.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__CURRENTVALUEEXTRACTOR_H__
#define __HEADER_GUARD_DATAACCESSOR__CURRENTVALUEEXTRACTOR_H__

#include <dataaccessorlib/ArgumentTracker.h>

namespace dataaccessor
{
	//! \tparam ValueType Typ wyci�ganej warto�ci z kana�u
	template<typename ValueType>
	//! Interfejs umo�liwiaj�cy dost�p do aktualnej warto�ci kana�u w oparciu o tracker
	class ICurrentValueExtractor
	{
	public:
		//! Destruktor wirtualny
		virtual ~ICurrentValueExtractor() {}
		//! \return Atkualna warto�� kana�u
		virtual ValueType currentValue() const = 0;
	};

	//! \tparam ValueType Typ wyci�ganej warto�ci z kana�u
	//! \tparam ArgumentType Typ argumentu kana�u dla kt�rego wyci�gamy warto�ci
	template<class ValueType, class ArgumentType>
	//! Obiekt wyci�gaj�cy aktualn� warto�c kana�u dla czasu wyznaczanego przez timer i obiektu wyci�gaj�cego warto�ci kana�u dla zadanego czasu
	class CurrentValueExtractor : public ICurrentValueExtractor<ValueType>
	{
	public:
		//! Typ obiektu z kt�rego czytamy aktualn� warto�� kana�u
		using ArgumentTrackerReaderPtr = typename IArgumentTrackerReader<ValueType>::ArgumentTrackerReaderPtr;

	public:

		//! \param tracker Obiekt z aktualnym argumentem
		//! \param accessor Akcesor z kt�rego wyci�gamy aktualna warto��
		CurrentValueExtractor(ArgumentTrackerReaderPtr tracker,
			const IFunctionAccessorT<ValueType, ArgumentType> & accessor)
			: tracker(tracker), accessor(accessor)
		{
			if (tracker == nullptr){
				throw std::invalid_argument("Wrong tracker for CurrentValueExtractor");
			}
		}

		//! Destruktor wirtualny
		virtual ~CurrentValueExtractor() {}

		//! \return Aktualna warto�� kana�u
		virtual ValueType currentValue() const override
		{
			return accessor.value(tracker->currentArgument());
		}

	private:
		//! Obiekt z aktualnym argumentem
		ArgumentTrackerReaderPtr tracker;
		//! Kana� danych
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__CURRENTVALUEEXTRACTOR_H__