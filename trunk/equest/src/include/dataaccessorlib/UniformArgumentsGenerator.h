/********************************************************************
	created:  2016/07/18
	filename: UniformArgumentsGenerator.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSGENERATOR_H__
#define __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSGENERATOR_H__

#include <utils/Utils.h>
#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/UniformArgumentsFeature.h>

namespace dataaccessor
{
	//! \tparam ArgumentType Typ argumentu
	template<typename ArgumentType>
	//! Klasa realizuj�ca generator argument�w
	class UniformArgumentsGenerator
	{
	public:

		template<typename AT = ArgumentType>
		//! \param step Krok z jakim generujemy argumenty
		//! \param start Warto�c argumentu od kt�rego zaczynamy
		//! \param end Warto�c argumentu na kt�rej ko�czymy!!
		UniformArgumentsGenerator(AT && step,
			const std::size_t size,
			AT && start = AT(0))
			: start_(std::forward<AT>(start)), step_(std::forward<AT>(step)),
			size_(size)
		{

		}

		//! \param Other Kopiowany generator
		UniformArgumentsGenerator(const UniformArgumentsGenerator & Other)
			: start_(Other.start_), size_(Other.size_),
			step_(Other.step_), feature(Other.feature)
		{

		}

		//! \tparam T
		template<typename T, ENABLE_IF(std::is_convertible<T, ArgumentType>::value)>
		//! \param Other Kopiowany generator
		UniformArgumentsGenerator(const UniformArgumentsGenerator<T> & Other)
			: start_(Other.start_), size_(Other.size_),
			step_(Other.step_)
		{
			if (Other.feature != nullptr) {
				feature = utils::make_shared<UniformArgumentsFeature<ArgumentType>>(Other.feature->argumentsInterval());
			}
		}

		//! \param Przenoszony generator
		UniformArgumentsGenerator(UniformArgumentsGenerator && Other)
			: start_(Other.start_), size_(Other.size_),
			step_(Other.step_), feature(std::move(Other.feature))
		{

		}

		//! Destruktor
		~UniformArgumentsGenerator() {}

		//! \param idx Index dla kt�rego generujemy argument
		inline ArgumentType argument(const std::size_t idx) const
		{
			if (idx >= size_) {
				throw std::range_error("Generator range exceeded");
			}

			return start_ + idx * step_;
		}

		//! \return Ilo�c pr�bek do wygenerowania
		inline ArgumentType start() const
		{
			return start_;
		}

		//! \return Ilo�c pr�bek do wygenerowania
		inline ArgumentType end() const
		{
			return start_ + size_ * step_;
		}

		//! \return Ilo�c pr�bek do wygenerowania
		inline std::size_t size() const
		{
			return size_;
		}

		//! \return Krok argument�w
		inline ArgumentType step() const
		{
			return step_;
		}

		//! \param ValueType Typ warto�ci kana�u
		template<typename ValueType>
		//! \param accessor Akcesor kt�ry zasilamy t� cech�
		inline void init(IAccessorT<ValueType, ArgumentType> * accessor)
		{
			if (accessor != nullptr) {
				if (feature == nullptr) {
					feature = utils::make_shared<UniformArgumentsFeature<ArgumentType>>(step_);
				}

				accessor->attachFeature(utils::make_shared<UniformArgumentsFeature<ArgumentType>>(step_));
			}
		}

	private:
		//! Granice do inicjalizacji
		utils::shared_ptr<UniformArgumentsFeature<ArgumentType>> feature;
		//! Warto�� pocz�tkowa argumentu
		const ArgumentType start_;
		//! Krok argument�w
		const ArgumentType step_;
		//! Ilo�� pr�bek
		const std::size_t size_;
	};
}

#endif  // __HEADER_GUARD_DATAACCESSOR__UNIFORMARGUMENTSGENERATOR_H__