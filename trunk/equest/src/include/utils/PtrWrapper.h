/********************************************************************
	created:    2010/07/30
	created:    30:7:2010     18:25
	filename: PtrWrapper.h
	author:	    Piotr Gwiazdowski
	
	purpose:    
*********************************************************************/
#ifndef HEADER_GUARD__UTILS_PTRWRAPPER_H__
#define HEADER_GUARD__UTILS_PTRWRAPPER_H__

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

template <class T, class PtrPolicy>
class Adapter : public PtrPolicy::Base
{
public:
    using value_type = T;
private:
    //! Wartość.
    T value;

public:
    //!
    Adapter()
    {}
    //!
    Adapter(const T& value) : value(value) 
    {}
    //! 
    Adapter(const Adapter& rhs) : value(rhs.value) 
    {}
    //!
    Adapter& operator=(const T& value) 
    { 
        this->value = value;
        return *this; 
    };
    //!
    operator T&()
    {
        return value;
    }

    //!
    T& get() 
    { 
        return value; 
    }
    //!
    const T& get() const 
    { 
        return value; 
    }
};

/**
 *	Typ wrapujący wskaźniki typów abstrakcyjnych, które nie mają klasy bazowej niezbędnej,
 *  aby mogły być używane w "mądrych" wskaźnikach.
 */
template <class T, class PtrPolicy>
class PtrWrapper : public PtrPolicy::Base
{
private:
    //! Wartość.
    T * ptr;

public:
    //!
    PtrWrapper() : ptr(NULL) 
    {}
    //!
    PtrWrapper(T* ptr) : ptr(ptr) 
    {}
    //! 
    PtrWrapper(const PtrWrapper& rhs) : ptr(rhs.ptr) 
    {}
    //!
    template <class Other> 
    PtrWrapper(const PtrWrapper<Other, PtrPolicy>& rhs) : ptr(rhs.ptr) 
    {}
    //!
    ~PtrWrapper() 
    { 
        delete ptr; 
    }

    //!
    PtrWrapper& operator = (T* ptr) 
    { 
        if ( ptr != this->ptr ) {
            delete this->ptr;
            this->ptr = ptr; 
        }
        return *this; 
    };

    //! implicit output conversion
    operator T*() const 
    { 
        return ptr; 
    }
    //!
    T& operator*() const 
    { 
        return *ptr; 
    }
    //!
    T* operator->() const
    { 
        return ptr; 
    }
    //!
    T* get() const 
    { 
        return ptr; 
    }
};


////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // HEADER_GUARD__UTILS_PTRWRAPPER_H__
