/********************************************************************
	created:    2010/05/14
	created:    14:5:2010     10:13
	filename: Utils.h
	author:	    Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__UTILS_H__
#define HEADER_GUARD__UTILS_H__

#include <utils/Config.h>
#include <utils/Macros.h>
#include <utils/SmartPtr.h>
#include <cstring>
#include <sstream>
#include <type_traits>
#include <memory>
#include <functional>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <csetjmp>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////	

	//! \tparam T Typ który "czyścimy"
	template<typename T>
	//! Klasa pomocnicza przy "oczyszczaniu" typów
	struct remove_toplevel
	{
		using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
	};

	//! \tparam T Typ który "czyścimy"
	template<typename T>
	//! Klasa pomocnicza przy "oczyszczaniu" typów
	struct add_const_reference
	{
		using type = typename std::add_lvalue_reference<typename std::add_const<T>::type>::type;
	};

	//! \tparam T Typ dla którego próbujemy ustalić najmniejszy możliwy krok
	template< class T >
	//! Klasa pomocnicza przy określaniu najmniejszego kroku dla danego typu
	class numeric_limits
	{
	public:
		//! \return Najmniejszy krok dla danego typu
		static inline T smallestStep() { return (std::numeric_limits<T>::is_integer == true) ? T(1) : std::numeric_limits<T>::epsilon(); }
	};

	namespace impl
	{
		template<typename T>
		struct wider_type {};

		template<> struct wider_type<char> { typedef short type; };
		template<> struct wider_type<signed char> { typedef signed short type; };
		template<> struct wider_type<unsigned char> { typedef unsigned short type; };
		template<> struct wider_type<char16_t> { typedef char32_t type; };
		template<> struct wider_type<char32_t> { typedef unsigned long type; };
		template<> struct wider_type<short> { typedef long type; };
		//template<> struct wider_type<unsigned short> { typedef unsigned long type; };
		template<> struct wider_type<int> { typedef long type; };
		//template<> struct wider_type<unsigned int> { typedef unsigned long type; };
		template<> struct wider_type<long> { typedef long long type; };
		template<> struct wider_type<unsigned long> { typedef unsigned long long type; };
		template<> struct wider_type<long long> { typedef float type; };
		template<> struct wider_type<unsigned long long> { typedef float type; };
		template<> struct wider_type<float> { typedef double type; };
		template<> struct wider_type<double> { typedef long double type; };
		template<> struct wider_type<long double> { typedef long double type; };
	}

	struct arithmetic_traits
	{
		template<typename T, typename U>
		using larger_type = typename std::common_type<T,U>::type;	

		template<typename T>
		using wider_type = impl::wider_type<T>;

		template<typename T, typename U>
		struct safe_sum_type { typedef typename wider_type<typename larger_type<T, U>::type>::type type; };

		template<typename T>
		struct safe_sum_type<T, T> { typedef typename wider_type<T>::type type; };

		template<typename T, typename U>
		using safe_subtract_type = safe_sum_type<T, U>;		

		template<typename T, typename U>
		using safe_multiply_type = safe_sum_type<T, U>;

		template<typename T, typename U>
		using safe_division_type = safe_sum_type<T, U>;

		template<typename T, typename U>
		using safe_modulo_type = safe_sum_type<T, U>;
	};

//------------------------------------------------------------------------------

//! \tparam T Typ wartości zmiennoprzecinkowej 
template<typename T>
//! \param value Wartość dla której eliminujemy ujemne zero
//! \return Wartość nieujemna
static inline T correctNegativeZero(const T value)
{
	static_assert(std::is_floating_point<T>::value, "T must be floating point type");
	if (value == T(0)){
		return std::fabs(value);
	}

	return value;
}

//------------------------------------------------------------------------------
//! \tparam Typ wartości dla której wyznaczamy znak
template <typename T>
//! \param value Wartość dla której wyciągamy znak
//! \return 1, 0 albo -1 w zależności od znaku
static inline char sign(const T x, std::false_type)
{
	return (T(0) < x) == true ? 1 : 0;
}

//! \tparam Typ wartości dla której wyznaczamy znak
template <typename T>
//! \param value Wartość dla której wyciągamy znak
//! \return 1, 0 albo -1 w zależności od znaku
static inline char sign(const T x, std::true_type)
{
	return ((T(0) < x) == true ? 1 : 0) - ((x < T(0)) == true ? 1 : 0);
}

//! \tparam Typ wartości dla której wyznaczamy znak
template <typename T>
//! \param value Wartość dla której wyciągamy znak
//! \return 1, 0 albo -1 w zależności od znaku
static inline char sign(const T x)
{
	static_assert(std::is_arithmetic<T>::value, "T must be arithmetic type");
	return sign(x, std::is_signed<T>());
}

//------------------------------------------------------------------------------

//! \tparam T Typ dla któego wykonujemy swap bajtów
//! \param value Wartość dla któej wykonujemy swap bajtów
//! \return Wartość ze zmienioną kolejnością bajtów
template<typename T>
static inline T EndianSwap(const T value)
{
	union
	{
		T u;
		uint8_t u8[sizeof(T)];
	} es;

	es.u = value;

	std::reverse(es.u8, es.u8 + sizeof(T));

	return es.u;
};

//------------------------------------------------------------------------------

//! Zeruje zadany obiekt.
//! \tparam T Typ zerowanego obiektu
template <typename T>
//! \param object Obiekt do wyzerowania.
static inline void zero(T & object)
{
    memset(&object, 0, sizeof(T));
}

//------------------------------------------------------------------------------
//! \tparam T
template<typename T>
static inline typename std::decay<T>::type decay_copy(T && val)
{ // forward val as value of decayed type
	return (std::forward<T>(val));
}

//------------------------------------------------------------------------------

//! Zwalnia zadany wskaźnik oraz zeruje jego wartość.
//! \tparam T Typ wskaźnika
template <typename T>
//! \param ptr Wskaźnik do zwolnienia.
static inline void deletePtr(T *& ptr)
{
	UTILS_DELETEPTR(ptr);
}

//------------------------------------------------------------------------------

struct NullType {};

//------------------------------------------------------------------------------

//! Klasa pomagająca realizować zadania przy kończeniu kontekstu w momencie niszczenia obiektu
class Cleanup
{
	private:
		//! Prywatna implementacja
		class CleanupPrivateBase
		{
		public:
			//! Destuktor wirtualny
			virtual ~CleanupPrivateBase() {}
		};

		//! \tparam T Typ operacji do wykonania
		template<typename T>
		//! Implementacja
		class CleanupPrivate : public CleanupPrivateBase
		{
		public:

			template<typename TT = T>
			//! \param t Przenoszona operacja
			CleanupPrivate(TT && t) : t(std::forward<TT>(t)) {}
			//! Destruktor wirtualny
			virtual ~CleanupPrivate() { t(); }
		private:
			//! Operacja
			T t;
		};

public:		

	//! \param cleanup Obiekt uywany do czyszczenia
	template<typename T>
	Cleanup(T && t) : cleanup(utils::make_shared<CleanupPrivate<T>>(std::forward<T>(t))) {}

	//! Destruktor
	~Cleanup() {}
private:
	//! Obiekt czyszczacy
	utils::shared_ptr<CleanupPrivateBase> cleanup;
};

//------------------------------------------------------------------------------

//! \tparam T Typ kolekcji nieuporządkowanej poddawanej łączeniu
//! \param a Pierwsza kolekcja łączona
//! \param b Druga kolekcja łączona
//! \return Kolekcja będąca rezultatem dołączonia do kolekcji a kolekcji b
template<typename T>
static inline T mergeUnordered(const T & a, const T & b)
{
	T ret(a);
	ret.insert(ret.end(), b.begin(), b.end());
	return ret;
}

//------------------------------------------------------------------------------

//! \tparam T Typ kolekcji uporządkowanej poddawanej łączeniu
//! \param a Pierwsza kolekcja łączona
//! \param b Druga kolekcja łączona
//! \return Kolekcja będąca rezultatem dołączonia elementó kolekcji b do elementów kolekcji kolekcji a
template<typename T>
static inline T mergeOrdered(const T & a, const T & b)
{
	T ret(a);
	ret.insert(b.begin(), b.end());
	return ret;
}

//! \tparam I Identyfikator
template <int I>
//! Idio realizujący mapowanie typu do liczby
struct Int2Type
{
	enum { value = I };
};

template<typename Base, typename Tag>
class TypeTagWrapper : public Base
{
public:

	using type = Tag;

public:

	template<typename... Args>
	TypeTagWrapper(Args&&... args) : Base(std::forward<Args>(args)...) {}

	virtual ~TypeTagWrapper() {}
};

template<typename Base, std::size_t Tag>
class IntegralTagWrapper : public Base
{
public:

	enum {
		type = Tag
	};

public:

	template<typename... Args>
	IntegralTagWrapper(Args&&... args) : Base(std::forward<Args>(args)...) {}

	virtual ~IntegralTagWrapper() {}
};

//! \tparam T Wartość logiczna do weryfikacji
//! \tparam U Wartość logiczna do weryfikacji
template<bool T, bool U>
//! Typ pomocniczy realizujący statycznie funkcję logiczną xor
struct xor_check : public std::integral_constant<bool, T != U> {};

#define ENABLE_IF(...) typename std::enable_if<(__VA_ARGS__)>::type * = nullptr

////////////////////////////////////////////////////////////////////////////////
} // namespace util
////////////////////////////////////////////////////////////////////////////////


#endif    // HEADER_GUARD__UTILS_H__
