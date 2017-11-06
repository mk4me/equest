/**
@author Marek Daniluk
@brief Klasa Filesystem z podstawowymi operacjami na plikach. Do uzupełnienia.
*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <list>
#include <set>
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>


////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

class Filesystem
{
public:
	//! Typ opisujący element katalogu
	using DirectoryEntry = boost::filesystem::directory_entry;
	//! Typ iteratora zawartości katalogów
	using Iterator = boost::filesystem::directory_iterator;
	//! Typ iteratora zawartości katalogów i podkatalogów
	using RecursiveIterator = boost::filesystem::recursive_directory_iterator;
    //! Typy do obsługi ścieżek
	using Path = boost::filesystem::path;
	//! Typ opisujący rozmiar danych
	using size_t = boost::uintmax_t;
	// Zbiór ścieżek
	using PathsList = std::list<Path>;
	// Zbiór plików
	using FilesList = PathsList;
	// Zbiór ścieżek
	using PathsSet = std::set<Path>;
	// Zbiór plików
	using FilesSet = PathsSet;

public:
	/*
	Tworzy foldery z podanej ścieżki.
	@param path ścieżka z której mają być utworzone foldery
	*/	
    static void createDirectory(const Path& path);
	/*
	Usuwa folder z podanej ścieżki wraz z zawatością.
	@param path ścieżka do folderu który zostanie usunięty
	*/	
    static void deleteDirectory(const Path& path);
	/*
	Usuwa pojedynczy plik.
	@param path ścieżka do pliku który zostanie usunięty
	*/	
    static void deleteFile(const Path& path);
	/*
	Przesuwa plik lub folder wraz z zawartością o podanej ścieżce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik się nadpisze, w przypadku folderów nie (trzeba to naprawić).
	@param pathOldFile ścieżka do pliku lub folderu który ma być przesunięty
	@param pathNewFile ścieżka docelowa przesunięcia
	*/	
    static void move(const Path& pathOld, const Path& pathNew);

	/*
	Kopiuje plik lub folder wraz z zawartością o podanej ścieżce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik się nadpisze, w przypadku folderów nie (trzeba to naprawić).
	@param pathOldFile ścieżka do pliku lub folderu który ma być kopiowany
	@param pathNewFile ścieżka docelowa kopiowania
	*/    
    static void copy(const Path& pathOld, const Path& pathNew);

    /*
	Listuje wszystkie pliki danego folderu spełniające kryterium maski.
	@param path ścieżka do folderu który ma być przeszukany
	@param recursive czy szukać plików w podfolderach
	@return lista wszystkich plików wraz ze ścieżką
	*/    
    static PathsList listFiles(const Path& path, bool recursive = false);
	/*
	Listuje wszystkie pliki danego folderu spełniające kryterium maski.
	@param path ścieżka do folderu który ma być przeszukany
	@param recursive czy szukać plików w podfolderach
	@param mask rozszerzenie pliku, np. ".avi"
	@return lista wszystkich plików wraz ze ścieżką
	*/	
	static PathsList listFiles(const Path& path, bool recursive, const std::string& mask);
	/*
	Listuje wszystkie pliki danego folderu spełniające kryterium masek.
	@param path ścieżka do folderu który ma być przeszukany
	@param recursive czy szukać plików w podfolderach
	@param mask wektor z rozszerzeniami szukanych plikow
	@return lista wszystkich plików wraz ze ścieżką
	*/	
	static PathsList listFiles(const Path& path, bool recursive, const std::vector<std::string>& masks);
	/*
	Listuje wszystkie pliki danego folderu spełniające kryterium przekazywanego filtru.
	@tparam Path Typ opisujący ścieżkę
	@tparam Predicate Obiekt opisujący czy dana wartość ma być usunięta
	@param path ścieżka do folderu który ma być przeszukany
	@param recursive czy szukać plików w podfolderach
	@param filter funkcja testująca przeszukiwane pliki
	@return lista wszystkich plików wraz ze ścieżką
	*/
	template<typename Path, typename Predicate>
	static PathsList listFilteredFiles(const Path& path, bool recursive, Predicate filter)
	{
		auto ret(listFiles(path, recursive));
		auto eIT = std::remove_if(ret.begin(), ret.end(), [&](const Path& path) { return !filter(path); });
		ret.erase(eIT, ret.end());
		return ret;
	}	
	/*
	Listuje wszystkie podfoldery danego folderu.
	@param path ścieżka do folderu który ma być przeszukany
	@return lista wszystkich podfolderów wraz ze ścieżką
	*/	
	static PathsList listSubdirectories(const Path& path);
    /*
	Sprawdza czy podana scieżka wskazuje zwykły plik
	@param path ścieżka do sprawdzenia
	@return true jeśli ścieżka wskazuje zwykły plik
	*/    
    static bool isRegularFile(const Path & path);
    /*
	Sprawdza czy podana scieżka wskazuje link symboliczny
	@param path ścieżka do sprawdzenia
	@return true jeśli ścieżka wskazuje link symboliczny
	*/   
    static bool isSymbolicLink(const Path & path);
    /*
	Sprawdza czy podana scieżka wskazuje katalog
	@param path ścieżka do sprawdzenia
	@return true jeśli ścieżka wskazuje katalog
	*/    
    static bool isDirectory(const Path & path);
    /*
	Sprawdza czy podana scieżka istnieje
	@param path ścieżka do sprawdzenia
	@return true jeśli ścieżka istnieje
	*/    
    static bool pathExists(const Path & path);
	/*
	Wyciąga wolną przestrzeń w bajtach do zapisu dla aktualnego użytkownika
	@param path ścieżka partycji
	@return wolna przestrzeń w bajtach dla danego użytkownika
	*/	
	static size_t availableSpace(const Path & path);
	/*
	Wyciąga pojemność partycji
	@param path ścieżka partycji
	@return pojemnośc dysku w bajtach
	*/	
	static size_t capacity(const Path & path);
	/*
	Wyciąga całkowitą wolną przestrzeń do zapisu dla partycji
	@param path ścieżka partycji
	@return Całkowita wolna przestrzeń partycji do zapisu w bajtach
	*/	
	static size_t freeSpace(const Path & path);
	/*
	Wyciąga rozmiar pliku/katalogu wskazanego ścieżką
	@param path ścieżka
	@return Całkowity rozmiar wskazany przez ścieżke
	*/	
	static size_t size(const Path & path);
	//! \return Nazwa pliku tymczasowego - uwzględnia czas i wątek
	static std::string temporaryFileName();
	//! \return Rozszerzenie pliku tymczasowego
	static std::string temporaryFileExtension();
	//! \return Nazwa + rozszerzenie pliku tymczasowego - uwzględnia czas i wątek
	static Path temporaryFile();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif
