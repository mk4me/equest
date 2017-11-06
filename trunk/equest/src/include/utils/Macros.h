/********************************************************************
created:  2010/04/23
created:  23:4:2010   7:58
filename: Macros.h
author:	  Piotr Gwiazdowski

purpose:
*********************************************************************/

#ifndef HEADER_GUARD_UTILS__MACROS_H__
#define HEADER_GUARD_UTILS__MACROS_H__

//------------------------------------------------------------------------------
#if _MSC_VER >= 1600 || (defined(__GNUC__) && (__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)))
	#define UTILS_CXX0X
#else
	#error "Unsupported compiler - C++11 support required"
#endif

//------------------------------------------------------------------------------
// Visual studio jeszcze nie wspiera noexcept, stad to makro
#ifndef _MSC_VER
	#define NOEXCEPT noexcept
#else
	#define NOEXCEPT
#endif

//------------------------------------------------------------------------------
// Sugerowany początek makra składającego się z wielu instrukcji
#define UTILS_MULTISTATEMENT_BEGIN  do {

//------------------------------------------------------------------------------
// Sugerowany koniec makra składającego się z wielu instrukcji
#ifdef _MSC_VER
#   define UTILS_MULTISTATEMENT_END     \
    __pragma(warning(push))             \
    __pragma(warning(disable: 4127))    \
			    } while(0) /* brak ; na koncu! */   \
    __pragma(warning(pop))
#else
#   define UTILS_MULTISTATEMENT_END    } while(0) /* brak ; na koncu! */
#endif

//------------------------------------------------------------------------------
//! Zwalnia zadany wskaźnik oraz zeruje jego wartość.
#define UTILS_DELETEPTR(ptr) UTILS_MULTISTATEMENT_BEGIN \
    delete ptr; \
    ptr = nullptr; \
    UTILS_MULTISTATEMENT_END

//------------------------------------------------------------------------------
/**
*	Definicja ostrzeżenia o przestarzałej funkcjonalności
*/
#ifdef __GNUC__
#   define UTILS_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#   define UTILS_DEPRECATED(func) __declspec(deprecated) func
#else
#   pragma basicMessage("WARNING: You need to implement DEPRECATED for this compiler")
#   define UTILS_DEPRECATED(func) func
#endif

//! Makro pozwalające generować string z ciągu strumieni
#define UTILS_FORMAT_STRING(ITEMS)												\
  ( ( dynamic_cast<std::ostringstream &> (										\
         std::ostringstream() . seekp( 0, std::ios_base::cur ) << ITEMS )		\
    ) . str() )

//------------------------------------------------------------------------------
/**
*	Makra zapamiętujące/odtwarzające stan warningów.
*/
#if defined(_MSC_VER)
#   define UTILS_PUSH_WARNINGS  __pragma(warning(push))
#   define UTILS_POP_WARNINGS   __pragma(warning(pop))
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING __pragma(warning(disable: 4251))
#elif defined(__GNUC__) && defined(UTILS_CXX0X)
#   define DO_PRAGMA(x) _Pragma (#x)
#   define UTILS_PUSH_WARNINGS  DO_PRAGMA(GCC diagnostic push)
#   define UTILS_POP_WARNINGS   DO_PRAGMA(GCC diagnostic pop)
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING
#else
#   pragma basicMessage("WARNING: You need to implement _Pragma for this compiler")
#   define UTILS_PUSH_WARNINGS
#   define UTILS_POP_WARNINGS
#   define UTILS_DISABLE_DLL_INTERFACE_WARNING
#endif

//------------------------------------------------------------------------------
// Makro podstawiające nazwę aktualnej funkcji
//TODO
//użyć BOOST_CURRENT_FUNCTION (boost/current_function.hpp)
#if defined __GNUC__
	#define UTILS_PORTABLE_FUNCTION_NAME __func__
#elif defined _MSC_VER
	#define UTILS_PORTABLE_FUNCTION_NAME __FUNCTION__
#else
	#error Provide macro name for retriving currently processed function name for current compiler!
#endif

//------------------------------------------------------------------------------
// Makro rzucające warningiem na konsole podczas kompilacji

#define UTILS_COMPILER_WARNING_GENERATOR(desc) "Custom compile-time-warning: " #desc " in " UTILS_PORTABLE_FUNCTION_NAME " -> " __FILE__ ":" STRINGIZE(__LINE__)

#if defined __GNUC__
#define UTILS_COMPILER_WARNING(desc) _Pragma ("\"UTILS_COMPILER_WARNING_GENERATOR(desc)\"")
#elif defined _MSC_VER
#define UTILS_COMPILER_WARNING(desc) __pragma(message(UTILS_COMPILER_WARNING_GENERATOR(desc)))
#else
#error Provide macro name for retriving currently processed function name for current compiler!
#endif

//------------------------------------------------------------------------------
#if defined(_DEBUG) || defined(DEBUG)
#define UTILS_DEBUG
#endif

//------------------------------------------------------------------------------
// Makro zamieniająca parametr na łańcuch.
#define STRINGIZE(arg)  __STRINGIZE(arg)
#define __STRINGIZE(arg) #arg

//------------------------------------------------------------------------------
// Makro łączące dwa parametry.
#define CONCATENATE(arg1, arg2)   __CONCATENATE(arg1, arg2)
#define __CONCATENATE(arg1, arg2)  arg1##arg2

//------------------------------------------------------------------------------
// Makro łączące dwa parametry z własnym separatorem
#define CUSTOM_SEPARATOR_CONCATENATE(arg1, arg2, separator)   __CUSTOM_SEPARATOR_CONCATENATE(arg1, arg2, separator)
#define __CUSTOM_SEPARATOR_CONCATENATE(arg1, arg2, separator)  arg1 ## separator ## arg2

//------------------------------------------------------------------------------
// Makro zamieniające definicję zawierającą przecinki na łańcuch.
#ifdef __GNUC__
// Dla gcc parametry trzeba "rozbić"
#define COMA_AWARE_STRINGIZE(...) # __VA_ARGS__ //__FOR_EACH(STRINGIZE, ",", __VA_ARGS__)
#else // __GNUC__
// Liczymy na to, że sobie poradzi (tak jak MSVC)
#define COMA_AWARE_STRINGIZE(...) __STRINGIZE(__VA_ARGS__)
#endif // __GNUC__

//------------------------------------------------------------------------------

#endif  // HEADER_GUARD_UTILS__MACROS_H__
