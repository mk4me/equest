/********************************************************************
	created:  2010/10/04
	created:  4:10:2010   15:30
	filename: LeakDetection.h
	author:	  Piotr Gwiazdowski
	
	purpose:  Nagłówek należy załączać tylko w głównym pliku projektu.
              Dodanie go więcej razy może spowodować błędy linkera.
*********************************************************************/
#ifndef HEADER_GUARD__CORE_LEAKDETECTION_H__
#define HEADER_GUARD__CORE_LEAKDETECTION_H__

#include <utils/Config.h>

#if defined(_WINDOWS)

#   ifdef UTILS_ENABLE_LEAK_DETECTION_VLD

// na razie VLD nie jest włączone do projektu tak jak należy; trzeba
// ściągnąć ręcznie (http://sites.google.com/site/dmoulding/vld) oraz
// samemu skonfigurować (readme po instalacji)
#include <vld.h>

#   else // UTILS_ENABLE_LEAK_DETECTION_VLD

/**
 *	Prosty typ uruchamiający śledzenie leaków z użyciem CRT. Poprzednio
 *  znajdował się w pliku .cpp, ale zgodnie z polityką usuwania konfiguracji z utilsów
 *  znajduje się tutaj.
 *  Jeżeliby przenosić ten typ do .cpp, to powinno się dać hinta linkerowi, aby
 *  instancjonował statyczny obiekt:
 *  #pragma comment(linker, "/include:?__leakDetector@@3V__CrtLeakDetector@@A")
 */
class __CrtLeakDetector
{
public:
    __CrtLeakDetector()
    {
        _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    }
    ~__CrtLeakDetector()
    {
    }
} __leakDetector;

#   endif // UTILS_ENABLE_LEAK_DETECTION_VLD

#else // __WIN32__

// jak to zrobić na unixie?

#endif // __WIN32__

#endif  // HEADER_GUARD__CORE_PLUGIN_H__
