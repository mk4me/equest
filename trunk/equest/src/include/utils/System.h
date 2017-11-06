/********************************************************************
	created:    2010/08/11
	created:    11:8:2010     11:06
	filename: System.h
	author:	    Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__SYSTEM_H__
#define HEADER_GUARD__SYSTEM_H__

#include <utils/Config.h>
#if defined(_WINDOWS)
#define NOMINMAX
#include <Windows.h>
#elif defined(__UNIX__)
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#endif

#include <utils/Debug.h>


////////////////////////////////////////////////////////////////////////////////
namespace utils {
namespace __system {
////////////////////////////////////////////////////////////////////////////////

#if defined(_WINDOWS)

//! Typ tyknięcia zegara.
typedef long long Tick;
//! ID wątku.
typedef unsigned ThreadID;

//! \return Tyknięcia zegara o wysokiej częstotliwości.
inline Tick getTick()
{
    LARGE_INTEGER nCurrTime;
    QueryPerformanceCounter(&nCurrTime);
    return nCurrTime.QuadPart;
}

//! \return Tyknięcia zegara wyrażone w sekundach.
inline double tickToSeconds(Tick value)
{
    static LARGE_INTEGER frequency;
    static BOOL __init = QueryPerformanceFrequency(&frequency);
    UTILS_ASSERT(__init);
    return static_cast<double>(value)/static_cast<double>(frequency.QuadPart);
}

//!
inline ThreadID getCurrentThreadID()
{
    return GetCurrentThreadId();
}

//!
struct Mutex : public CRITICAL_SECTION
{
    inline ~Mutex()
    {
        DeleteCriticalSection(this);
    }

    inline static Mutex* create()
    {
        Mutex* result = new Mutex();
        InitializeCriticalSection(result);
        return result;
    }

    inline void lock()
    {
        EnterCriticalSection(this);
    }

    inline void unlock()
    {
        LeaveCriticalSection(this);
    }

private:
    Mutex() {}
};


#else

//! Typ tyknięcia zegara.
typedef long long Tick;
//! ID wątku.
typedef pthread_t ThreadID;

//! \return Tyknięcia zegara o wysokiej częstotliwości.
inline Tick getTick()
{
    struct timespec tp;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
    // rezultat w mikrosekundach
    Tick result;
    result = 1000000 * tp.tv_sec + tp.tv_nsec / 1000;
    return result;
}

//! \return Tyknięcia zegara wyrażone w sekundach.
inline double tickToSeconds(Tick value)
{
    return value / 1000000.0;
}

//!
inline ThreadID getCurrentThreadID()
{
    return pthread_self();
}

//!
struct Mutex
{
    pthread_mutex_t mutex;
    inline ~Mutex()
    {
        int err = pthread_mutex_destroy(&mutex);
        UTILS_ASSERT(err == 0, "Destroying mutex failed.");
    }

    inline static Mutex* create()
    {
        Mutex* result = new Mutex();
        int err = pthread_mutex_init(&result->mutex, NULL);
        UTILS_ASSERT(err == 0, "Creating mutex failed.");
        return result;
    }

    inline void lock()
    {
        pthread_mutex_unlock(&mutex);
    }

    inline void unlock()
    {
        pthread_mutex_lock(&mutex);
    }

private:
    Mutex() {}
};

#endif



////////////////////////////////////////////////////////////////////////////////
} // namespace __system
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif    // HEADER_GUARD__SYSTEM_H__
