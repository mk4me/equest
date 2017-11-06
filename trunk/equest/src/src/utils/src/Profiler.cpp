#include <iostream>
#include <sstream>
#include <utils/Debug.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <utils/Macros.h>
#include <utils/System.h>

#if defined(_WINDOWS)
#define NOMINMAX
#include <Windows.h>
#endif
#include <utils/Profiler.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

//! Globalny muteks.
__system::Mutex* mutex = NULL;
//! Instancja.
Profiler Profiler::instance;
//! Domyślna nazwa.
const char defaultName[] = "::global";

//------------------------------------------------------------------------------

class ScopedLock
{
private:
    //! Faktyczny mechanizm blokujący.
    __system::Mutex* lock;

public:
    //! \param Mechanizm blokujący. Gdy NULL nic się nie dzieje.
    ScopedLock(__system::Mutex* lock)
    {
        if ( (this->lock = lock) != NULL ) {
            lock->lock();
        }
    }

    //! Zwalnia blokadę.
    ~ScopedLock()
    {
        if (lock) {
            lock->unlock();
        }
    }

private:
    //! Zapobiega kopiowaniu.
    ScopedLock(const ScopedLock&);
    //! Zapobiega kopiowaniu.
    ScopedLock& operator=(const ScopedLock&);

};

//------------------------------------------------------------------------------

// Funkcje pomocnicze: zwracają indeks listy stanów badając id bez isntrukcji
// warunkowych (po kompilacji)

inline Profiler::threadID getListIndex( Profiler::threadID id, const Profiler::ThreadEntry ((&list)[1] ) )
{
    return (list[0].first != id) ? -1 : 0;
}

inline Profiler::threadID getListIndex( Profiler::threadID id, const Profiler::ThreadEntry ((&list)[2] ) )
{
    Profiler::threadID mod0 = (list[0].first != id) ? 0 : 1;
    Profiler::threadID mod1 = (list[1].first != id) ? 0 : 2;
    return mod0 + mod1 - 1;
}

inline Profiler::threadID getListIndex( Profiler::threadID id, const Profiler::ThreadEntry ((&list)[3] ) )
{
    Profiler::threadID mod0 = (list[0].first != id) ? 0 : 1;
    Profiler::threadID mod1 = (list[1].first != id) ? 0 : 2;
    Profiler::threadID mod2 = (list[2].first != id) ? 0 : 3;
    return mod0 + mod1 + mod2 - 1;
}

inline Profiler::threadID getListIndex( Profiler::threadID id, const Profiler::ThreadEntry ((&list)[4] ) )
{
    Profiler::threadID mod0 = (list[0].first != id) ? 0 : 1;
    Profiler::threadID mod1 = (list[1].first != id) ? 0 : 2;
    Profiler::threadID mod2 = (list[2].first != id) ? 0 : 3;
    Profiler::threadID mod3 = (list[3].first != id) ? 0 : 4;
    return mod0 + mod1 + mod2 + mod3 - 1;
}

inline Profiler::threadID getListIndex( Profiler::threadID id, const Profiler::ThreadEntry ((&list)[5] ) )
{
    Profiler::threadID mod0 = (list[0].first != id) ? 0 : 1;
    Profiler::threadID mod1 = (list[1].first != id) ? 0 : 2;
    Profiler::threadID mod2 = (list[2].first != id) ? 0 : 3;
    Profiler::threadID mod3 = (list[3].first != id) ? 0 : 4;
    Profiler::threadID mod4 = (list[4].first != id) ? 0 : 5;
    return mod0 + mod1 + mod2 + mod3 + mod4 - 1;
}

//------------------------------------------------------------------------------

// Funktory działające na stanach wątków

struct ThreadEntryToCSV
{
    //! Strumień docelowy.
    std::ostringstream * output;
    //! Czy zapisywać kolumnę z ID wątku?
    bool threadColumn;
    //!
    void operator()(const Profiler::ThreadEntry& entry)
    {
        Profiler::getInstance().stateToCSV( *entry.second, *output, threadColumn ? &entry.first : NULL );
    }
};

struct ThreadCombiner
{
    //! Stan docelowy.
    Profiler::State * state;
    //!
    void operator()(const Profiler::ThreadEntry& entry)
    {
        const Profiler::State* toCopy = entry.second;
        state->duration += toCopy->duration;
        for (size_t i = 0; i < UTILS_PROFILER_MAX_ENTRIES; ++i) {
            const Profiler::Measurement* src = toCopy->measurements + i;
            Profiler::Measurement* dst = state->measurements + i;
            dst->entrances += src->entrances;
            dst->ticks += src->ticks;
        }

    }
};

struct DeleteState
{
    void operator()(Profiler::ThreadEntry& entry)
    {
        delete entry.second;
    }
    void operator()(Profiler::ThreadMap::value_type& entry)
    {
        delete entry.second;
    }
};

//------------------------------------------------------------------------------

Profiler::Profiler()
{
    nextID = 0;
    start = getTick();
    enabled = false;
    overhead = 0;
    duration = 0;
    enabledStart = 0;
    enabledDuration = 0;
    perThreadDumpEnabled = false;
    autoDumpFile = "Profiler.cvs";
    mutex = __system::Mutex::create();
}

Profiler::~Profiler()
{
    // wyłączenie
    duration = getTick() - start;
    setEnabled(false);

    // zapis do pliku
    if ( !autoDumpFile.empty() && (threadCount != 0 || !inactiveThreads.empty()) ) {
        std::ofstream output(autoDumpFile.c_str(), std::ios_base::out);
        output << createCSV();
    }

    // zwolnienie danych
    UTILS_ASSERT(threadCount == 0, "Nie powinno być aktywnych wątków!");
    DeleteState func;
    forEachThread(func);

    delete mutex;
}

void Profiler::setEnabled( bool enabled )
{
    if ( enabled && !instance.enabled ) {
        // rozpoczęcie
        instance.enabledStart = getTick();
    } else if (!enabled && instance.enabled) {
        // koniec
        tick delta = getTick() - instance.enabledStart;
        instance.enabledDuration += delta;
    }
    instance.enabled = enabled;
}

template <class FwdIter>
void Profiler::scopeToCSV( std::ostringstream& str, FwdIter first, FwdIter last )
{
    for ( ; first != last; ++first) {
        threadID id = first->first;
        State* state = first->second;

        int count = sizeof(state->measurements) / sizeof(Measurement);
        for (int i = 0; i < count; ++i) {
            Measurement* measurement = state->measurements + i;
            if ( measurement->entrances ) {
                ProfilerEntry* entry = entires[i];
                std::size_t nameLength;
                const char* typeName = entry->getTypeName(nameLength);
                if ( perThreadDumpEnabled ) {
                    str<<id<<'\t';
                }
                // zapisanie kolejnych kolumn
                str<<
                    std::string(typeName, nameLength)<<'\t'<<
                    entry->getFunctionName()<<'\t'<<
                    measurement->entrances<<'\t'<<
                    tickToSeconds(measurement->ticks)<<'\t'<<
                    tickToSeconds(measurement->ticks)/measurement->entrances<<'\t'<<
                    100.0 * measurement->ticks/state->duration<<'\t'<<
                    100.0 * measurement->ticks/duration<<'\t'<<
                    entry->signature<<std::endl;
            }
        }
    }
}


template <class Func>
void utils::Profiler::forEachThread( Func func )
{
    // najpierw sprawdzane aktywne
    if ( threadCount <= UTILS_PROFILER_LIST_SIZE ) {
        std::for_each(threadList, threadList + threadCount, func);
    } else {
        std::for_each(threadMap.begin(), threadMap.end(), func);
    }
    std::for_each(inactiveThreads.begin(), inactiveThreads.end(), func);
}

void Profiler::dump( std::ostream& output )
{
    ScopedLock lock(mutex);
    instance.duration = getTick() - instance.start;
    output << instance.createCSV();
}

std::string Profiler::dump()
{
    ScopedLock lock(mutex);
    instance.duration = getTick() - instance.start;
    return instance.createCSV();
}

std::string Profiler::createCSV()
{
    std::ostringstream output;

    // pobranie bieżącego czasu
    time_t rawtime;
    time ( &rawtime );
#if defined(_WINDOWS)
    struct tm timeinfo;
    localtime_s(&timeinfo, &rawtime);
    char timeBuffer[26] = { 0 };
    asctime_s(timeBuffer, &timeinfo);
    output<<"Profiling date:\t"<<timeBuffer;
#else
    struct tm * timeinfo;
    timeinfo = localtime ( &rawtime );
    char buffer [80];
    strftime (buffer,80,"%I:%M%p.",timeinfo);
    output << "Profiling date:\t" << buffer;
#endif


    output<<"Total time:\t"<<tickToSeconds(duration)<<std::endl;
    output<<"Profiler overhead:\t"<<tickToSeconds(overhead)<<std::endl;

    if ( perThreadDumpEnabled ) {
        output<<"Thread\tType\tFunction\tEntries\tTotal time\tAvg time\tPercent (thread)\tPercent\tSignature"<<std::endl;
        ThreadEntryToCSV func = { &output, true };
        forEachThread(func);
    } else {
        // łączymy dane
        State* combinedState = new State();
        zero(*combinedState);
        ThreadCombiner combiner = { combinedState };
        forEachThread(combiner);

        output<<"Type\tFunction\tEntries\tTotal time\tAvg time\tPercent (thread)\tPercent\tSignature"<<std::endl;
        stateToCSV(*combinedState, output, NULL);
        delete combinedState;
    }

    return output.str();
}

void Profiler::addEntry( ProfilerEntry * entry )
{
    // tutaj może wystąpić zjawisko wyścigu, ale nie można zastosować muteksów,
    // gdyż nie ma pewności, które zmienne globalne zostały zainicjalizowane
    // jeśli zajdzie wyścig nie będzie bardzo niebezpieczny; tutaj pewne ryzyko
    // jest podjęte w celu optymalizacji czasowej
    if ( entry->id == ProfilerEntry::InvalidID ) {
        // czemu nie instance.nextID++? ponieważ tutaj jest mniejsze prawdopodobieństwo błędu.
#if defined(_WINDOWS)
        entry->id = InterlockedIncrement(&instance.nextID) - 1;
#else
        entry->id = ++instance.nextID - 1;
#endif
        instance.entires[entry->id] = entry;
    }
}

void Profiler::setThreadEnabled( bool enabled )
{
    tick start = getTick();
    {
        threadID thread = __system::getCurrentThreadID();
        ScopedLock lock(mutex);

        if ( enabled ) {
            enableThread(thread);
        } else {
            disableThread(thread);
        }
#if defined(_WINDOWS)
    }
    InterlockedExchangeAdd64(&overhead, getTick() - start);
#else
        overhead += getTick() - start;
    }
#endif
}

Profiler::State* Profiler::getState()
{
    tick start = getTick();
    Profiler::State* result = NULL;
    {
        threadID thread = __system::getCurrentThreadID();
        ScopedLock lock(mutex);
        if ( threadCount > UTILS_PROFILER_LIST_SIZE ) {
            ThreadMap::iterator found = threadMap.find(thread);
            if (found != threadMap.end()) {
                result = found->second;
            }
        } else {
            int idx = getListIndex(thread, threadList);
            if (idx >= 0) {
                result = threadList[idx].second;
            }
        }
#if defined(_WINDOWS)
    }
    InterlockedExchangeAdd64(&overhead, getTick() - start);
#else
        overhead += getTick() - start;
    }
#endif
    return result;
}

bool Profiler::isThreadEnabled()
{
    return getState() != NULL;
}

void Profiler::enableThread(threadID thread)
{
    // czy trzeba przenieść do mapy?
    if ( threadCount == UTILS_PROFILER_LIST_SIZE ) {
        threadMap.insert(threadList, threadList+UTILS_PROFILER_LIST_SIZE);
        // usuwamy śmieci
        std::fill(threadList, threadList+UTILS_PROFILER_LIST_SIZE, ThreadEntry(0, nullptr));
    }

    // stworzenie nowego stanu
    State *state = new State();
    zero(*state);
    state->start = getTick();

    ThreadEntry entry = ThreadEntry(thread, state);
    if ( threadCount >= UTILS_PROFILER_LIST_SIZE ) {
        // dodajemy do mapy
        std::pair<ThreadMap::iterator, bool> inserted = threadMap.insert(entry);
        UTILS_ASSERT(inserted.second);
    } else {
        // dodajemy do listy
        UTILS_ASSERT(getListIndex(thread, threadList) < 0);
        threadList[threadCount] = entry;
    }
    ++threadCount;
}

void Profiler::disableThread(threadID thread)
{
    if ( threadCount > UTILS_PROFILER_LIST_SIZE ) {
        ThreadMap::iterator found = threadMap.find(thread);
        if ( found == threadMap.end() ) {
            // nie znaleziono
            UTILS_FAIL();
        } else {
            found->second->duration += getTick() - found->second->start;
            found->second->start = -1;
            // kopiujemy do nieaktywnych
            std::pair<ThreadMap::iterator, bool> inserted = inactiveThreads.insert(*found);
            UTILS_ASSERT(inserted.second);
            // usuwamy ze starych
            threadMap.erase(found);
            // czy można wrócić do listy?
            if ( --threadCount == UTILS_PROFILER_LIST_SIZE ) {
                // można, kopiujemy
                std::copy(threadMap.begin(), threadMap.end(), threadList);
                // i zwalniamy
                threadMap.clear();
            }
        }
    } else {
        // usuwamy z listy
        int idx = getListIndex(thread, threadList);
        if ( idx < 0 ) {
            UTILS_FAIL();
        } else {
            // kopiujemy do nieaktywnych
            ThreadEntry* toRemove = threadList + idx;
            toRemove->second->duration += getTick() - toRemove->second->start;
            toRemove->second->start = -1;
            std::pair<ThreadMap::iterator, bool> inserted = inactiveThreads.insert(*toRemove);
            UTILS_ASSERT(inserted.second);
            // usuwamy zbędne
            ThreadEntry* last = std::remove(threadList, threadList+UTILS_PROFILER_LIST_SIZE, *toRemove);
            std::fill(last, threadList+UTILS_PROFILER_LIST_SIZE, ThreadEntry(0, nullptr));
            --threadCount;
        }
    }
}

void Profiler::stateToCSV( const State& state, std::ostream& output, const threadID* id )
{
    for (size_t i = 0; i < UTILS_PROFILER_MAX_ENTRIES; ++i ) {
        // tylko niepuste pomiary!
        const Measurement* measurement = state.measurements + i;
        if ( measurement->entrances ) {
            // bezpieczny dostęp nawet w warunkach wyścigu
            ProfilerEntry* entry = entires[i];
            std::size_t nameLength;
            const char* typeName = entry->getTypeName(nameLength);
            // opcjonalna kolumna z id wątku
            if ( id ) {
                output << *id << '\t';
            }
            output<<
                std::string(typeName, nameLength)<<'\t'<<
                entry->getFunctionName()<<'\t'<<
                measurement->entrances<<'\t'<<
                tickToSeconds(measurement->ticks)<<'\t'<<
                tickToSeconds(measurement->ticks)/measurement->entrances<<'\t'<<
                100.0 * measurement->ticks/state.duration<<'\t'<<
                100.0 * measurement->ticks/duration<<'\t'<<
                entry->signature<<std::endl;
        }
    }
}
//------------------------------------------------------------------------------

ProfilerEntry::ProfilerEntry( const char * signature, const char * function, const char * file, unsigned line ) :
signature(signature), function(function), file(file), line(line), id(InvalidID)
{
    //UTILS_DEBUG_PRINT("%x\t%s", signature, signature);
    // bez akcesora, żeby w debug nie powodować opóźnień
    Profiler::addEntry(this);
}


ProfilerEntry::~ProfilerEntry()
{
}

const char* ProfilerEntry::getTypeName(std::size_t& length) const
{
    // początek bloku argumentów
    int parasynthesisDepth = 0;
    const char * end = signature + strlen(signature);
    while ( --end >= signature ) {
        if ( *end == ')' ) {
            ++parasynthesisDepth;
        } else if ( *end == '(' ) {
            if (--parasynthesisDepth == 0) {
                break;
            }
        }
    }
    UTILS_ASSERT(end>=signature);

    // wyszukanie końca zakresu
    int templateDepth = 0;
    while ( --end >= signature ) {
        if ( *end == '>' ) {
            ++templateDepth;
        } else if ( *end == '<' ) {
            --templateDepth;
        } else if ( *end == ':' && !templateDepth ) {
            // pomijamy kolejny dwukropek
            --end;
            break;
        } else if ( *end == ' ' && !templateDepth) {
            end = signature;
        }
    }

    if ( end < signature ) {
        // nie spacji, czyli funkcja
        length = sizeof(defaultName) - 1;
        return defaultName;
    } else {
        // wszyszukanie początku zakresu
        const char* start = end;
        while ( --start >= signature && *start != ' ') {}
        ++start;
        length = end - start;
        return start;
    }
}

const char* ProfilerEntry::getFunctionName() const
{
#if defined(_WINDOWS)
    const char * end = strrchr(function, ':');
    if ( end ) {
        return end+1;
    } else {
        return function;
    }
#else
    return function;
#endif
}

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////
