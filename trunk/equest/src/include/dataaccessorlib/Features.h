/********************************************************************
	created:  2016/01/14
	filename: Features.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__FEATURES_H__
#define __HEADER_GUARD_DATAACCESSOR__FEATURES_H__

#include <type_traits>
#include <utils/TypeInfo.h>
#include <utils/SmartPtr.h>
#include <dataaccessorlib/AccessorType.h>

namespace dataaccessor
{
	//! Interfejs cech
	class IFeature
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFeature() {}
		//! \return Typ cechy danych kana³u
		virtual int id() const = 0;
		//! \return Typ obs³ugiwanego kana³u
		virtual AccessorType type() const { return AccessorType(); };
	};

	//! \tparam ID Identyfikator cechy 
	//! \tparam Feature Typ oakowywanej cechy
	template<int TypeT, typename Feature = IFeature>
	//! Klasa pomagaj¹ca w implementacji cech kana³ów
	class FeatureHelperT : public Feature
	{
		//! Weryfikacja interfejsu, któy musi dziedziczyæ po IFuture
		static_assert(std::is_same<IFeature, Feature>::value || std::is_base_of<IFeature, Feature>::value, "Feature must be derived from IFeature");

	public:
		//! Id cechy
		static const int ID = TypeT;

	public:
		//! Destruktor wirtualny
		virtual ~FeatureHelperT() {}
		//! \return Typ cechy
		virtual int id() const override final { return ID; }
	};

	//! \tparam ArgumentType Typ argumentu
	template<typename ArgumentType>
	//! Klasa opisuj¹ca cechy argumentów kana³u
	class IArgumentFeatureT : public virtual IFeature
	{
	public:
		//! Destruktor wirtualny
		virtual ~IArgumentFeatureT() {}
		//! \return Typ obs³ugiwanego kana³u
		virtual AccessorType type() const override { return AccessorType(typeid(void), typeid(ArgumentType)); };
	};

	//! \tparam ValueType Typ wartoœci
	template<typename ValueType>
	//! Klasa opisuj¹ca cechy wartoœci kana³u
	class IValueFeatureT : public virtual IFeature
	{
	public:
		//! Destruktor wirtualny
		virtual ~IValueFeatureT() {}
		//! \return Typ obs³ugiwanego kana³u
		virtual AccessorType type() const override { return AccessorType(typeid(ValueType), typeid(void)); };
	};

	//! \tparam ValueType Typ wartoœci
	//! \tparam ArgumentType Typ argumentu
	template<typename ValueType, typename ArgumentType>
	//! Klasa opisuj¹ca cechy argumentó i wartoœci kana³u
	class IAccessorFeatureT : public virtual IFeature, public IValueFeatureT<ValueType>, public IArgumentFeatureT<ArgumentType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IAccessorFeatureT() {}
		//! \return Typ obs³ugiwanego kana³u
		virtual AccessorType type() const override final { return AccessorType(typeid(ValueType), typeid(ArgumentType)); };
	};

	DEFINE_SMART_POINTERS(IFeature);
}

#endif  // __HEADER_GUARD_DATAACCESSOR__FEATURES_H__