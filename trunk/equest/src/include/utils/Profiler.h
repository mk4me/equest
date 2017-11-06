/********************************************************************
	created:  2010/08/05
	created:  5:8:2010   11:27
	filename: Profiler.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__UTILS_PROFILER_H__
#define HEADER_GUARD__UTILS_PROFILER_H__

#include <utils/Config.h>
#include <map>
#include <string>
#include <ostream>
#include <vector>
#include <utils/Macros.h>
#include <utils/System.h>
#include <time.h>

#if defined(_WINDOWS)
#define NOMINMAX
#include <windows.h>
#elif defined(__GNUC__)
#else
#error Unknown compiler.
#endif

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

struct ProfilerEntry;
struct ProfilerEntryUpdater;
struct ProfilerThreadScope;
struct ThreadEntryToCSV;

//! Maksymalna liczba wpisów.
#define UTILS_PROFILER_MAX_ENTRIES 1000
//! Maksymalna liczba aktywnych wątków możliwa do identyfikacji bez rozgałęzień.
#define UTILS_PROFILER_LIST_SIZE 4
//! Domyślny plik docelowy.
#define UTILS_PROFILER_DEFAULT_DUMP_FILE "ProfilerLog.csv"

/**
 *  Profiler wielowątkowy. Działa z minimalnymi narzutami (kilka promili) nawet
 *  w trybie debug.
 *  Żeby uaktywnić profilowanie wątku, należy albo stworzyć lokalnie instancję
 *  typu ThreadScope, albo posłużyć się metodą Profiler::setThreadEnabled
 *  przy rozpoczynaniu i kończeniu wątku.
 *  Pomiary odbywają się przy użyciu statycznych wpisów identyfikujących funkcję
 *  (ProfilerEntry) oraz ich aktualizatory, tworzone lokalnie ProfilerEntryUpdater. 
 *  Gdy zasięg nazw, dla którego stworzono instancję ProfilerEntryUpdater się zakończy,
 *  destruktur na podstawie wskaźnika do ProfilerEntry będzie w stanie zaktualizować
 *  dane pomiarowe.
 *
 *  Problemem okazało się przeprowadzanie pomiarów środowisku wielowątkowym. Ostatecznie
 *  każdy wątek dysponuje swoim zestawem pomiarów, dzięki czemu kod synchronizujący
 *  ograniczony jest do minimum. W momencie wykonywania zrzutu do pliku dane są konsolidowane.
 *  Dodatkowo jeżeli liczba aktywnych wątków jest niska (<= UTILS_PROFILER_LIST_SIZE),
 *  wyznaczenie zestawu danych na podsatwie id odbywa się bez instrukcji rozgałęzień. Jeżeli
 *  liczba aktywnych wątków urośnie ponad tę liczbę, wówczas używany jest kontener std::map,
 *  w którym overhead związany z obsługą drzewa zwiększa narzut czasowy Profilera, zwłaszcza
 *  w trybie Debug.
 */
class Profiler
{
    friend struct ProfilerEntry;
    friend struct ProfilerEntryUpdater;
    friend struct ThreadScope;
    friend struct ThreadEntryToCSV;

public:
    //! Czas w wysokiej rozdzielczości.
    typedef __system::Tick tick;
    //! Typ id wątku.
    typedef __system::ThreadID threadID;

    //! Typ włączający profilowanie bieżącego wątku w bieżącej przestrzeni nazw.
    struct ThreadScope
    {
        ThreadScope() { Profiler::instance.setThreadEnabled(true);}
        ~ThreadScope() { Profiler::instance.setThreadEnabled(false); }
    };

    //! Pomiar.
    struct Measurement
    {
        //! Liczba tyknięć.
        Profiler::tick ticks;
        //! Liczba wejść.
        unsigned entrances;
    };

    //! Stan profilingu.
    struct State
    {
        //! Bieżący wpis.
        ProfilerEntry* currentEntry;
        //! Czas rozpoczęcia.
        tick start;
        //! Łączny czas trwania.
        tick duration;
        //! Pomiary.
        Measurement measurements[UTILS_PROFILER_MAX_ENTRIES];
    };

    //! Skojarzenie wątku ze stanem.
    typedef std::pair<threadID, State*> ThreadEntry;
    //! Dane o wątkach w postaci mapy.
    typedef std::map<ThreadEntry::first_type, ThreadEntry::second_type> ThreadMap;

private:
    //! Instancja profilera.
    static Profiler instance;
    //! ID następnego wpisu.
    volatile long nextID;
    //! Czy jest włączony?
    bool enabled;
    //! Czas trwania programu.
    tick duration;
    //! Czas startu programu.
    tick start;
    //! Czas ostatniego włączenia.
    tick enabledStart;
    //! Łączny czas włączenia.
    tick enabledDuration;
    //! Czas zmarnotrawiony na logikę profilera.
    volatile tick overhead;
    //! Wpisy.
    ProfilerEntry* entires[UTILS_PROFILER_MAX_ENTRIES];
    //! Lista aktywnych wątków.
    ThreadEntry threadList[UTILS_PROFILER_LIST_SIZE];
    //! Mapa aktywnych wątków.
    ThreadMap threadMap;
    //! Liczba aktywnych wątków.
    int threadCount;
    //! Niekatywne już wątki.
    ThreadMap inactiveThreads;

    //! Plik do automatycznego zapisu loga.
    std::string autoDumpFile;
    //! Czy w logu dane pomiarowe mają być rozbite na poszczegolne wątki?
    bool perThreadDumpEnabled;

public:
    //! 
    Profiler();
    //! 
    ~Profiler();

public:

    //! \return Referencja do instancji.
    static Profiler& getInstance()
    {
        return instance;
    }


    //! Czy profiler jest włączony?
    //! \param enabled
    static void setEnabled(bool enabled);
    //! Czy profilowanie dla wątku jest włączone?
    void setThreadEnabled(bool enabled);
    //! Czy profilowanie dla wątku jest włączone?
    bool isThreadEnabled();

    //! \return Ścieżka do automatycznego zapisu.
    static const std::string& getAutoDumpFile() 
    { 
        return instance.autoDumpFile;
    }
    //! \param autoDumpFile Ścieżka do automatycznego zapisu.
    static void setAutoDumpFile(const std::string& autoDumpFile) 
    { 
        instance.autoDumpFile = autoDumpFile; 
    }

    //! \return Tyknięcia zegara o wysokiej częstotliwości.
    inline static tick getTick()
    {
        return __system::getTick();
    }
    //! Konwersja tyknięcia do sekund.
    inline static double tickToSeconds(tick value)
    {
        return __system::tickToSeconds(value);
    }

    //! Zapisuje log pod zadany strumień.
    static void dump(std::ostream& output);
    //! \return Log.
    static std::string dump();

    //! \return
    static bool isPerThreadDumpEnabled()
    { 
        return instance.perThreadDumpEnabled;
    }
    //! \param perThreadDumpEnabled
    static void setPerThreadDumpEnabled(bool perThreadDumpEnabled) 
    { 
        instance.perThreadDumpEnabled = perThreadDumpEnabled; 
    }

private:
    //! Dodaje wpis do listy.
    static void addEntry(ProfilerEntry * entry);
    
    //! \return Stan dla bieżącego wątku bądź NULL, jeżeli jest wyłączony.
    State* getState();

    //! Włącza profilowanie dla wątku.
    void enableThread(threadID thread);
    //! Wyłącza profileowanie dla wątku.
    void disableThread(threadID thread);

    //! Zrzuca dane do łańcucha.
    //! \param output
    std::string createCSV();
    //! Funkcja zrzucająca pewien przedział elementów ThreadEntry

    template <class FwdIter>
    void scopeToCSV(std::ostringstream& str, FwdIter first, FwdIter last);

    //! Funkcja wykonująca jakąś czynność na wszystkich stanach wszystkich wątków.
    template <class Func>
    void forEachThread(Func func);

    //!
    void stateToCSV(const State& state, std::ostream& output, const threadID* id);
};




/**
 *	Wpis profilera. Typ przeznaczany do tworzenia jako zmienne statyczne w funckjach. Rodzi
 *  to problem wyścigu w warunkach wielowątkowości, ale ponieważ typ nie konstruuje żadnych
 *  podobiektów ani ich nie kasuje - nie jest ważne, ile razy będzie wywołany
 *  konstruktor / destruktor.
 */
struct ProfilerEntry
{
    enum {
        InvalidID = -1
    };

    //! ID wpisu.
    long id;
    //! Sygnatura funkcji.
    const char* signature;
    //! Nazwa funkcji.
    const char* function;
    //! Plik.
    const char* file;
    //! Linia pliku.
    unsigned line;

    //! 
    //! \param signature
    //! \param function
    //! \param file
    //! \param line
    ProfilerEntry(const char * signature, const char * function, const char * file, unsigned line);

    //!
    ~ProfilerEntry();

    //! \param length Długość nazwy.
    //! \return Nazwa typu. Nie jest zakończona zerem!
    const char* getTypeName(std::size_t& length) const;
    //! \return Typ.
    const char* getFunctionName() const;
};


/**
 *	Struktura aktualizująca dane profilera.
 */
struct ProfilerEntryUpdater
{
    //! Poprzedni wpis.
    ProfilerEntry* prevEntry;
    //! Pomiary.
    Profiler::State* state;
    //! Pomiary.
    Profiler::Measurement* measurement;
    //! Poprzedni czas wpisu minus tick wejścia. Czemu po prostu nie dodajemy delty do czasu? Wywołania rekurencyjne!
    Profiler::tick entryTickMinusPrev;
    //! Czy włączony?
    bool enabled;

    //! 
    //! \param entry
    ProfilerEntryUpdater(ProfilerEntry * entry)
    {
        if ( (state = Profiler::instance.getState()) != 0 ) {
            // zamiana bieżącego wpisu
            prevEntry = state->currentEntry;
            state->currentEntry = entry;
            measurement = state->measurements + entry->id;
            // aktualizacja i zapamiętanie czasu wejścia
            if ( (enabled = Profiler::instance.enabled) != false ) {
                ++measurement->entrances;
                entryTickMinusPrev = Profiler::getTick() - measurement->ticks;
            }
        } 
    }

    //! 
    ~ProfilerEntryUpdater()
    {
        if ( state ) {
            if ( enabled ) {
                measurement->ticks = Profiler::getTick() - entryTickMinusPrev;
            }
            state->currentEntry = prevEntry;
        }
    }
};

//#define DISABLE_PROFILER
//#define UTILS_RELEASE_PROFILER
#if !defined(DISABLE_PROFILER) && (defined(_DEBUG) || defined(DEBUG) || defined(UTILS_RELEASE_PROFILER))

//! Makro, które należy wstawić w głównej funkcji wątku, aby umożliwić zbieranie danych
//! pomiarowych z nim związanych
#define UTILS_PROFILER_THREAD_SCOPE utils::Profiler::ThreadScope __profiler_thread_scope;

//! Makro służące do pomiaru czasu wykonania funkcji. Powinno być umieszczane
//! w pierwszej linii. NIE należy umieszczać w funkcjach inline (gdyż przestaną
//! wtedy być inline :))
#if defined(_MSC_VER)
#define UTILS_PROFILER_ENTRY \
    static utils::ProfilerEntry __profiler_entry(__FUNCSIG__, __FUNCTION__, __FILE__, __LINE__);\
    utils::ProfilerEntryUpdater __profiler_updater(&__profiler_entry)
#elif defined(__GNUC__)
#define UTILS_PROFILER_ENTRY \
    static utils::ProfilerEntry __profiler_entry(__PRETTY_FUNCTION__, __FUNCTION__, __FILE__, __LINE__);\
    utils::ProfilerEntryUpdater __profiler_updater(&__profiler_entry)
#endif

//! Makro służące do pomiaru części funkcji. Nazwa może składać się tylko ze znaków
//! alfanumerycznych.
//! \param name Nazwa zakresu
#if defined(_MSC_VER)
#define UTILS_PROFILER_SCOPE(name) \
    static utils::ProfilerEntry __scope_entry_##name(__FUNCSIG__, __FUNCTION__ "{" STRINGIZE(name) "}", __FILE__, __LINE__); \
    utils::ProfilerEntryUpdater __scope_updater_##name(&__scope_entry_##name)
#elif defined(__GNUC__)
// nie da się robić konkatenacji __FUNCTION__ i łańcuchów...
#define UTILS_PROFILER_SCOPE(name) \
    static utils::ProfilerEntry __scope_entry_##name(__PRETTY_FUNCTION__, "customscope::" STRINGIZE(name), __FILE__, __LINE__); \
    utils::ProfilerEntryUpdater __scope_updater_##name(&__scope_entry_##name)
#endif

#else

#define UTILS_PROFILER_THREAD_SCOPE
#define UTILS_PROFILER_ENTRY
#define UTILS_PROFILER_SCOPE(name)

#endif


//static utils::ProfilerEntry CONCATENATE(__profiler_entry, __LINE__)(__FUNCSIG__, __FUNCTION__ "!" name, __FILE__, __LINE__); utils::ProfilerEntryUpdater CONCATENATE(__profiler_updater, __LINE__)(&CONCATENATE(__profiler_entry, __LINE__));

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif  // HEADER_GUARD__PROFILER_H__
