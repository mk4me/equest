/********************************************************************
    created:  2012/04/26
    created:  26:4:2012   23:59
    filename: DescriptorFeature.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_DATAACCESSOR__DESCRIPTORFEATURE_H__
#define HEADER_GUARD_DATAACCESSOR__DESCRIPTORFEATURE_H__

#include <dataaccessorlib/Features.h>

namespace dataaccessor {

	//! Interfejs do czytania opisu kanału - osi x i y oraz nazwy kanału.
	class DescriptorFeature : public FeatureHelperT<Description>
	{
	private:
		//! Opis kanału
		const std::string name_;
		//! Jednostka czasu
		const std::string argumentUnit_;
		//! Jednostka czasu
		const std::string argumentType_;
		//! Jendostka wartości
		const std::string valueUnit_;
		//! Jendostka wartości
		const std::string valueType_;

	public:

		DescriptorFeature(const std::string & name,
			const std::string & valueType,
			const std::string & valueUnit,
			const std::string & argumentType,
			const std::string & argumentUnit)
			: name_(name), valueUnit_(valueUnit),
			argumentUnit_(argumentUnit), valueType_(valueType),
			argumentType_(argumentType)
		{

		}

		DescriptorFeature(const DescriptorFeature & Other)
			: name_(Other.name_), valueUnit_(Other.valueUnit_),
			argumentUnit_(Other.argumentUnit_),
			valueType_(Other.valueType_), argumentType_(Other.argumentType_)
		{

		}

		DescriptorFeature(DescriptorFeature && Other)
			: name_(std::move(Other.name_)),
			valueUnit_(std::move(Other.valueUnit_)),
			argumentUnit_(std::move(Other.argumentUnit_)),
			valueType_(std::move(Other.valueType_)),
			argumentType_(std::move(Other.argumentType_))
		{

		}

		//! Destruktor wirtualny
		~DescriptorFeature() {}

		//! \return
		std::string name() const { return name_; }

		//! \return
		std::string argumentUnit() const { return argumentUnit_; }

		//! \return
		std::string valueUnit() const { return valueUnit_; }

		//! \return
		std::string argumentType() const { return argumentType_; }

		//! \return
		std::string valueType() const { return valueType_; }

		//! \tparam ValueType Typ wartości
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param valueUnit Jendostka wartości
		//! \param argumentUnit Jednostka argumentu
		//! \return Obiekt deskryptora kanału danych
		static inline DescriptorFeature * create(const std::string & name,
			const std::string & valueUnit,
			const std::string & argumentUnit,
			ValueType * dummyValue = nullptr,
			ArgumentType * dummyArgument = nullptr) {
			return new DescriptorFeature(name, typeid(ValueType).name(), valueUnit,
				typeid(ArgumentType).name(), argumentUnit);
		}

		template<typename ValueType, typename ArgumentType>
		inline static DescriptorFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			return create<ValueType, ArgumentType>("", "", "");
		}
	};
}

#endif	//	HEADER_GUARD_DATAACCESSOR__DESCRIPTORFEATURE_H__
