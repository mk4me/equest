/********************************************************************
	created:  2010/08/05
	created:  5:8:2010   18:28
	filename: Debug.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__UTILS_DEBUG_H__
#define HEADER_GUARD__UTILS_DEBUG_H__

#include <utils/Config.h>
#include <utils/Macros.h>
#include <utils/Utils.h>
#include <sstream>
#include <boost/assert.hpp>
#include <chrono>
#include <thread>
#include <boost/lexical_cast.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

struct Debug
{
    //!
    //! \param file
    //! \param line
    //! \param format
    //! \param
    static void print(const char * file, int line, const char * format, ...);
    //!
    //! \param file
    //! \param line
    //! \param format
    //! \param
    static void formatMessage(char * buffer, int bufferSize, const char * format, ...);
    //! Zaślepka. Pomaga w budowaniu makr.
    //! \param buffer
    //! \param bufferSize
    static void formatMessage(char * buffer, int bufferSize);
};


//! \tparam TimeT Typ czasu
//! \tparam ClockT Typ zegara
template<typename TimeT = std::chrono::milliseconds,
	typename ClockT = std::chrono::high_resolution_clock>
//! Klasa pozwalająca mierzyć czas wykonania
struct MeasureDuration
{
	//! \tparam F Typ funkcji
	//! \tparam Args Typ(y) arugmentu(ów) funkcji
	//! \return Czas wykonania
	template<typename F, typename ...Args>
	inline static typename TimeT::rep execute(F & f, Args&&... args)
	{
		const auto start = ClockT::now();
		
		f(std::forward<Args>(args)...);

		return std::chrono::duration_cast<TimeT>(ClockT::now() - start).count();
	}
};

//! RAII dla mierzenia czasu życia obiektu - może posłużyć do profilowania funkcji -> pierwszy obiekt w ciele funkcji
class ScopeTimePrinter
{
public:
	//! \param scopeName Nazwa zakresu
	ScopeTimePrinter(const std::string& scopeName);
	//! \param scopeName Nazwa zakresu
	//! \param file Plik
	//! \param line Linia w pliku
	ScopeTimePrinter(const std::string& scopeName, const char * file, int line);
	//! Destruktor wirtualny
	virtual ~ScopeTimePrinter();
private:
	//! Nazwa zakresu
	std::string scopeName;
	//! Czas rozpoczęcia życia obiektu
	std::chrono::high_resolution_clock::time_point tick;
};

#define DEFAULT_SCOPETIMEPRINTER_NAME __stp__

#define CUSTOM_SIMPLE_ST_TIME_PRINTER(var, name) ScopeTimePrinter var(name);
#define CUSTOM_EXTENDED_ST_TIME_PRINTER(var, name) ScopeTimePrinter var(name, __FILE__, __LINE__);

#define DEFAULT_SIMPLE_ST_TIME_PRINTER(name) ScopeTimePrinter DEFAULT_SCOPETIMEPRINTER_NAME(name);
#define DEFAULT_EXTENDED_ST_TIME_PRINTER(name) ScopeTimePrinter DEFAULT_SCOPETIMEPRINTER_NAME(name, __FILE__, __LINE__);

#define CUSTOM_SIMPLE_MT_TIME_PRINTER(var, name) ScopeTimePrinter var(std::string(name) + " - thread " + boost::lexical_cast<std::string>(std::this_thread::get_id()));
#define CUSTOM_EXTENDED_MT_TIME_PRINTER(var, name) ScopeTimePrinter var(std::string(name) + " - thread " + boost::lexical_cast<std::string>(std::this_thread::get_id()), __FILE__, __LINE__);

#define DEFAULT_SIMPLE_MT_TIME_PRINTER(var, name) ScopeTimePrinter DEFAULT_SCOPETIMEPRINTER_NAME(std::string(name) + " - thread " + boost::lexical_cast<std::string>(std::this_thread::get_id()));
#define DEFAULT_EXTENDED_MT_TIME_PRINTER(var, name) ScopeTimePrinter DEFAULT_SCOPETIMEPRINTER_NAME(std::string(name) + " - thread " + boost::lexical_cast<std::string>(std::this_thread::get_id()), __FILE__, __LINE__);

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

/**
 *	Definicja funkcji wypisującej do konsoli w trybie debug.
 */
#ifdef UTILS_DEBUG
#   define UTILS_DEBUG_PRINT(format, ...) utils::Debug::print(__FILE__, __LINE__, format, __VA_ARGS__)
#else
#   define UTILS_DEBUG_PRINT(...)
#endif

/**
 *	Definicja asercji.
 */
#ifdef UTILS_DEBUG

#define UTILS_ASSERT(condition, ...) BOOST_ASSERT_MSG(condition, COMA_AWARE_STRINGIZE(#condition,__VA_ARGS__))
#define UTILS_FAIL(...) UTILS_ASSERT(false, COMA_AWARE_STRINGIZE(failure,__VA_ARGS__))

#else
#   define UTILS_ASSERT(...)
#   define UTILS_FAIL(...)
#endif




#endif  // HEADER_GUARD__UTILS_DEBUG_H__
