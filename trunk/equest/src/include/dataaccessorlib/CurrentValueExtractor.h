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
	//! \tparam ValueType Typ wyci¹ganej wartoœci z kana³u
	template<typename ValueType>
	//! Interfejs umo¿liwiaj¹cy dostêp do aktualnej wartoœci kana³u w oparciu o tracker
	class ICurrentValueExtractor
	{
	public:
		//! Destruktor wirtualny
		virtual ~ICurrentValueExtractor() {}
		//! \return Atkualna wartoœæ kana³u
		virtual ValueType currentValue() const = 0;
	};

	//! \tparam ValueType Typ wyci¹ganej wartoœci z kana³u
	//! \tparam ArgumentType Typ argumentu kana³u dla którego wyci¹gamy wartoœci
	template<class ValueType, class ArgumentType>
	//! Obiekt wyci¹gaj¹cy aktualn¹ wartoœc kana³u dla czasu wyznaczanego przez timer i obiektu wyci¹gaj¹cego wartoœci kana³u dla zadanego czasu
	class CurrentValueExtractor : public ICurrentValueExtractor<ValueType>
	{
	public:
		//! Typ obiektu z którego czytamy aktualn¹ wartoœæ kana³u
		using ArgumentTrackerReaderPtr = typename IArgumentTrackerReader<ValueType>::ArgumentTrackerReaderPtr;

	public:

		//! \param tracker Obiekt z aktualnym argumentem
		//! \param accessor Akcesor z którego wyci¹gamy aktualna wartoœæ
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

		//! \return Aktualna wartoœæ kana³u
		virtual ValueType currentValue() const override
		{
			return accessor.value(tracker->currentArgument());
		}

	private:
		//! Obiekt z aktualnym argumentem
		ArgumentTrackerReaderPtr tracker;
		//! Kana³ danych
		const IFunctionAccessorT<ValueType, ArgumentType> & accessor;
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__CURRENTVALUEEXTRACTOR_H__