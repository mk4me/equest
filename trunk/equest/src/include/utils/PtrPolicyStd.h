/********************************************************************
	created:  2014/09/29	10:35:43
	filename: PtrPolicyStd.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__PTRPOLICYSTD_H__
#define __HEADER_GUARD_UTILS__PTRPOLICYSTD_H__

#include <memory>
#include <utils/Utils.h>
#include <utils/PtrPolicyHelper.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
	////////////////////////////////////////////////////////////////////////////////

	/**
	*	Domyślna implementacja dla wskaźników utils::shared_ptr.
	*/
	struct PtrPolicyStd
	{
		//! Typ wskaźnika.
		template <typename T> struct Ptr
		{
			using Type = std::shared_ptr<T>;
		};
		//! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
		template <typename T> struct Pointed;
		//! Specjalizacja wyciągająca wskaźnik.
		template <typename T> struct Pointed< std::shared_ptr<T> >
		{
			using Type = typename std::shared_ptr<T>::element_type;
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
			static void setPtr(std::shared_ptr<T> & ptr, Y * data)
			{
				ptr.reset(data);
			}

			//! Ustawienie wartości wskaźnika poprzes kopiowanie z innego mądrego wskaźnika.
			//! \param ptr
			//! \param data
			template<typename T, typename Y>
			static void setPtr(std::shared_ptr<T> & ptr, const std::shared_ptr<Y> & data)
			{
				ptr = data;
			}

			template<typename T>
			static void swapPtr(std::shared_ptr<T> & ptr, std::shared_ptr<T> & data)
			{
				std::swap(ptr, data);
			}

			template<typename T, typename Y>
			static std::shared_ptr<Y> dynamicCastPtr(const std::shared_ptr<T> & ptr)
			{
				return std::dynamic_pointer_cast<Y>(ptr);
			}

			template<typename T, typename Y>
			static std::shared_ptr<Y> constCastPtr(const std::shared_ptr<T> & ptr)
			{
				return std::const_pointer_cast<Y>(ptr);
			}

			//! Zwraca surowy wskaźnik
			//! \param ptr
			template<typename T>
			static void* getRawPtr(const std::shared_ptr<T> & ptr)
			{
				return ptr.get();
			}

			//! Zwraca surowy wskaźnik
			//! \param ptr
			template<typename T>
			static const void* getConstRawPtr(const std::shared_ptr<T> & ptr)
			{
				return ptr.get();
			}

			//! Zwraca surowy wskaźnik
			//! \param ptr
			template<typename T>
			static T* getPtr(const std::shared_ptr<T> & ptr)
			{
				return ptr.get();
			}

			//! Zwraca surowy wskaźnik
			//! \param ptr
			template<typename T>
			static const T* getConstPtr(const std::shared_ptr<const T> & ptr)
			{
				return ptr.get();
			}

			//! Czy wskaźnik jest unikatowy?
			//! \param ptr
			template<typename T>
			static bool isUnique(const std::shared_ptr<T> & ptr)
			{
				return ptr.unique();
			}

			//! \return Ilość referencji do tego wskaźnika
			template<typename T>
			static auto referenceCount(const std::shared_ptr<T> & ptr) -> decltype(ptr.use_count())
			{
				return ptr.use_count();
			}

			template<typename T>
			using enable_shared_from_this = std::enable_shared_from_this < T > ;
	};

	template <> struct is_ptr_policy<PtrPolicyStd> : public std::true_type{};



	////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif	// __HEADER_GUARD_UTILS__PTRPOLICYSTD_H__