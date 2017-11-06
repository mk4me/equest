/********************************************************************
	created:    2010/04/07
	created:    7:4:2010     18:08
	filename: PolciesBoost.h
	author:	    Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef __AVINOUT_POLCIESBOOST_H__
#define __AVINOUT_POLCIESBOOST_H__

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <utils/Utils.h>
#include <utils/PtrPolicyHelper.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
	////////////////////////////////////////////////////////////////////////////////

	/**
	*	Domyślna implementacja dla wskaźników boost::shared_ptr.
	*/
	struct PtrPolicyBoost
	{
		//! Typ wskaźnika.
		template <typename T> struct Ptr
		{
			using Type = boost::shared_ptr<T>;
		};
		//! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
		template <typename T> struct Pointed;
		//! Specjalizacja wyciągająca wskaźnik.
		template <typename T> struct Pointed< boost::shared_ptr<T> >
		{
			using Type = typename boost::shared_ptr<T>::element_type;
		};

		//! Klasa bazowa.
		using Base = NullType;

		//! Zerowanie wskaźnika.
		//! \param ptr
		template <typename T>
		static void initPtr(T & ptr)
		{}

		//! Ustawienie wartości wskaźnika surowymi danymi.
		//! \param ptr
		//! \param data
		template<typename T, typename Y>
		static void setPtr(boost::shared_ptr<T> & ptr, Y * data)
		{
			ptr.reset(data);
		}

		//! Ustawienie wartości wskaźnika poprzes kopiowanie z innego mądrego wskaźnika.
		//! \param ptr
		//! \param data
		template<typename T, typename Y>
		static void setPtr(boost::shared_ptr<T> & ptr, const boost::shared_ptr<Y> & data)
		{
			ptr = data;
		}

		template<typename T>
		static void swapPtr(boost::shared_ptr<T> & ptr, boost::shared_ptr<T> & data)
		{
			std::swap(ptr, data);
		}

		template<typename T, typename Y>
		static boost::shared_ptr<Y> dynamicCastPtr(const boost::shared_ptr<T> & ptr)
		{
			return boost::dynamic_pointer_cast<Y>(ptr);
		}

		template<typename T, typename Y>
		static boost::shared_ptr<Y> constCastPtr(const boost::shared_ptr<T> & ptr)
		{
			return boost::const_pointer_cast<Y>(ptr);
		}

		//! Zwraca surowy wskaźnik
		//! \param ptr
		template<typename T>
		static void* getRawPtr(const boost::shared_ptr<T> & ptr)
		{
			return ptr.get();
		}

		//! Zwraca surowy wskaźnik
		//! \param ptr
		template<typename T>
		static const void* getConstRawPtr(const boost::shared_ptr<T> & ptr)
		{
			return ptr.get();
		}

		//! Zwraca surowy wskaźnik
		//! \param ptr
		template<typename T>
		static T* getPtr(const boost::shared_ptr<T> & ptr)
		{
			return ptr.get();
		}

		//! Zwraca surowy wskaźnik
		//! \param ptr
		template<typename T>
		static const T* getConstPtr(const boost::shared_ptr<T> & ptr)
		{
			return ptr.get();
		}

		//! Czy wskaźnik jest unikatowy?
		//! \param ptr
		template<typename T>
		static bool isUnique(const boost::shared_ptr<T> & ptr)
		{
			return ptr.unique();
		}

		//! \return Ilość referencji do tego wskaźnika
		template<typename T>
		static auto referenceCount(const boost::shared_ptr<T> & ptr) -> decltype(ptr.use_count())
		{
			return ptr.use_count();
		}

		template<typename T>
		using enable_shared_from_this = boost::enable_shared_from_this < T > ;
	};

	template <> struct is_ptr_policy<PtrPolicyBoost> : public std::true_type{};

	////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // __AVINOUT_POLCIESBOOST_H__
