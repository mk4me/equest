/********************************************************************
	created:    2010/07/15
	created:    15:7:2010     8:50
	filename: Pool.h
	author:	    Piotr Gwiazdowski
	
	purpose:    
*********************************************************************/
#ifndef HEADER_GUARD__POOL_H__
#define HEADER_GUARD__POOL_H__

#include <utils/Config.h>
#include <utils/Align.h>
#include <utils/Utils.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Typ przechowujący obiekty dowolnych typów w wewnętrznym buforze.
 */
class Pool
{
private:
    //! Makro definiujące typ Align1st
    #define UTILS_POOL_ALIGNS1(type1) \
        typedef AlignToType<0, type1> Align1st
    //! Makro definiujące typ Align1st i Align2nd
    #define UTILS_POOL_ALIGNS2(type1, type2) \
        UTILS_POOL_ALIGNS1(type1);\
        typedef AlignToType<Align1st::value + sizeof(type1), type2> Align2nd
    //! Makro definiujące typ Align1st, Align2nd i Align3rd
    #define UTILS_POOL_ALIGNS3(type1, type2, type3) \
        UTILS_POOL_ALIGNS2(type1, type2);\
        typedef AlignToType<Align2nd::value + sizeof(type2), type3> Align3rd

    //! Interfejs obiektu zdolnego wywoływać destruktory innych obiektów.
    class DestructorCaller
    {
    public:
        //! Pusty polimorficzny destruktor.
        virtual ~DestructorCaller() {}
        //! \param addr Adres jakiegoś obiektu.
        virtual void destroy(unsigned char * addr) = 0;
    };

    //! Implementacja interfejsu dla jednego konkretnego typu.
    template <class T>
    class Destructor1 : public DestructorCaller
    {
    public:
        virtual void destroy(unsigned char * addr)
        {
            UTILS_POOL_ALIGNS1(T);
            reinterpret_cast<T*>(addr + Align1st::value)->~T();
        }
    };

    //! Implementacja interfejsu dla dwóch sekwencyjnie ułożonych typów.
    template <class T1, class T2>
    class Destructor2 : public Destructor1<T1>
    {
    public:
        virtual void destroy(unsigned char * addr)
        {
            UTILS_POOL_ALIGNS2(T1, T2);
            Destructor1<T1>::destroy(addr);
            reinterpret_cast<T2*>(addr + Align2nd::value)->~T2();
        }
    };

    //! Implementacja interfejsu dla trzech sekwencyjnie ułożonych typów.
    template <class T1, class T2, class T3>
    class Destructor3 : public Destructor2<T1, T2>
    {
    public:
        virtual void destroy(unsigned char * addr)
        {
            UTILS_POOL_ALIGNS3(T1, T2, T3);
            Destructor2<T1, T2>::destroy(addr);
            reinterpret_cast<T3*>(addr + Align3rd::value)->~T3();
        }
    };

    //! Implementacja interfejsu dla tablicy konkretnego typu.
    template <class T, int Size>
    class ArrayDectructor : public DestructorCaller
    {
    public:
        virtual void destroy(unsigned char * addr)
        {
            for (int i = 0; i < Size; ++i) {
                (reinterpret_cast<T*>(addr)+i)->~T();
            }
        }
    };

private:
    //! Surowe dane. Zapewniamy im odpowiedni align.
    union {
        long long aligner;
        unsigned char rawData [64];
    };
    //! Destruktor obiektu.
    DestructorCaller * destructor;

public:
    //! Konstruktor ustawiający parametry.
    //! \param sender
    Pool() :
        destructor(NULL)
    {
        utils::zero(rawData);
    }

    //! \param sender
    //! \param data
    template <class T1>
    Pool(const T1 & data)
    {
        // tworzymy kopię w naszych surowych danych
        // jeśli wystąpi błąd oznacza to, że rozmiar struktury jest większy niż rozmiar bufora
        UTILS_POOL_ALIGNS1(T1);
        static_assert(Align1st::value+sizeof(T1)<=sizeof(rawData), "Overflow");
        new (rawData + Align1st::value) T1(data);
        destructor = new Destructor1<T1>();
    }

    //! \param sender
    //! \param data
    template <class T1, class T2>
    Pool(const T1 & data1, const T2 & data2)
    {
        // tworzymy kopię w naszych surowych danych
        // jeśli wystąpi błąd oznacza to, że rozmiar struktury jest większy niż rozmiar bufora
        UTILS_POOL_ALIGNS2(T1, T2);
        static_assert(Align2nd::value+sizeof(T2)<=sizeof(rawData), "Overflow");
        new (rawData+Align1st::value) T1(data1);
        new (rawData+Align2nd::value) T2(data2);
        destructor = new Destructor2<T1, T2>();
    }

    //! \param sender
    //! \param data
    template <class T1, class T2, class T3>
    Pool(const T1 & data1, const T2 & data2, const T3 & data3)
    {
        // tworzymy kopię w naszych surowych danych
        // jeśli wystąpi błąd oznacza to, że rozmiar struktury jest większy niż rozmiar bufora
        UTILS_POOL_ALIGNS3(T1, T2, T3);
        static_assert(Align3rd::value+sizeof(T3)<=sizeof(rawData), "Overflow");
        new (rawData+Align1st::value) T1(data1);
        new (rawData+Align2nd::value) T2(data2);
        new (rawData+Align3rd::value) T3(data3);
        destructor = new Destructor3<T1, T2, T3>();
    }

    //! 
    //! \param sender
    //! \param data
    template <class T, int Size>
    Pool(T (&data)[Size])
    {
        // tworzymy kopię w naszych surowych danych
        // jeśli wystąpi błąd oznacza to, że rozmiar struktury jest większy niż rozmiar bufora
        static_assert(sizeof(T) * Size <= sizeof(rawData), "Overflow");
        for (int i = 0; i < Size; ++i) {
            new (rawData+i*sizeof(T)) T(data[i]);
        }
        destructor = new ArrayDectructor<T, Size>();
    }

    //! Destruktor wirtualny, dla klas pochodnych.
    virtual ~Pool()
    {
        if ( destructor ) {
            destructor->destroy(rawData);
            delete destructor;
            destructor = NULL;
        }
    }

    //! Pomocnicza funkcja. Zwraca referencję do pierwszego parametru.
    //! \param dummy Nie podawać nic tylko specjalizować!
    //! \return Referencja do pierwszego parametru.
    template <class T>
    T& getValue(T * dummy = NULL)
    {
        return *reinterpret_cast<T*>(rawData);
    }

    //! Ustawia wskaźnik na pierwszy parametr.
    //! \param value1
    template <class T1>
    void getValues(T1 *& value1)
    {
        UTILS_POOL_ALIGNS1(T1);
        value1 = reinterpret_cast<T1*>(rawData+Align1st::value);
    }

    //! Ustawia wskaźniki na dwa parametry.
    //! \param value1
    //! \param value2
    template <class T1, class T2>
    void getValues(T1 *& value1, T2 *& value2)
    {
        UTILS_POOL_ALIGNS2(T1, T2);
        value1 = reinterpret_cast<T1*>(rawData+Align1st::value);
        value2 = reinterpret_cast<T2*>(rawData+Align2nd::value);
    }

    //! Ustawia wskaźniki na dwa parametry.
    //! \param value1
    //! \param value2
    //! \param value3
    template <class T1, class T2, class T3>
    void getValues(T1 *& value1, T2 *& value2, T3 *& value3)
    {
        UTILS_POOL_ALIGNS3(T1, T2, T3);
        value1 = reinterpret_cast<T1*>(rawData+Align1st::value);
        value2 = reinterpret_cast<T2*>(rawData+Align2nd::value);
        value3 = reinterpret_cast<T3*>(rawData+Align3rd::value);
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif    // HEADER_GUARD__POOL_H__
