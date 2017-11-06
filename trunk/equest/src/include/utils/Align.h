/********************************************************************
	created:  2010/07/15
	created:  15:7:2010   8:08
	filename: Align.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__ALIGN_H__
#define HEADER_GUARD__ALIGN_H__

#include <utils/Config.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Wyrównuje adres do zadanego podzielnika.
/**
 *	Wyrównuje adres do zadanego podzielnika. Podzielnik powinien być potęgą
 *  dwójki.
 *  \param addr Adres.
 *  \param alignTo Podzielnik.
 */
#define UTILS_ALIGN(addr, alignTo) ( (addr&(alignTo-1)) ? ((addr&(~(alignTo-1)))+alignTo) : addr )

//! Wyrównuje adres do zadanego podzielnika.
//! \param addr Adres
//! \param alignTo Podzielnik
template <class T, class U>
inline T align(T addr, U alignTo)
{
    return UTILS_ALIGN(addr, alignTo);
}

//! Domyślny align danych.
/** Stała value w tej odmianie oraz w specjalizacjach mówi do jakiego adresu
 *  typ o zadanym rozmiarze ma być wyrównany. 
 */
template <int Size> struct Align { enum { value = 4 }; };
//! Align dla rozmiaru 1.
template <> struct Align<1> { enum { value = 1}; };
//! Align dla rozmiaru 2.
template <> struct Align<2> { enum { value = 2}; };
//! Align dla rozmiaru 3.
template <> struct Align<3> { enum { value = 4}; };

//! Pomocniczy typ, zamiast rozmiaru przyjmuje typ
template <class T> struct AlignType { 
    enum { value = Align<sizeof(T)>::value }; 
};

//! Typ wyrównujący adres przy zadanym wyrównaniu
template <int Addr, int AlignBytes> struct AlignTo {
    // Liczba bajtów musi być potega dwójki!
    static_assert((AlignBytes & (AlignBytes-1)) == 0, "Power of 2 error");
    //! Warunek: jeżeli ostatnie bajty nie są wyzerowane to je zeruję i dodaję bajty alignu;
    //! w przeciwnym wypadku wartość jest już wyalignowana.
    enum { value = UTILS_ALIGN(Addr, AlignBytes) };
};

//! Typ wyrównujący adres dla zadanego typu.
template <int Addr, class T> struct AlignToType {
    enum {
        value = AlignTo< Addr, AlignType<T>::value >::value
    };
};

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__ALIGN_H__
