/********************************************************************
	created:    2010/04/07
	created:    7:4:2010     18:06
	filename: PoliciesOSG.h
	author:	    Piotr Gwiazdowski
	
	purpose:    
*********************************************************************/
#ifndef __AVINOUT_POLICIESOSG_H__
#define __AVINOUT_POLICIESOSG_H__

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <utils/PtrPolicyHelper.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

/**
*	Domyślna implementacja dla wskaźników osg::ref_ptr.
*/
struct PtrPolicyOSG
{
    //! Typ wskaźnika.
    template <typename T> struct Ptr 
    {
        typedef osg::ref_ptr<T> Type;
    };
    //! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
	template <typename T> struct Pointed;
    //! Specjalizacja wyciągająca wskaźnik.
	template <typename T> struct Pointed<osg::ref_ptr<T> >
    {
        typedef T Type;
    };

    //! Klasa bazowa.
    typedef osg::Referenced Base;

    //! Inicjalizacja wartości.
    //! \param ptr
	template <typename T>
    static void initPtr( T & ptr )
    {}

    //! Implemetnacja resetowania wskaźnika dla osg::ref_ptr.
    //! \param ptr
    //! \param data
	template<typename T, typename Y>
    static void setPtr(osg::ref_ptr<T> & ptr, Y * data)
    {
        ptr = data;
    }

    //! Implemetnacja resetowania wskaźnika dla osg::ref_ptr.
    //! \param ptr
    //! \param data
	template<typename T, typename Y>
    static void setPtr(osg::ref_ptr<T> & ptr, const osg::ref_ptr<Y> & data)
    {
        ptr = data;
    }

	template<typename T>
	static void swapPtr(osg::ref_ptr<T> & ptr, osg::ref_ptr<T> & data)
	{
		std::swap(ptr, data);
	}

	template<typename T, typename Y>
	static const osg::ref_ptr<Y> dynamicCastPtr(const osg::ref_ptr<T> & ptr)
	{
		return osg::dynamic_pointer_cast<Y*>(ptr);
	}

	template<typename T, typename Y>
	static const osg::ref_ptr<Y> constCastPtr(const osg::ref_ptr<T> & ptr)
	{
		return osg::const_pointer_cast<Y*>(ptr);
	}

    //! Zwraca surowy wskaźnik
    //! \param ptr
	template<typename T>
    static void* getRawPtr(const osg::ref_ptr<T> & ptr)
    {
        return ptr.get();
    }

    //! Zwraca surowy wskaźnik
    //! \param ptr
	template<typename T>
    static const void* getConstRawPtr(const osg::ref_ptr<T> & ptr)
    {
        return ptr.get();
    }

	//! Zwraca surowy wskaźnik
	//! \param ptr
	template<typename T>
	static T* getPtr(const osg::ref_ptr<T> & ptr)
	{
		return ptr.get();
	}

	//! Zwraca surowy wskaźnik
	//! \param ptr
	template<typename T>
	static const T* getConstPtr(const osg::ref_ptr<T> & ptr)
	{
		return ptr.get();
	}

    //! Implementacja unikalności wskaźnika dla osg::ref_ptr.
    //! \param ptr
	template<typename T>
    static const bool isUnique(const osg::ref_ptr<T> & ptr)
    {
        return ptr->referenceCount() == 0;
    }

	//! \return Ilość referencji do tego wskaźnika
	template<typename T>
	static const long referenceCount(const osg::ref_ptr<T> & ptr)
	{
		return (ptr != nullptr) ? ptr->referenceCount() : 0;
	}

	template<typename T>
	struct enable_shared_from_this
	{
	public:
		enable_shared_from_this()
		{
		}

		enable_shared_from_this(enable_shared_from_this const &)
		{
		}

		enable_shared_from_this & operator=(enable_shared_from_this const &)
		{
			return *this;
		}

		~enable_shared_from_this()
		{
		}

	public:

		osg::ref_ptr<T> shared_from_this()
		{
			return osg::ref_ptr<T>(dynamic_cast<T*>(this));
		}

		osg::ref_ptr<const T> shared_from_this() const
		{
			return osg::ref_ptr<const T>(dynamic_cast<const T*>(this));
		}
	};
};

template <> struct is_ptr_policy<PtrPolicyOSG> : public std::true_type {};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // __AVINOUT_POLICIESOSG_H__
