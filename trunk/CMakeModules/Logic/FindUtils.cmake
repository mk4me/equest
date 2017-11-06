###############################################################################
# Zbiór makr pomagaj¹cych szukaæ bibliotek w ramach ustalonej struktury.
#
# Struktura bibliotek wygl¹da nastepuj¹co:
# root/
#		include/
#				libraryA/
#						 LibraryAHeaders
#				libraryB/
#						 LibraryBHeaders
#		lib/
#			platform/	[win32 | linux32 | win64 | linux64] aktualnie wspierane s¹ pierwsze 2
#					 build/ [debug | release]
#							LibraryA/
#									  LibraryAArtifacts [libs, dlls, so, a, plugins, exe]
#
# W taki sposób generowane s¹ biblioteki na CI i dla takiej struktury mamy findery bibliotek zewnêtrznych
# Dla takiej struktury generujemy równie¿ findery naszych bibliotek oraz mechanizm instalacji
#
###############################################################################
# Zmienne jakie bior¹ udzia³ w wyszukiwaniu bibliotek:
#
# Wyjciowe:
# _HEADERS_INCLUDE_DIR - g³ówny katalog z includami dla biblioteki dla której przed chwil¹ wo³ano _FIND_INIT2, w przeciwnym wypadku nie istnieje
# ${library}_INCLUDE_DIR - g³ówny katalog z includami dla danej biblioteki, patrz _INCLUDE_DIR
# ${library}_ADDITIONAL_INCLUDE_DIRS - dodatkowe katalogi z includami dla danej biblioteki.
# 										Mog¹ wynikaæ z zale¿noci od innych bibliotek lub
#										realizacji danej biblioteki
# LIBRARY_${library}_FOUND - informacja czy znaleziono bibliotekê
# LIBRARY_${library}_LIBRARIES - zbiór linkowanych statycznych (patrz uwaga poni¿ej) bibliotek na potrzeby zadanej biblioteki z podzia³em na wersjê debug, release i ogólne
# LIBRARY_${library}_RELEASE_LIBS - zbiór zmiennych przechowuj¹cych cie¿ki do linkowanych bibliotek w wersji release
# LIBRARY_${library}_RELEASE_DLLS - zbiór zmiennych przechowuj¹cych cie¿ki do dynamicznych bibliotek w wersji release
# LIBRARY_${library}_RELEASE_DIRECTORIES - zbiór zmiennych przechowuj¹cych cie¿ki do katalogów (np. pluginów, innych resources) w wersji release
# LIBRARY_${library}_RELEASE_EXECUTABLES - zbiór zmiennych przechowuj¹cych cie¿ki do plików wykonywalnych w wersji release
# LIBRARY_${library}_DEBUG_LIBS - zbiór zmiennych przechowuj¹cych cie¿ki do linkowanych bibliotek w wersji debug
# LIBRARY_${library}_DEBUG_DLLS - zbiór zmiennych przechowuj¹cych cie¿ki do dynamicznych bibliotek w wersji debug
# LIBRARY_${library}_DEBUG_DIRECTORIES - zbiór zmiennych przechowuj¹cych cie¿ki do katalogów (np. pluginów, innych resources) w wersji debug
# LIBRARY_${library}_DEBUG_EXECUTABLES - zbiór zmiennych przechowuj¹cych cie¿ki do plików wykonywalnych w wersji debug
# LIBRARY_${library}_DEPENDENCIES - lista jawnych zależności od innych bibliotek
# LIBRARY_${library}_PREREQUISITES - lista wymaganych bibliotek w runtime (przykryte implementacją)
# LIBRARY_${library}_DEBUG_TRANSLATIONS - lista plików tłumaczeń dla wersji debug
# LIBRARY_${library}_RELEASE_TRANSLATIONS - lista plików tłumaczeń dla wersji release
###############################################################################
#
#	Wa¿na informacja na temat traktowania bibliotek - pod linux biblioteki dynamiczne
#	s¹ traktowane jak statyczne w przypadku kompilacji - musimy je linkowaæ
#	aby do³¹czyæ odpowiednie symbole. Tam nie ma podzia³u tak jak na windows na lib i dll!
# 	Niemniej w skryptach nadal wystêpuj¹ jako biblioteki dynamiczne, tylko jawnie dla linux
#	s¹ do³anczane na potrzeby linkowania do LIBRARY_${library}_LIBRARIES
#
###############################################################################
#
# Wejciowe:
# ${library}_LIBRARY_DIR_DEBUG - katalog z artefaktami w wersji debug
# ${library}_LIBRARY_DIR_RELEASE - katalog z artefaktami w wersji release
#
###############################################################################
#
# Modyfikowane zmienne globalne CMAKE:
# FIND_DEBUG_SUFFIXES - suffix dla bibliotek w wersji debug, u nas zawsze d na koñcu nazwy artefaktu!
# CMAKE_FIND_LIBRARY_SUFFIXES - lista rozszerzeñ dla poszukiwanych bibliotek - sami ni¹ sterujemy na potrzeby szukania 
#								bibliotek statycznych i dynamicznych na ró¿nych platformach. Zawsze przywracamy jej oryginaln¹ wartoæ
#
###############################################################################
#
# Mechanizm wyszukiwania bibliotek:
# Wszystkie makra wyszukuj¹ce zawarte pomiêdzy FIND_INIT i FIND_FINISH modyfikuj¹ wspólne zmienne informuj¹c przy tym
# czy dany element uda³o siê znaleæ czy nie. W ten sposób w FIND_FINISH na bazie takiego iloczynu mo¿na stwierdziæ
# czy dan¹ bibliotekê uda³o siê znaleæ poprawnie w ca³oci czy nie i odpowiednio ustawiæ zmienn¹ LIBRARY_${library}_FOUND.
#
# TODO:
# nale¿y dodaæ mechanizm opcjonalnego wyszukiwania elementów, które w przypadku nieznalezienia nie bêd¹ powodowa³y oznaczenia
# biblioteki jako nieznalezionej
#
###############################################################################
#
# Mechanizm obs³ugi zale¿noci bibliotek miêdzy sob¹:
# Czêsto pomiêdzy bibliotekami wystepuj¹ dodatkowe zale¿noci jawne (includy + liby i dllki),
# oraz niejawne gdzie wymagane s¹ tylko wersje dynamiczne innych bibliotek (s¹ one ca³kowicie przykryte
# i ich nag³ówki ani libki statyczne nie sa wymagane). Dlatego biblioteki zale¿ne dzielimy na:
# DEPENDENCIES - jawne zale¿noci mog¹ce pojawiaæ siê w includach, wymagaj¹ wiêc znalezienia i do³¹czenia do
#				 zmiennej ${library}__ADDITIONAL_INCLUDE_DIR includów z bibliotek zale¿nych, do zmiennej
#				 LIBRARY_${library}_LIBRARIES zaleznych bibliotek statycznych
# PREREQUISITES - niejawne zaleznoci wymagaj¹ce dostarczenia jedynie wersji bibliotek dynamicznych naszej zaleznoci
#
###############################################################################
#
# Mechanizm realizacji zaleznoci pomiêdzy bibliotekami dzia³a dwu-etapowo:
# 1. W momencie wyszukiwania biblioteki sprawdzamy czy jej dodatkowe zale¿noci by³y ju¿ szukane
#    i odpowiednio modyfikujemy informacjê o tym czy bibliotekê znaleziono czy nie
# 2. Jeli w tym momencie zadane biblioteki nie by³y wyszukiwane zostaj¹ zapamiêtane do ponownego wyszukiwania w póniejszym czasie
#    (byæ mo¿e kto inny wci¹gnie je jawnie)
#
# W drugim przebiegu s¹ szukane te biblioteki któe by³y zg³oszone jako zale¿noci innych.
# Jeli jeszcze do tej pory nie by³y szukane s¹ szukane w tym momencie. Jeli maj¹ dodatkowe zale¿noci
# s¹ one dopisywane wg schematu ju¿ opisanego lub jeli nie by³y jeszcze szukane odk³adamy je do póniejszego szukania
# Procedura ta jest powtarzana tak d³ugo a¿ dla wszystkich bibliotek wyczerpiemy szukanie ich zalezoci.
#
###############################################################################

# inicjalizacja logowania wiadomosci modulu find
INIT_VERBOSE_OPTION(FIND "Print find verbose info?")	

###############################################################################
# Inicjuje ścieżki wyszukiwania dla danego roota
macro(_SETUP_FIND_ROOT rootPath)

	set(FIND_LIBRARIES_ROOT_DEBUG "${rootPath}/lib/${SOLUTION_LIBRARIES_PLATFORM}/debug" CACHE INTERNAL "")
	set(FIND_LIBRARIES_ROOT_RELEASE "${rootPath}/lib/${SOLUTION_LIBRARIES_PLATFORM}/release" CACHE INTERNAL "")
	set(FIND_LIBRARIES_INCLUDE_ROOT "${rootPath}/include" CACHE INTERNAL "")
	
	FIND_NOTIFY("rootPath" "Setup find root: include->${FIND_LIBRARIES_INCLUDE_ROOT}; libs->${rootPath}/lib/${SOLUTION_LIBRARIES_PLATFORM}")

endmacro(_SETUP_FIND_ROOT)

###############################################################################
# Inicjuje proces wyszukiwania biblioteki.
macro(_FIND_INIT2 library fullIncludeDir includeDirRoot libraryDirDebug libraryDirRelease skipHeaderCheck)
	
	set(_HEADERS_INCLUDE_DIR)
	# g³ówne cie¿ki
	if (${skipHeaderCheck})
		set(${library}_INCLUDE_DIR "${includeDirRoot}" CACHE PATH "Location of ${library} headers." FORCE)
	elseif (NOT FIND_DISABLE_INCLUDES AND EXISTS "${includeDirRoot}")		
		get_filename_component(_abs "${includeDirRoot}" ABSOLUTE)
		file(GLOB_RECURSE _headerFiles "${_abs}/*.*" "${_abs}/*.h" "${_abs}/*.hh" "${_abs}/*.hpp")
		list(LENGTH _headerFiles _headerFilesLength)
		
		if(_headerFilesLength EQUAL 0)
			# szukamy czegokolwiek - prawdopodobnie nagłówki są bez rozszerzenia
			file(GLOB_RECURSE _headerFiles "${_abs}/*")
			list(LENGTH _headerFiles _headerFilesLength)
		endif()
		
		if(_headerFilesLength GREATER 0)
			set(${library}_INCLUDE_DIR "${includeDirRoot}" CACHE PATH "Location of ${library} headers." FORCE)
			set(_HEADERS_INCLUDE_DIR "${fullIncludeDir}")
		endif()
	endif()
	set(${library}_LIBRARY_DIR_DEBUG "${libraryDirDebug}" CACHE PATH "Location of ${library} debug libraries." FORCE)
	set(${library}_LIBRARY_DIR_RELEASE "${libraryDirRelease}" CACHE PATH "Location of ${library} libraries." FORCE)
	# lokalizacja bibliotek dla trybu debug
	set (FIND_DIR_DEBUG ${${library}_LIBRARY_DIR_DEBUG})	
	# lokalizacja bibliotek
	set (FIND_DIR_RELEASE ${${library}_LIBRARY_DIR_RELEASE})
	# mo¿liwy przyrostek dla bibliotek w wersji debug
	set (FIND_DEBUG_SUFFIXES "d")

	# wyzerowanie zmiennych logicznych
	set (FIND_RESULTS_LOGICAL_OR 0)
	set (FIND_RESULTS_LOGICAL_AND 1)

	FIND_NOTIFY(${library} "FIND_INIT: include: ${${library}_INCLUDE_DIR}; debug: ${${library}_LIBRARY_DIR_DEBUG}; release: ${${library}_LIBRARY_DIR_RELEASE}")
	
	# wyzerowanie listy plików
	# release
	set(_ALL_LIBS)
	# lista libów
	set(_ALL_RELEASE_LIBS)
	# lista dllek
	set(_ALL_RELEASE_DLLS)
	# lista dodatkowych katalogów - np. pluginy dla qt czy osg
	set(_ALL_RELEASE_DIRECTORIES)
	# lista aplikacji
	set(_ALL_RELEASE_EXECUTABLES)
	# lista plików tłumaczeń dla wersji release
	set(_LIBRARY_RELEASE_TRANSLATIONS)
	#debug
	# lista libów
	set(_ALL_DEBUG_LIBS)
	# lista dllek
	set(_ALL_DEBUG_DLLS)
	# lista dodatkowych katalogów - np. pluginy dla qt czy osg
	set(_ALL_DEBUG_DIRECTORIES)
	# lista aplikacji
	set(_ALL_DEBUG_EXECUTABLES)
	# lista plików tłumaczeń
	set(_LIBRARY_DEBUG_TRANSLATIONS)
	
endmacro(_FIND_INIT2)

###############################################################################
# Inicjuje proces wyszukiwania biblioteki.
macro(FIND_INIT2 library dirName includeDir libraryDirDebug libraryDirRelease)

	_FIND_INIT2(${library} "${FIND_LIBRARIES_INCLUDE_ROOT}/${dirName}" "${FIND_LIBRARIES_INCLUDE_ROOT}/${includeDir}" "${FIND_LIBRARIES_ROOT_DEBUG}/${libraryDirDebug}" "${FIND_LIBRARIES_ROOT_RELEASE}/${libraryDirRelease}" 0)

endmacro(FIND_INIT2)

###############################################################################
# Inicjuje proces wyszukiwania biblioteki.
macro(FIND_INIT_HEADER library dirName includeDir libraryDirDebug libraryDirRelease)
	_FIND_INIT2(${library} "${FIND_LIBRARIES_INCLUDE_ROOT}/${dirName}" "${FIND_LIBRARIES_INCLUDE_ROOT}/${includeDir}" "${FIND_LIBRARIES_ROOT_DEBUG}/${libraryDirDebug}" "${FIND_LIBRARIES_ROOT_RELEASE}/${libraryDirRelease}" 1)
	if(NOT EXISTS "${${library}_INCLUDE_DIR}")
		set(FIND_RESULTS_LOGICAL_AND 0)
	endif()
endmacro(FIND_INIT_HEADER)

###############################################################################
# Inicjuje proces wyszukiwania biblioteki.
macro(FIND_INIT library dirName)
	FIND_INIT2(${library} "${dirName}/${dirName}" ${dirName} ${dirName} ${dirName})
endmacro(FIND_INIT)

###############################################################################
# Koñczy proces wyszukiwania biblioteki.
macro(FIND_FINISH library)	

	set(LIBRARY_${library}_FOUND ${FIND_RESULTS_LOGICAL_AND})
	# skopiowanie
	set (FIND_DISABLE_INCLUDES OFF)
	FIND_NOTIFY(${library} "FIND_FINISH: found libraries ${FIND_RESULTS_LOGICAL_AND}")
	
	set(LIBRARY_${library}_LIBRARIES ${_ALL_LIBS})
	set(LIBRARY_${library}_RELEASE_LIBS ${_ALL_RELEASE_LIBS})
	set(LIBRARY_${library}_RELEASE_DLLS ${_ALL_RELEASE_DLLS})
	set(LIBRARY_${library}_RELEASE_DIRECTORIES ${_ALL_RELEASE_DIRECTORIES})
	set(LIBRARY_${library}_RELEASE_EXECUTABLES ${_ALL_RELEASE_EXECUTABLES})
	set(LIBRARY_${library}_RELEASE_TRANSLATIONS ${_LIBRARY_RELEASE_TRANSLATIONS})
	set(LIBRARY_${library}_DEBUG_LIBS ${_ALL_DEBUG_LIBS})
	set(LIBRARY_${library}_DEBUG_DLLS ${_ALL_DEBUG_DLLS})
	set(LIBRARY_${library}_DEBUG_DIRECTORIES ${_ALL_DEBUG_DIRECTORIES})
	set(LIBRARY_${library}_DEBUG_EXECUTABLES ${_ALL_DEBUG_EXECUTABLES})
	set(LIBRARY_${library}_DEBUG_TRANSLATIONS ${_LIBRARY_DEBUG_TRANSLATIONS})
	
endmacro(FIND_FINISH)

###############################################################################

# Makro wyszukuje bibliotek statycznych lub plików lib dla wspó³dzielonych bibliotek (windows).
# Zak³ada, ¿e istniej¹ dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szukaæ bibliotek w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szukaæ bibliotek w wersji release
# releaseOutputSufix - sufix dla zmiennej trzymaj¹cej cie¿kê do znalezionej biblioteki w wersji release
# Wygl¹da nastepuj¹co: ${variable}_${releaseOutputSufix}
# debugOutputSufix - patrz opis wy¿ej dla releaseOutputSufix
# Wyjanienie: extension u¿ywany jest w sytuacji, gdy
# CMake nie potrafi wyszukaæ biblioteki bez rozszerzenia (np. biblioteki stayczne na Unixie)
# w 99% przypadków jednak nic nie zawiera i w tych wypadkach rozszerzenia brane s¹ z suffixes.
# Rezultaty:
# 	${variable}_${debugOutputSufix} lokalizacja biblioteki w wersji debug
#   ${variable}_${releaseOutputSufix} lokazliacja biblioteki w wersji release
macro(FIND_LIB_FILES_PATTERN variable releasePattern debugPattern releaseOutputSufix debugOutputSufix msgHeader extensions)

	set(suffixes_copy ${CMAKE_FIND_LIBRARY_SUFFIXES})
	string(LENGTH "${extensions}" length)
	if (NOT length EQUAL 0)
		set(CMAKE_FIND_LIBRARY_SUFFIXES ${extensions})
	endif()

	if (FIND_DISABLE_CUSTOM_DIRECTORY)
		FIND_NOTIFY(${variable} "${msgHeader}: only system directories!")
	endif()

	# wyszukanie wersji debug
	set(_lib_names)
	CREATE_NAMES_LIST("<?,lib>${debugPattern}${extensions}" _lib_names)	
	
	FIND_NOTIFY(${variable} "${msgHeader}: debug pattern ${debugPattern} unrolled to ${_lib_names}")
	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		# szukamy wersji release, najpierw w wyznaczonym miejscu
		find_library(${variable}_${debugOutputSufix}
			NAMES ${_lib_names}
			PATHS ${FIND_DIR_DEBUG}
			DOC "Location of debug version of ${_lib_names}"
			NO_DEFAULT_PATH
		)
		
	endif()
	
	# potem w ca³ym systemie
	find_library(${variable}_${debugOutputSufix}
		NAMES ${_lib_names}
		DOC "Location of debug version of ${_lib_names}"
	)

	# wyszukanie wersji release
	set(_lib_names)
	CREATE_NAMES_LIST("<?,lib>${releasePattern}${extensions}" _lib_names)

	FIND_NOTIFY(${variable} "${msgHeader}: release pattern ${releasePattern} unrolled to ${_lib_names}")
	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		# szukamy wersji release, najpierw w wyznaczonym miejscu
		find_library(${variable}_${releaseOutputSufix}
			NAMES ${_lib_names}
			PATHS ${FIND_DIR_RELEASE}
			DOC "Location of release version of ${_lib_names}"
			NO_DEFAULT_PATH
		)
	endif()
	# potem w ca³ym systemie
	find_library(${variable}_${releaseOutputSufix}
		NAMES ${_lib_names}
		DOC "Location of release version of ${_lib_names}"
	)

	# przywracamy sufiksy
	set(CMAKE_FIND_LIBRARY_SUFFIXES ${suffixes_copy})

endmacro(FIND_LIB_FILES_PATTERN)

###############################################################################

# Makro wyszukuje bibliotek statycznych lub plików lib dla wspó³dzielonych bibliotek (windows).
# Zak³ada, ¿e istniej¹ dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szukaæ bibliotek w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szukaæ bibliotek w wersji release
# Wyjanienie: extension u¿ywany jest w sytuacji, gdy
# CMake nie potrafi wyszukaæ biblioteki bez rozszerzenia (np. biblioteki stayczne na Unixie)
# w 99% przypadków jednak nic nie zawiera i w tych wypadkach rozszerzenia brane s¹ z suffixes.
# Rezultaty:
# 	${variable}_LIBRARY_DEBUG lokalizacja biblioteki w wersji debug
#   ${variable}_LIBRARY_RELEASE lokazliacja biblioteki w wersji release
macro(FIND_LIBS_PATTERN variable releasePattern debugPattern extensions)

	FIND_LIB_FILES_PATTERN(${variable} "${releasePattern}" "${debugPattern}" "LIBRARY_RELEASE" "LIBRARY_DEBUG" "FIND_LIBS_PATTERN" "${extensions}")

endmacro(FIND_LIBS_PATTERN)

###############################################################################

# Makro wyszukuje bibliotek dynamicznych.
# Zak³ada, ¿e istniej¹ dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szukaæ bibliotek w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szukaæ bibliotek w wersji release
# Wyjanienie: extension u¿ywany jest w sytuacji, gdy
# CMake nie potrafi wyszukaæ biblioteki bez rozszerzenia (np. biblioteki stayczne na Unixie)
# w 99% przypadków jednak nic nie zawiera i w tych wypadkach rozszerzenia brane s¹ z suffixes.
# Rezultaty:
# 	${variable}_LIBRARY_DEBUG_DLL lokalizacja biblioteki w wersji debug
#   ${variable}_LIBRARY_RELEASE_DLL lokazliacja biblioteki w wersji release
macro(FIND_SHARED_PATTERN variable releasePattern debugPattern extensions)

	FIND_LIB_FILES_PATTERN(${variable} "${releasePattern}" "${debugPattern}" "LIBRARY_RELEASE_DLL" "LIBRARY_DEBUG_DLL" "FIND_SHARED_PATTERN" "${extensions}")

endmacro(FIND_SHARED_PATTERN)

###############################################################################

# Makro wyszukuje plików wykonywalnych.
# Zak³ada, ¿e istniej¹ dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szukaæ aplikacji w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szukaæ aplikacji w wersji release
# Wyjanienie: extension u¿ywany jest w sytuacji, gdy
# CMake nie potrafi wyszukaæ aplikacji bez rozszerzenia (np. na Unixie)
# w 99% przypadków jednak nic nie zawiera i w tych wypadkach rozszerzenia brane s¹ z suffixes.
# Rezultaty:
# 	${variable}_EXECUTABLE_DEBUG lokalizacja aplikacji w wersji debug
#   ${variable}_EXECUTABLE_RELEASE lokazliacja aplikacji w wersji release

macro(FIND_EXECUTABLE_PATTERN variable releasePattern debugPattern)
	FIND_NOTIFY(${variable} "FIND_EXECUTABLE_PATTERN: begin")
	if (FIND_DISABLE_CUSTOM_DIRECTORY)
		FIND_NOTIFY(${variable} "FIND_EXECUTABLE_PATTERN: only system directories!")
	endif()

	set(_app_names)
	CREATE_NAMES_LIST("${releasePattern}" _app_names)
	FIND_NOTIFY(${variable} "FIND_EXECUTABLE_PATTERN: release pattern ${releasePattern} unrolled to ${_app_names}")
	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		# najpierw przeszukiwany jest katalog release
		find_program(${variable}_EXECUTABLE_RELEASE
			NAMES ${_app_names}
			PATHS ${FIND_DIR_RELEASE}
			DOC "Location of ${variable}"
			NO_DEFAULT_PATH
		)
	endif()
	# potem w ca³ym systemie
	find_program(${variable}_EXECUTABLE_RELEASE
		NAMES ${_app_names}
		DOC "Location of ${variable}"
	)
	
	set(_app_names)
	CREATE_NAMES_LIST("${debugPattern}" _app_names)
	FIND_NOTIFY(${variable} "FIND_EXECUTABLE_PATTERN: debug pattern ${debugPattern} unrolled to ${_app_names}")

	if (NOT FIND_DISABLE_CUSTOM_DIRECTORY)
		# najpierw przeszukiwany jest katalog debug		
		find_program(${variable}_EXECUTABLE_DEBUG
			NAMES ${_app_names}
			PATHS ${FIND_DIR_DEBUG}
			DOC "Location of ${variable}"
			NO_DEFAULT_PATH
		)
	endif()
	# potem w ca³ym systemie
	find_program(${variable}_EXECUTABLE_DEBUG
		NAMES ${_app_names}
		DOC "Location of ${variable}"
	)
	
	FIND_NOTIFY(${variable} "FIND_EXECUTABLE_PATTERN: debug app: ${${variable}_EXECUTABLE_DEBUG}; release app: ${${variable}_EXECUTABLE_RELEASE}")
	FIND_NOTIFY(${variable} "FIND_EXECUTABLE_PATTERN: end")
	
endmacro(FIND_EXECUTABLE_PATTERN)

###############################################################################

# Makro wyszukuje plików wykonywalnych.
# Zak³ada, ¿e istniej¹ dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szukaæ aplikacji w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szukaæ aplikacji w wersji release
# Wyjanienie: extension u¿ywany jest w sytuacji, gdy
# CMake nie potrafi wyszukaæ aplikacji bez rozszerzenia (np. na Unixie)
# w 99% przypadków jednak nic nie zawiera i w tych wypadkach rozszerzenia brane s¹ z suffixes.
# Rezultaty:
# 	${variable}_EXECUTABLE_DEBUG lokalizacja aplikacji w wersji debug
#   ${variable}_EXECUTABLE_RELEASE lokazliacja aplikacji w wersji release

macro(FIND_EXECUTABLE variable names)
	
	FIND_EXECUTABLE_EXT(${variable} "${names}" "${names}<d,?>")
	
endmacro(FIND_EXECUTABLE)

###############################################################################

# Makro wyszukuje plików wykonywalnych.
# Zak³ada, ¿e istniej¹ dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szukaæ aplikacji w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szukaæ aplikacji w wersji release
# Wyjanienie: extension u¿ywany jest w sytuacji, gdy
# CMake nie potrafi wyszukaæ aplikacji bez rozszerzenia (np. na Unixie)
# w 99% przypadków jednak nic nie zawiera i w tych wypadkach rozszerzenia brane s¹ z suffixes.
# Rezultaty:
# 	${variable}_EXECUTABLE_DEBUG lokalizacja aplikacji w wersji debug
#   ${variable}_EXECUTABLE_RELEASE lokazliacja aplikacji w wersji release

macro(FIND_EXECUTABLE_EXT variable namesRelease namesDebug)
	
	_FIND_EXECUTABLE_EXT(${variable} "${namesRelease}" "${namesDebug}" "_ALL")
	
endmacro(FIND_EXECUTABLE_EXT)



macro(_FIND_EXECUTABLE_EXT variable namesRelease namesDebug prefix)
	
	FIND_EXECUTABLE_PATTERN(${variable} "${namesRelease}" "${namesDebug}")
	
	set(EXECUTABLE_${variable}_FOUND)
	
	# czy uda³o siê cokolwiek?
	if (${variable}_EXECUTABLE_DEBUG OR ${variable}_EXECUTABLE_RELEASE)

		# czy uda³o siê znaleæ odpowiednie warianty?
		if ( ${variable}_EXECUTABLE_DEBUG AND ${variable}_EXECUTABLE_RELEASE )
			list(APPEND ${prefix}_RELEASE_EXECUTABLES ${variable}_EXECUTABLE_RELEASE)
			list(APPEND ${prefix}_DEBUG_EXECUTABLES ${variable}_EXECUTABLE_DEBUG)
		elseif ( ${variable}_LIBRARY_DEBUG )
			list(APPEND ${prefix}_RELEASE_EXECUTABLES ${variable}_EXECUTABLE_DEBUG)
			list(APPEND ${prefix}_DEBUG_EXECUTABLES ${variable}_EXECUTABLE_DEBUG)
			FIND_MESSAGE("Release version of ${variable} executable not found, using Debug version.")
		else()
			list(APPEND ${prefix}_RELEASE_EXECUTABLES ${variable}_EXECUTABLE_RELEASE)
			list(APPEND ${prefix}_DEBUG_EXECUTABLES ${variable}_EXECUTABLE_RELEASE)
			FIND_MESSAGE("Debug version of ${variable} executable not found, using Release version.")
		endif()

		# znalelimy
		set(EXECUTABLE_${variable}_FOUND 1)
		FIND_NOTIFY_RESULT(1)
	else()
		FIND_NOTIFY_RESULT(0)
	endif()
	
endmacro(_FIND_EXECUTABLE_EXT)

###############################################################################

# Makro wyszukuje biblioteki z pojedynczego pliku
# Zak³ada, ¿e istniej¹ dwie zmienne:
# FIND_DIR_DEBUG Miejsca gdzie szukaæ bibliotek w wersji debug
# FIND_DIR_RELEASE Miejsca gdzie szukaæ bibliotek w wersji release
# Rezultaty:
# 	${variable} Zaimportowana biblioteka
#   LIBRARY_${variable}_FOUND Flaga okrelaj¹ca, czy siê uda³o
#   ${variable}_LIBRARY_DEBUG cie¿ka do biblioteki w wersji DEBUG.
#   ${variable}_LIBRARY_RELEASE cie¿ka do biblioteki w wersji RELEASE.
macro(_ADD_LIBRARY_SINGLE variable names debugNames static prefix)

	set(LIBRARY_${variable}_FOUND 0)
	
	if(NOT DEFINED ${prefix}_RELEASE_LIBS)
		set(${prefix}_RELEASE_LIBS "")
	endif()
	
	if(NOT DEFINED ${prefix}_DEBUG_LIBS)
		set(${prefix}_DEBUG_LIBS "")
	endif()
	
	if(NOT DEFINED ${prefix}_LIBS)
		set(${prefix}_LIBS "")
	endif()

	# szukamy libów
	if(${static})
		if ( WIN32 )
			FIND_LIBS_PATTERN(${variable} "${names}" "${debugNames}" ".lib")
		else()
			FIND_LIBS_PATTERN(${variable} "${names}" "${debugNames}" ".a")
		endif()
		
		# czy uda³o siê cokolwiek?
		if (${variable}_LIBRARY_DEBUG OR ${variable}_LIBRARY_RELEASE)

			# czy uda³o siê znaleæ odpowiednie warianty?
			if ( ${variable}_LIBRARY_DEBUG AND ${variable}_LIBRARY_RELEASE )
				list(APPEND ${prefix}_RELEASE_LIBS ${variable}_LIBRARY_RELEASE)
				list(APPEND ${prefix}_DEBUG_LIBS ${variable}_LIBRARY_DEBUG)
				list(APPEND ${prefix}_LIBS optimized "${${variable}_LIBRARY_RELEASE}" debug "${${variable}_LIBRARY_DEBUG}")
			elseif ( ${variable}_LIBRARY_DEBUG )
				list(APPEND ${prefix}_DEBUG_LIBS ${variable}_LIBRARY_DEBUG)
				list(APPEND ${prefix}_LIBS "${${variable}_LIBRARY_DEBUG}")
				FIND_MESSAGE("Release version of ${variable} not found, using Debug version.")
			else()
				list(APPEND ${prefix}_RELEASE_LIBS ${variable}_LIBRARY_RELEASE)
				list(APPEND ${prefix}_LIBS "${${variable}_LIBRARY_RELEASE}")
				FIND_MESSAGE("Debug version of ${variable} not found, using Release version.")
			endif()

			# znalelimy
			set(LIBRARY_${variable}_FOUND 1)
			FIND_NOTIFY_RESULT(1)
			
		endif()
		
	else()
	
		if ( WIN32 )
			FIND_SHARED_PATTERN(${variable} "${names}" "${debugNames}" ".dll")
		else()
			FIND_SHARED_PATTERN(${variable} "${names}" "${debugNames}" ".so")
		endif()	

		# czy uda³o siê cokolwiek?
		if (${variable}_LIBRARY_DEBUG_DLL OR ${variable}_LIBRARY_RELEASE_DLL)

			# czy uda³o siê znaleæ odpowiednie warianty?
			if ( ${variable}_LIBRARY_DEBUG_DLL AND ${variable}_LIBRARY_RELEASE_DLL )
				list(APPEND ${prefix}_RELEASE_DLLS ${variable}_LIBRARY_RELEASE_DLL)
				list(APPEND ${prefix}_DEBUG_DLLS ${variable}_LIBRARY_DEBUG_DLL)
				if(NOT WIN32)
					list(APPEND ${prefix}_LIBS optimized "${${variable}_LIBRARY_RELEASE_DLL}" debug "${${variable}_LIBRARY_DEBUG_DLL}")
				endif()
			elseif ( ${variable}_LIBRARY_DEBUG_DLL )
				list(APPEND ${prefix}_DEBUG_DLLS ${variable}_LIBRARY_DEBUG_DLL)
				if(NOT WIN32)
					list(APPEND ${prefix}_LIBS "${${variable}_LIBRARY_DEBUG_DLL}")
				endif()
				FIND_MESSAGE("Release version of ${variable} not found, using Debug version.")
			else()
				list(APPEND ${prefix}_RELEASE_DLLS ${variable}_LIBRARY_RELEASE_DLL)
				if(NOT WIN32)
					list(APPEND ${prefix}_LIBS "${${variable}_LIBRARY_RELEASE_DLL}")
				endif()
				FIND_MESSAGE("Debug version of ${variable} not found, using Release version.")
			endif()

			# znalelimy
			set(LIBRARY_${variable}_FOUND 1)
			FIND_NOTIFY_RESULT(1)
		endif()
	endif()
	
	if( NOT LIBRARY_${variable}_FOUND )
		# nie znaleziono niczego
		if(${static})
			FIND_MESSAGE("Static library ${variable} not found")
		else()
			FIND_MESSAGE("Shared library ${variable} not found")
		endif()
		FIND_NOTIFY_RESULT(0)
	endif()

endmacro (_ADD_LIBRARY_SINGLE)


macro(ADD_LIBRARY_SINGLE variable names debugNames static)

_ADD_LIBRARY_SINGLE(${variable} "${names}" "${debugNames}" ${static} "_ALL")

endmacro (ADD_LIBRARY_SINGLE)

###############################################################################

macro(_FIND_STATIC_EXT variable names debugNames prefix)
	FIND_NOTIFY(${variable} "FIND_STATIC_EXT: begin")
	_ADD_LIBRARY_SINGLE(${variable} "${names}" "${debugNames}" 1 "${prefix}")
	FIND_NOTIFY(${variable} "FIND_STATIC_EXT: debug lib: ${${variable}_LIBRARY_DEBUG}; release lib ${${variable}_LIBRARY_RELEASE}")
	FIND_NOTIFY(${variable} "FIND_STATIC_EXT: end")
endmacro(_FIND_STATIC_EXT)

macro(FIND_STATIC_EXT variable names debugNames)
	_FIND_STATIC_EXT(${variable} "${names}" "${debugNames}" "_ALL")
endmacro(FIND_STATIC_EXT)

# Wyszukuje bibliotekê statyczn¹
# variable	Nazwa zmiennej
# shortname	Nazwa biblioteki (nazwa pliku)
# Odnonie rezulatów przeczytaj komentarz do makra ADD_LIBRARY_SINGLE
macro(FIND_STATIC variable names)
	FIND_STATIC_EXT(${variable} "${names}" "${names}<d,?>")
endmacro(FIND_STATIC)

###############################################################################

macro (_FIND_SHARED_EXT variable names debugNames dllNames dllDebugNames prefix)
	FIND_NOTIFY(${variable} "FIND_SHARED_EXT: begin")
	if (NOT WIN32)
		# jeden plik
		ADD_LIBRARY_SINGLE(${variable} "${names}" "${debugNames}" 0)
	else()
		
		set(LIBRARY_${variable}_FOUND 0)
	
		# bêdzie plik lib i dll...
		# szukamy libów
		FIND_LIBS_PATTERN(${variable} "${names}" "${debugNames}" ".lib")
		# szukamy dllek
		FIND_SHARED_PATTERN(${variable} "${dllNames}" "${dllDebugNames}" ".dll")
		set(MESSAGE_BODY "${variable} (${dllNames})")
		if ((${variable}_LIBRARY_DEBUG AND ${variable}_LIBRARY_DEBUG_DLL) OR (${variable}_LIBRARY_RELEASE AND ${variable}_LIBRARY_RELEASE_DLL))
			# ok, mamy co najmniej jedn¹ wersjê
			if ((${variable}_LIBRARY_DEBUG AND ${variable}_LIBRARY_DEBUG_DLL) AND
				(${variable}_LIBRARY_RELEASE AND ${variable}_LIBRARY_RELEASE_DLL))
				list(APPEND _ALL_RELEASE_DLLS ${variable}_LIBRARY_RELEASE_DLL)
				list(APPEND _ALL_DEBUG_DLLS ${variable}_LIBRARY_DEBUG_DLL)
				list(APPEND _ALL_RELEASE_LIBS ${variable}_LIBRARY_RELEASE)
				list(APPEND _ALL_DEBUG_LIBS ${variable}_LIBRARY_DEBUG)
				
				list(APPEND _ALL_LIBS optimized "${${variable}_LIBRARY_RELEASE}" debug "${${variable}_LIBRARY_DEBUG}")
				
			elseif (${variable}_LIBRARY_DEBUG AND ${variable}_LIBRARY_DEBUG_DLL)
				list(APPEND _ALL_DEBUG_DLLS ${variable}_LIBRARY_DEBUG_DLL)
				list(APPEND _ALL_DEBUG_LIBS ${variable}_LIBRARY_DEBUG)
				
				list(APPEND _ALL_LIBS "${${variable}_LIBRARY_DEBUG}")
				
				FIND_MESSAGE("Release version of ${MESSAGE_BODY} not found, using Debug version.")
			else()
				list(APPEND _ALL_RELEASE_DLLS ${variable}_LIBRARY_RELEASE_DLL)
				list(APPEND _ALL_RELEASE_LIBS ${variable}_LIBRARY_RELEASE)
				
				list(APPEND _ALL_LIBS "${${variable}_LIBRARY_RELEASE}")
				
				FIND_MESSAGE("Debug version of ${MESSAGE_BODY} not found, using Release version.")
			endif()

			# znalelimy
			set(LIBRARY_${variable}_FOUND 1)
			FIND_NOTIFY_RESULT(1)
		else()
			# nie znaleziono niczego
			FIND_MESSAGE("Shared library ${MESSAGE_BODY} was not found")
			FIND_NOTIFY_RESULT(0)
		endif()
	endif()
	FIND_NOTIFY(${variable} "FIND_SHARED_EXT: debug lib: ${${variable}_LIBRARY_DEBUG}; release lib: ${${variable}_LIBRARY_RELEASE}; debug dll: ${${variable}_LIBRARY_DEBUG_DLL}; release dll: ${${variable}_LIBRARY_RELEASE_DLL}")
	FIND_NOTIFY(${variable} "FIND_SHARED_EXT: end")
endmacro( _FIND_SHARED_EXT )

macro (FIND_SHARED_EXT variable names debugNames dllNames dllDebugNames)

_FIND_SHARED_EXT(${variable} "${names}" "${debugNames}" "${dllNames}" "${dllDebugNames}" "_ALL")

endmacro( FIND_SHARED_EXT )

#################################################################################################

# Wyszukuje bibliotekê wspó³dzielon¹
# variable	Nazwa zmiennej
# names	Nazwa biblioteki (nazwa pliku) .so dla Unixa lub .lib dla Windowsa
# dllNames Mo¿liwe nazwy biblioteki .dll dla Windowsa.
# Odnonie rezulatów przeczytaj komentarz do makra ADD_LIBRARY_SINGLE
macro (FIND_SHARED variable names dllNames)
	FIND_SHARED_EXT(${variable} "${names}" "${names}<d,?>" "${dllNames}" "${dllNames}<d,?>")
endmacro (FIND_SHARED)

###############################################################################

# Wyszukuje katalog wymagany dla biblioteki
# Parametry:
#	variable	Nazwa zmiennej
#	pathRelease	Wzglêdna cie¿ka katalogu dla release
#	pathDebug	Wzglêdna cie¿ka katalogu dla debug
macro (_FIND_LIBRARY_ADDITIONAL_DIRECTORY_EXT variable pathRelease pathDebug)
	
	set(${variable}_DIRECTORY_RELEASE)
	
	if(IS_DIRECTORY "${FIND_DIR_RELEASE}/${pathRelease}")
		set(${variable}_DIRECTORY_RELEASE "${FIND_DIR_RELEASE}/${pathRelease}")
	endif()
	
	set(${variable}_DIRECTORY_DEBUG)
	
	if(IS_DIRECTORY "${FIND_DIR_DEBUG}/${pathDebug}")
		set(${variable}_DIRECTORY_DEBUG "${FIND_DIR_DEBUG}/${pathDebug}")
	endif()
	
endmacro(_FIND_LIBRARY_ADDITIONAL_DIRECTORY_EXT)

###############################################################################

# Wyszukuje katalog wymagany dla biblioteki
# Parametry:
#	variable	Nazwa zmiennej
#	pathRelease	Wzglêdna cie¿ka katalogu dla release
#	pathDebug	Wzglêdna cie¿ka katalogu dla debug
macro(FIND_DIRECTORY variable path)
	
	FIND_DIRECTORY_EXT(${variable} "${path}" "${path}")
	
endmacro(FIND_DIRECTORY)

###############################################################################

# Wyszukuje katalog wymagany dla biblioteki
# Parametry:
#	variable	Nazwa zmiennej
#	pathRelease	Wzglêdna cie¿ka katalogu dla release
#	pathDebug	Wzglêdna cie¿ka katalogu dla debug
macro(FIND_DIRECTORY_EXT variable pathRelease pathDebug)
	FIND_NOTIFY(${variable} "FIND_DIRECTORY_EXT: begin")
	_FIND_LIBRARY_ADDITIONAL_DIRECTORY_EXT(${variable} "${pathRelease}" "${pathDebug}")
	set(DIRECTORY_${variable}_FOUND)
	set(MESSAGE_BODY "${variable} (${pathRelease}, (${pathDebug})")
	
	# czy uda³o siê cokolwiek?
	if (${variable}_DIRECTORY_DEBUG OR ${variable}_DIRECTORY_RELEASE)

		# czy uda³o siê znaleæ odpowiednie warianty?
		if ( ${variable}_DIRECTORY_DEBUG AND ${variable}_DIRECTORY_RELEASE )
			list(APPEND _ALL_RELEASE_DIRECTORIES ${variable}_DIRECTORY_RELEASE)
			list(APPEND _ALL_DEBUG_DIRECTORIES ${variable}_DIRECTORY_DEBUG)
		elseif ( ${variable}_DIRECTORY_DEBUG )
			list(APPEND _ALL_RELEASE_DIRECTORIES ${variable}_DIRECTORY_DEBUG)
			list(APPEND _ALL_DEBUG_DIRECTORIES ${variable}_DIRECTORY_DEBUG)
			FIND_MESSAGE("Release version of ${variable} directory not found, using Debug version.")
		else()
			list(APPEND _ALL_RELEASE_DIRECTORIES ${variable}_DIRECTORY_RELEASE)
			list(APPEND _ALL_DEBUG_DIRECTORIES ${variable}_DIRECTORY_RELEASE)
			FIND_MESSAGE("Debug version of ${variable} direcotry not found, using Release version.")
		endif()

		# znalelimy
		set(DIRECTORY_${variable}_FOUND 1)
		FIND_NOTIFY_RESULT(1)
	else()
		FIND_MESSAGE("Directory ${MESSAGE_BODY} was not found")
		FIND_NOTIFY_RESULT(0)
	endif()
	FIND_NOTIFY(${variable} "FIND_DIRECTORY_EXT: debug dir: ${${variable}_DIRECTORY_DEBUG}; release dir: ${${variable}_DIRECTORY_RELEASE}")
	FIND_NOTIFY(${variable} "FIND_DIRECTORY_EXT: end")
endmacro(FIND_DIRECTORY_EXT)

###############################################################################

# Wyszukuje tłumaczenia wymagane dla biblioteki
# Parametry:
#	variable	Nazwa zmiennej
#	path	Sciezka do tlumaczen
macro(FIND_TRANSLATIONS variable path)
	
	FIND_TRANSLATIONS_EXT(${variable} "${path}" "${path}")
	
endmacro(FIND_TRANSLATIONS)

###############################################################################

# Wyszukuje katalog wymagany dla biblioteki
# Parametry:
#	variable	Nazwa zmiennej
#	pathRelease	Wzglêdna cie¿ka katalogu dla release
#	pathDebug	Wzglêdna cie¿ka katalogu dla debug

macro(GENERATE_TRANSLATION_PATERNS var translationLangs)

	set(${var} "${translationLangs}")
	
	foreach(l ${translationLangs})
	
		string(REPLACE "_" ";" _l ${l})
		list(APPEND ${var} ${_l})
	
	endforeach()
	
	list(REMOVE_DUPLICATES ${var})

endmacro(GENERATE_TRANSLATION_PATERNS)

###############################################################################

# Wyszukuje katalog wymagany dla biblioteki
# Parametry:
#	variable	Nazwa zmiennej
#	pathRelease	Wzglêdna cie¿ka katalogu dla release
#	pathDebug	Wzglêdna cie¿ka katalogu dla debug
macro(_FIND_TRANSLATIONS_EXT variable pathRelease pathDebug releasePatterns debugPatterns)
	list(LENGTH SOLUTION_TRANSLATION_LANGUAGES _solTransLength)
	list(LENGTH ${releasePatterns} _rPatternsLength)
	list(LENGTH ${debugPatterns} _dPatternsLength)	
	
	if(${_solTransLength} GREATER 0 AND (${_rPatternsLength} GREATER 0 OR ${_dPatternsLength} GREATER 0))
	
		set(translationFilesRelease "")
		
		foreach(rPattern ${${releasePatterns}})
			
			# Gather list of all .qm files
			file(GLOB _transFiles "${FIND_DIR_RELEASE}/${pathRelease}/${rPattern}.qm")
			list(APPEND translationFilesRelease ${_transFiles})
			
		endforeach()
		
		list(REMOVE_DUPLICATES translationFilesRelease)		
		
		set(translationFilesDebug "")
		
		foreach(dPattern ${${debugPatterns}})
			
			# Gather list of all qm files
			file(GLOB _transFiles "${FIND_DIR_DEBUG}/${pathDebug}/${dPattern}.qm")
			list(APPEND translationFilesDebug ${_transFiles})
		
		endforeach()
		
		list(REMOVE_DUPLICATES translationFilesDebug)		
		list(LENGTH translationFilesRelease releaseTranslationsLength)
		list(LENGTH translationFilesDebug debugTranslationsLength)
			
		set(TRANSLATIONS_${variable}_FOUND 0)
		set(MESSAGE_BODY "${variable} (${pathRelease}), (${pathDebug})")
		
		# czy uda³o siê cokolwiek?
		if (${debugTranslationsLength} GREATER 0 OR ${releaseTranslationsLength} GREATER 0)

			# czy uda³o siê znaleæ odpowiednie warianty?
			if ( ${debugTranslationsLength} GREATER 0 AND ${releaseTranslationsLength} GREATER 0 )
				list(APPEND _LIBRARY_RELEASE_TRANSLATIONS ${translationFilesRelease})
				list(APPEND _LIBRARY_DEBUG_TRANSLATIONS ${translationFilesDebug})
			elseif ( ${debugTranslationsLength} GREATER 0 )
				list(APPEND _LIBRARY_RELEASE_TRANSLATIONS ${translationFilesDebug})
				list(APPEND _LIBRARY_DEBUG_TRANSLATIONS ${translationFilesDebug})
				FIND_MESSAGE("Release version of ${variable} translations not found, using Debug version.")
			else()
				list(APPEND _LIBRARY_RELEASE_TRANSLATIONS ${translationFilesRelease})
				list(APPEND _LIBRARY_DEBUG_TRANSLATIONS ${translationFilesRelease})
				FIND_MESSAGE("Debug version of ${variable} translations not found, using Release version.")
			endif()

			# znalelimy
			set(TRANSLATIONS_${variable}_FOUND 1)
			FIND_NOTIFY_RESULT(1)
		else()
			FIND_MESSAGE("Translations ${MESSAGE_BODY} was not found")
			FIND_NOTIFY_RESULT(0)
		endif()
	
	else()
	
		# znalelimy
		set(TRANSLATIONS_${variable}_FOUND 1)
		FIND_NOTIFY_RESULT(1)
	
	endif()
	
endmacro(_FIND_TRANSLATIONS_EXT)

###############################################################################

# Wyszukuje katalog wymagany dla biblioteki
# Parametry:
#	variable	Nazwa zmiennej
#	pathRelease	Wzglêdna cie¿ka katalogu dla release
#	pathDebug	Wzglêdna cie¿ka katalogu dla debug
macro(FIND_TRANSLATIONS_EXT variable pathRelease pathDebug)
	FIND_NOTIFY(${variable} "FIND_TRANSLATIONS_EXT: begin")
	_FIND_TRANSLATIONS_EXT(${variable} "${pathRelease}" "${pathDebug}" "*" "*")
	FIND_NOTIFY(${variable} "FIND_TRANSLATIONS_EXT: debug translations: ${_LIBRARY_DEBUG_TRANSLATIONS}; release translations: ${_LIBRARY_RELEASE_TRANSLATIONS}")
	FIND_NOTIFY(${variable} "FIND_TRANSLATIONS_EXT: end")
endmacro(FIND_TRANSLATIONS_EXT)

###############################################################################

macro(FIND_MESSAGE)
	if (NOT FIND_SUPRESS_MESSAGES)
		message(${ARGV})
	endif()
endmacro(FIND_MESSAGE)

###############################################################################

macro(FIND_NOTIFY_RESULT value)
	if ( ${value} )
		set(FIND_RESULTS_LOGICAL_OR 1)
	else()
		if ( FIND_RESULTS_LOGICAL_AND )
			set(FIND_RESULTS_LOGICAL_AND 0)
		endif()
	endif()
endmacro(FIND_NOTIFY_RESULT)

###############################################################################

# Tworzy listê nazw na podstawie wzoru; miejsca podmiany musz¹ byæ w ostrych
# nawiasach, natomiast wartoci oddzielone przecinkiem; znak "?" to specjalna
# wartoæ oznaczaj¹ca pusty ³añcuch
# nie mog¹ powtarzaæ siê identyczne miejsca podmiany! (u³omnoæ CMake)
# przyk³ad: pattern = bib<1,2,3>v<?,_d>
#			result = bib1v;bib1v_d;bib2v;bib2v_d;bib3v;bib3v_d
macro(CREATE_NAMES_LIST pattern result)
	set(_names ${pattern})
	set(_pattern ${pattern})	
	
	# pobranie opcji
	string(REGEX MATCH "<([^<]*)>" _toReplace ${_pattern})
	
	while(_toReplace)
						
		# konwersja na listê
		if (NOT CMAKE_MATCH_1 STREQUAL "")
			string(REPLACE "," ";" _options ${CMAKE_MATCH_1})
		else()
			set(_options "?")
		endif()
		
		# usuniêcie opcji z ³añcucha
		STRING_REPLACE_FIRST("${_pattern}" "${_toReplace}" "" _pattern ON)		
		# podmiana klucza
		set(_newNames)
		foreach( comb ${_names} )
			foreach (opt ${_options})
				# znak zapytania traktowany jako pusty znak
				if (opt STREQUAL "?")
					STRING_REPLACE_FIRST("${comb}" "${_toReplace}" "" _temp ON)					
				else()
					STRING_REPLACE_FIRST("${comb}" "${_toReplace}" "${opt}" _temp ON)					
				endif()
				list(APPEND _newNames ${_temp})
			endforeach()
		endforeach()
		set(_names ${_newNames})

		# pobranie opcji
		string(REGEX MATCH "<([^<]*)>" _toReplace ${_pattern})
		
	endwhile()
	set(${result} ${_names})
endmacro(CREATE_NAMES_LIST)

###############################################################################

macro(FIND_NOTIFY var msg)
	if (FIND_VERBOSE)
		message(STATUS "FIND>${var}>${msg}")
	endif()
endmacro(FIND_NOTIFY)

###############################################################################
# Makro szuka pojedynszej biblioteki dynamicznej dla której nie ma ¿adnej libki i nag³ówków
# Makro przeznaczone do szukania np. pluginów innych, wiêkszych bibliotek
# Parametry:
#	variable - nazwa zmiennej dla biblioteki któr¹ szukamy, na jej podstawie powstanie
#				zmienna LIBRARY_${variable}_FOUND zawieraj¹ca info czy uda³o siê znaleæ bibliotekê
#	release - nazwa biblioteki dla release
#	debug - nazwa biblioteki dla debug
macro(FIND_DLL_EXT variable release debug)
	FIND_NOTIFY(${variable} "FIND_DLL_EXT: begin")	
	# szukamy samych, go³ych dllek - np. pluginów innych bibliotek jak OpenCV i FFMPEG
	FIND_LIB_FILES_PATTERN(${variable} "${release}" "${debug}" "LIBRARY_RELEASE_DLL" "LIBRARY_DEBUG_DLL" "FIND_DLL" ".dll")
	
	if (${variable}_LIBRARY_DEBUG_DLL OR ${variable}_LIBRARY_RELEASE_DLL)

		# czy uda³o siê znaleæ odpowiednie warianty?
		if ( ${variable}_LIBRARY_DEBUG_DLL AND ${variable}_LIBRARY_RELEASE_DLL )
			list(APPEND _ALL_RELEASE_DLLS ${variable}_LIBRARY_RELEASE_DLL)
			list(APPEND _ALL_DEBUG_DLLS ${variable}_LIBRARY_DEBUG_DLL)
			if(NOT WIN32)
				list(APPEND _ALL_LIBS optimized "${${variable}_LIBRARY_RELEASE_DLL}" debug "${${variable}_LIBRARY_DEBUG_DLL}")
			endif()
		elseif ( ${variable}_LIBRARY_DEBUG_DLL )
			list(APPEND _ALL_RELEASE_DLLS ${variable}_LIBRARY_DEBUG_DLL)
			list(APPEND _ALL_DEBUG_DLLS ${variable}_LIBRARY_DEBUG_DLL)
			if(NOT WIN32)
				list(APPEND _ALL_LIBS "${${variable}_LIBRARY_DEBUG_DLL}")
			endif()
			FIND_MESSAGE("Release version of ${variable} not found, using Debug version.")
		else()
			list(APPEND _ALL_RELEASE_DLLS ${variable}_LIBRARY_RELEASE_DLL)
			list(APPEND _ALL_DEBUG_DLLS ${variable}_LIBRARY_RELEASE_DLL)
			if(NOT WIN32)
				list(APPEND _ALL_LIBS "${${variable}_LIBRARY_RELEASE_DLL}")
			endif()
			FIND_MESSAGE("Debug version of ${variable} not found, using Release version.")
		endif()

		# znalelimy
		set(LIBRARY_${variable}_FOUND 1)
		FIND_NOTIFY_RESULT(1)
	else()	
		set(LIBRARY_${variable}_FOUND 0)
		FIND_MESSAGE("Dll library ${variable} not found")		
		FIND_NOTIFY_RESULT(0)
	endif()
	
	FIND_NOTIFY(${variable} "FIND_DLL_EXT: debug dll: ${${variable}_LIBRARY_DEBUG_DLL}; release dll: ${${variable}_LIBRARY_RELEASE_DLL}")
	FIND_NOTIFY(${variable} "FIND_DLL_EXT: end")
		
endmacro(FIND_DLL_EXT)

###############################################################################
# Makro szuka pojedynszej biblioteki dynamicznej dla której nie ma ¿adnej libki i nag³ówków
# Makro przeznaczone do szukania np. pluginów innych, wiêkszych bibliotek
# Parametry:
#	variable - nazwa zmiennej dla biblioteki któr¹ szukamy, na jej podstawie powstanie
#				zmienna LIBRARY_${variable}_FOUND zawieraj¹ca info czy uda³o siê znaleæ bibliotekê
#	name - nazwa biblioteki
macro(FIND_DLL variable name)

	# szukamy samych, go³ych dllek - np. pluginów innych bibliotek jak OpenCV i FFMPEG
	FIND_DLL_EXT(${variable} "${name}" "${name}<d,?>")
		
endmacro(FIND_DLL)

###############################################################################

# Makro szukaj¹ce dodatkowych zale¿noci bibliotek zale¿nych
# Parametry:
#	library - biblioteka dla której szukamy dodatkowych zale¿noci
#	depsList - lista bibliotek zale¿nych
#	[dodatkowe nag³ówki do wci¹gniêcia] - opcjonalny parametr, lista par -> biblioteka + reszta cie¿ki wzglêdem jej includów
macro (FIND_DEPENDENCIES library depsList)

	set(_DEPENDENCY_FIND_RESULT 1)
	set(${library}_SECOND_PASS_FIND_DEPENDENCIES "")
	
	if(NOT DEFINED ${library}_ADDITIONAL_INCLUDE_DIRS)
		set(${library}_ADDITIONAL_INCLUDE_DIRS "")
	endif()
	
	set(LIBRARY_${library}_DEPENDENCIES  ${LIBRARY_${library}_DEPENDENCIES} ${depsList})
	
	foreach(dep ${depsList})
		if(DEFINED LIBRARY_${dep}_FOUND)
			# szukano juz tej biblioteki - sprawdzamy czy znaleziono
			if(NOT ${LIBRARY_${dep}_FOUND})
				# nie znaleziono
				set(_DEPENDENCY_FIND_RESULT 0)
			else()
				# znaleziono - muszê sobie dopi¹æ includy i liby
				list(APPEND ${library}_ADDITIONAL_INCLUDE_DIRS "${${dep}_INCLUDE_DIR}")
				
				if(DEFINED ${dep}_ADDITIONAL_INCLUDE_DIRS)
					list(APPEND ${library}_ADDITIONAL_INCLUDE_DIRS "${${dep}_ADDITIONAL_INCLUDE_DIRS}")
				endif()
				
				if(DEFINED LIBRARY_${dep}_LIBRARIES)
					list(APPEND LIBRARY_${library}_LIBRARIES "${LIBRARY_${dep}_LIBRARIES}")
				endif()
			endif()
		else()
			# nie szukano jeszcze tego - dodaje do listy przysz³ych poszukiwañ dependency
			list(APPEND SECOND_PASS_FIND_DEPENDENCIES ${library})
			list(APPEND ${library}_SECOND_PASS_FIND_DEPENDENCIES ${dep})
		endif()
	endforeach()

	# dodatkowe includy na póniej
	if(${ARGC} GREATER 2)
		list(LENGTH ${library}_SECOND_PASS_FIND_DEPENDENCIES depLength)
		if(${depLength} GREATER 0)
			# muszê je prze³o¿yæ na potem bo zale¿noæ bêdzie szukana w drugim przebiegu
			set(${library}_SECOND_PASS_FIND_DEPENDENCIES_INCLUDE ${ARGV2})
		else()
			# mogê je teraz tutaj dodaæ bo wszystko ju¿ mam
			set(additionalIncludes ${ARGV2})
			list(LENGTH additionalIncludes incLength)
			math(EXPR incMod "${incLength} % 2")
			if(${incMod} EQUAL 0)
				math(EXPR incLength "${incLength} / 2")
				
				set(loopIDX 0)
				set(idx 0)
				while(${incLength} GREATER ${loopIDX})
				
					list(GET additionalIncludes ${idx} variableName)
					math(EXPR idx "${idx}+1")
					list(GET additionalIncludes ${idx} path)
					
					if(DEFINED ${variableName})
						list(APPEND ${library}_ADDITIONAL_INCLUDE_DIRS "${${variableName}}/${path}")
					else()
						FIND_NOTIFY(variableName "B³¹d podczas dodawania dodatkowych includów biblioteki ${library}. Zmienna ${variableName} nie istnieje, cie¿ka ${variableName}/${path} nie mog³a byæ dodana.")
						set(_DEPENDENCY_FIND_RESULT 0)
					endif()
					math(EXPR idx "${idx}+1")
					math(EXPR loopIDX "${loopIDX}+1")
					
				endwhile()
			else()
				FIND_NOTIFY(additionalIncludes "B³¹d dodawania dodatkowych includów - d³ugoæ listy jest nieparzysta (b³êdny format listy). Lista: ${additionalIncludes}")
				set(_DEPENDENCY_FIND_RESULT 0)
			endif()
		endif()
	endif()
	
	FIND_NOTIFY_RESULT(${_DEPENDENCY_FIND_RESULT})

endmacro(FIND_DEPENDENCIES)

###############################################################################

# Makro szukaj¹ce dodatkowych zale¿noci bibliotek na podstawie warunków
# Parametry:
#	library - biblioteka dla której szukamy dodatkowych zale¿noci
#	variables - zmienne które decyduj¹ jakie biblioteki podpi¹æ
#	depsON - lista bibliotek zale¿nych podpinanych gdy wszystkie zmienne s¹ ustawione
#	depsOFF - lista bibliotek zale¿nych podpinanych gdy conajmniej jedna zmienna nie ustawiona
macro (FIND_CONDITIONAL_DEPENDENCIES_EXT library variables depsON depsOFF)

	set(_USE_ON 1)

	foreach(var ${variables})
	
		if(NOT ${${var}})
			set(_USE_ON 0)
		endif()
	
	endforeach()
	
	if(_USE_ON)
		FIND_DEPENDENCIES(${library} "${depsON}")
	else()
		FIND_DEPENDENCIES(${library} "${depsOFF}")
	endif()

endmacro(FIND_CONDITIONAL_DEPENDENCIES_EXT)

###############################################################################

# Makro szukaj¹ce dodatkowych zale¿noci bibliotek na podstawie warunków
# Parametry:
#	library - biblioteka dla której szukamy dodatkowych zale¿noci
#	variables - zmienne które decyduj¹ jakie biblioteki podpi¹æ
#	deps - lista bibliotek zale¿nych podpinanych gdy wszystkie zmienne s¹ ustawione
macro (FIND_CONDITIONAL_DEPENDENCIES library variables deps)

	FIND_CONDITIONAL_DEPENDENCIES_EXT(${library} "${variables}" "${deps}" "")

endmacro(FIND_CONDITIONAL_DEPENDENCIES)

###############################################################################

# Makro szukaj¹ce dodatkowych zale¿noci bibliotek na podstawie warunków
# Parametry:
#	library - biblioteka dla której szukamy dodatkowych zale¿noci
#	variables - zmienne które decyduj¹ jakie biblioteki podpi¹æ
#	prereqsON - lista bibliotek zale¿nych podpinanych gdy wszystkie zmienne s¹ ustawione
#	prereqsOFF - lista bibliotek zale¿nych podpinanych gdy conajmniej jedna zmienna nie ustawiona
macro (FIND_CONDITIONAL_PREREQUISITES_EXT library variables prereqsON prereqsOFF)

	set(_USE_ON 1)

	foreach(var ${variables})
	
		if(NOT ${${var}})
			set(_USE_ON 0)
		endif()
	
	endforeach()
	
	if(_USE_ON)
		FIND_PREREQUISITES(${library} "${prereqsON}")
	else()
		FIND_PREREQUISITES(${library} "${prereqsOFF}")
	endif()

endmacro(FIND_CONDITIONAL_PREREQUISITES_EXT)

###############################################################################

# Makro szukaj¹ce dodatkowych zale¿noci bibliotek na podstawie warunków
# Parametry:
#	library - biblioteka dla której szukamy dodatkowych zale¿noci
#	variables - zmienne które decyduj¹ jakie biblioteki podpi¹æ
#	deps - lista bibliotek zale¿nych podpinanych gdy wszystkie zmienne s¹ ustawione
macro (FIND_CONDITIONAL_PREREQUISITES library variables prereqs)

	FIND_CONDITIONAL_PREREQUISITES_EXT(${library} "${variables}" "${prereqs}" "")

endmacro(FIND_CONDITIONAL_PREREQUISITES)

###############################################################################

# Makro pozwalaj¹ce dodawaæ prerequisites dla bibliotek zaleznych
# Parametry:
#	library - biblioteka dla której prerequisites szukamy
#	result - zmienna któa zostanie zaktualizowana czy znaleziono wszystkie prerequisites czy jakiego brakuje
#	prereqList - lista dodatkowych zale¿noci
macro (FIND_PREREQUISITES library prereqList)
	
	set(_PREREQUISIT_FIND_RESULT 1)
	set(${library}_SECOND_PASS_FIND_PREREQUISITES "")
	
	set(LIBRARY_${library}_PREREQUISITES ${LIBRARY_${library}_PREREQUISITES} ${prereqList})	
	foreach(prereq ${prereqList})
		if(DEFINED LIBRARY_${prereq}_FOUND)
			# szukano juz tej biblioteki - sprawdzamy czy znaleziono
			if(NOT ${LIBRARY_${prereq}_FOUND})
				# nie znaleziono
				set(_PREREQUISIT_FIND_RESULT 0)
			endif()
		else()
			# nie szukano jeszcze tego - dodaje do listy przysz³ych poszukiwañ prerequisites
			list(APPEND SECOND_PASS_FIND_PREREQUISITES ${library})
			list(APPEND ${library}_SECOND_PASS_FIND_PREREQUISITES ${prereq})
		endif()
	endforeach()
	
	FIND_NOTIFY_RESULT(${_PREREQUISIT_FIND_RESULT})

endmacro(FIND_PREREQUISITES)

###############################################################################
# Funkcja wykrywaj¹ce czy dana definicja wystêpuje w zadanej zawartosci pliku
# Parametry:
#	fileContent Zawartoæ pliku do przejrzenia
#	preprocesorDefine Define którego szukamy
# Wartoc zwracana:
# 	zmienne ${preprocesorDefine}_FOUND ustawiona na 0 jeli nie znaleziono i na 1 jeli znaleziono
function(FIND_PREPROCESOR_DEFINE fileContent preprocesorDefine)
	# próba odczytania wersji z pliku
	string(REGEX MATCH ".*#define .*${preprocesorDefine}" DEFINE_${preprocesorDefine}_FOUND ${fileContent})
	if(NOT DEFINE_${preprocesorDefine}_FOUND STREQUAL "Unknown")
		set(DEFINE_${preprocesorDefine}_FOUND 0)
	else()
		set(DEFINE_${preprocesorDefine}_FOUND 1)
	endif()
endfunction(FIND_PREPROCESOR_DEFINE)

###############################################################################
# Makro szuka definów w pliku,
# które potem staj¹ siê czêci¹ logiki do³anczania nowych bibliotek do prerequisites
# lub dependencies
# Parametry:
#	srcFile - nag³ówek publiczny który badamy, cie¿ka wzglêdna wg schematu bibliotk
#	defines - lista definów których szukamy
# Na bazie definów powstan¹ odpowiednie zmienne mówi¹ce nam czy define zosta³ znaleziony
# czy nie
macro(FIND_SOURCE_FILE_DEFINE_CONDITIONS srcFile defines)

	FIND_SOURCE_FILE_DEFINE_CONDITIONS_EXT("${_HEADERS_INCLUDE_DIR}/${srcFile}" "${defines}")

endmacro(FIND_SOURCE_FILE_DEFINE_CONDITIONS)

###############################################################################
# Makro szuka definów w pliku,
# które potem staj¹ siê czêci¹ logiki do³anczania nowych bibliotek do prerequisites
# lub dependencies
# Parametry:
#	srcFile - nag³ówek publiczny który badamy - cie¿ka bezwzglêdna
#	defines - lista definów których szukamy
# Na bazie definów powstan¹ odpowiednie zmienne mówi¹ce nam czy define zosta³ znaleziony
# czy nie
macro(FIND_SOURCE_FILE_DEFINE_CONDITIONS_EXT srcFile defines)

	if(IS_ABSOLUTE "${srcFile}")
		if(EXISTS "${srcFile}")
			file(READ "${srcFile}" _fileContent)
			foreach(def ${defines})
				FIND_PREPROCESOR_DEFINE("${_fileContent}" "${def}")
			endforeach()
			FIND_NOTIFY_RESULT(1)
		else()
			FIND_NOTIFY(srcFile "File with conditional defines for dependencies and PREREQUISITES not exist")
			FIND_NOTIFY_RESULT(0)
		endif()
	else()
		FIND_NOTIFY(srcFile "Given file path is not absolute - could not read required defines for conditional dependencies and prerequsites")
		FIND_NOTIFY_RESULT(0)
	endif()
endmacro(FIND_SOURCE_FILE_DEFINE_CONDITIONS_EXT)

###############################################################################
# Sprawdza czy biblioteka jest bezposrednio instalowalna
#	Parametry:
# 			variable - zmienna której ustawiamy 0 lub 1 w zależności czy biblioteka dla danej konfiguracji jest instalowalna
#			type - typ instalacji dla którego sprawdzamy czy biblioteka jest instalowalna
#			library - biblioteka która sprawdzamy		
macro(IS_LIBRARY_INSTALLABLE variable type library)

	set(${variable} 0)

	list(LENGTH LIBRARY_${library}_RELEASE_DLLS _rDlls)
	list(LENGTH LIBRARY_${library}_RELEASE_DIRECTORIES _rDirs)
	#list(LENGTH LIBRARY_${library}_RELEASE_TRANSLATIONS _rTrans)
	
	list(LENGTH LIBRARY_${library}_DEBUG_DLLS _dDlls)
	list(LENGTH LIBRARY_${library}_DEBUG_DIRECTORIES _dDirs)
	#list(LENGTH LIBRARY_${library}_DEBUG_TRANSLATIONS _dTrans)
	
	if(_rDlls GREATER 0 OR _dDlls GREATER 0
		OR _rDirs GREATER 0 OR _dDirs GREATER 0
		OR _rTrans GREATER 0 OR _dTrans GREATER 0)
		
		set(${variable} 1)
		
	elseif(UNIX)
		
		list(LENGTH LIBRARY_${library}_RELEASE_EXECUTABLES _rApps)
		list(LENGTH LIBRARY_${library}_DEBUG_EXECUTABLES _dApps)
		
		if(_rApps GREATER 0 OR _dApps GREATER 0)		
			set(${variable} 1)
		endif()
	
	endif()
	
	
	if(${variable} EQUAL 0 AND "${type}" STREQUAL "dev")
		
		if(DEFINED ${library}_INCLUDE_DIR)
		
			set(${variable} 1)
			
		else()
		
			list(LENGTH LIBRARY_${library}_RELEASE_LIBS _rLibs)
			list(LENGTH LIBRARY_${library}_DEBUG_LIBS _dLibs)
			
			if(_rLibs GREATER 0 OR _dLibs GREATER 0)
		
				set(${variable} 1)
				
			endif()
			
		endif()
	
	endif()	
		
endmacro(IS_LIBRARY_INSTALLABLE)
