# inicjalizacja logowania wiadomosci modulu target
INIT_VERBOSE_OPTION(TARGET "Print target verbose info?")	

###############################################################################
# Zestaw makr u³atwiaj¹cych opisywanie projektów : plików ród³owych, zale¿noci,
# typów artefaktów, instalacji.
#
###############################################################################
#
# Mechanizm dodawania projektów:
# Projekty s¹ dodawane makredm ADD_PROJECT. W tym momencie projekt nie jest faktycznie
# konfigurowany, a jedynie zapamiêtywany do póniejdzej konfiguracji. Dzia³anie to
# jest podyktowane tym, ¿e zanim rozpoczniemy konfigurowaæ chemy wyszukaæ wszystkie niezbêdne
# biblioteki zale¿ne dla wszystkich projektów. Ponadto - jeli projekty s¹ od siebie uzale¿nione
# chcemy wprowadziæ te zale¿noci jawnie, by projekty budowa³y siê w odpowiedniej kolejnoci.
# 
# Typy projektów:
#	project - zwyk³y projekt bêd¹czy czêsci¹ produktu
#	tests - testy dla projects
#	examples - przyk³ady jak dzia³aæ z projects
#
###############################################################################
#
#	Projekty znajduj¹ siê w osobnych katalogach. Ich struktura wygl¹da nastêpuj¹co:
#
#	root/
#		 project/
#				 CMakeLists.txt - plik z konfiguracj¹ projektu, makra: BEGIN_PROJECT, ...., END_PROJECT
#		 		 src - katalog nag³óków prywatnych i plików ród³owych projektu
#				 doc - dokumentacja projektu
#				 resources - resourcy projektu
#				 ui - elementy GUI dla projektu
#
#	public_includes/
#					project - katalog nag³ówków publicznych
#
###############################################################################
#
#	Zmienne u¿ywane podczas opisu projektów:
#	Wyjciowe:
#		PROJECT_${projectName}_DEPENDENCIES_CONFIGURATIONS_SIZE - iloæ konfiguracji dla zale¿nych bibliotek zewnêtrznych
#		PROJECT_${projectName}_DEPENDENCIES_CONFIG_${idx}_VARIABLES - zmienne dla konfiguracji idx decydujace które biblioteki podpi¹æ
#		PROJECT_${projectName}_DEPENDENCIES_CONFIG_${idx}_DEPS_ON - zale¿noci podpinane kiedy wszystkie zmienne s¹ prawdziwe
#		PROJECT_${projectName}_DEPENDENCIES_CONFIG_${idx}_DEPS_OFF - zale¿noci które s¹ podpinane kiedy nie wszystkie zmienne sa prawdziwe
#		PROJECT_${projectName}_DEPENDENCIES - wszystkie zale¿noci projektu
#		PROJECT_${projectName}_PUBLIC_HEADERS - wszystkie publiczne pliki projektu, w³¹cznie z konfigurowalnymi
#		PROJECT_${projectName}_PATH - cie¿ka do projektu z której bêdê go potem konfigurowa³
#		PROJECT_${projectName}_RELATIVE_PATH - wzglêdna scie¿ka projektu na potrzeby instalacji nag³ówków publicznych
#		PROJECT_${projectName}_GROUP - grupa projektów w której ma siê znaleæ projekt
#		PROJECT_${projectName}_INITIALISED - czy projekt poprawnie zainicjalizowany, u¿ywamy równie¿ do wykrywania wzajemnie zale¿nych projektów
#		PROJECT_${projectName}_ADDITIONAL_INSTALLS - dodatkowe instalacje - np. z inną nazwa, w inne miejsce
#
###############################################################################
# Makro ustawiaj¹ce folder dla kolejnych projektów
# Parametry
#	folder Nazwa folderu
macro(BEGIN_PROJECTS_GROUP name)
	string(FIND "${name}" "\\" pos REVERSE)
	
	if(pos GREATER -1)
		message(FATAL_ERROR "Błędna nazwa grupy - nie może zawierać separatora hierarchi grup \"\\\"")
	endif()
	
	string(LENGTH "${name}" lStr)
	if(${lStr} EQUAL 0)
		message(FATAL_ERROR "Pusta nazwa grupy projektów") 
	else()
		if(DEFINED CURRENT_PROJECT_GROUP_NAME)
			set(CURRENT_PROJECT_GROUP_NAME "${CURRENT_PROJECT_GROUP_NAME}\${name}")
		else()
			set(CURRENT_PROJECT_GROUP_NAME "${name}")
		endif()
	endif()

endmacro(BEGIN_PROJECTS_GROUP)

macro(END_PROJECTS_GROUP)

	string(FIND "${CURRENT_PROJECT_GROUP_NAME}" "\\" pos REVERSE)
	if(pos GREATER -1)
		string(SUBSTRING "${CURRENT_PROJECT_GROUP_NAME}" 0 ${pos} CURRENT_PROJECT_GROUP_NAME)
	else()
		set(CURRENT_PROJECT_GROUP_NAME)
	endif()

endmacro(END_PROJECTS_GROUP)

###############################################################################
# Makro tworz¹ce prawdziwe cie¿ki plików
# Parametry
#	relIn Lista wejciowa z nazwami plików do przetworzenia wzglêdna dla path
#	out Lista wyjciowa z cie¿kami plików po przetworzeniu (najprawdopodobniej bezwzglêdna
#   path cie¿ka w której powinny znajdowaæ siê pliki wejsciowe
function(SET_PROJECT_SOURCE_GROUP name files)

	string(REPLACE "\\" "\\\\" _fixedName "${name}")
	string(REPLACE "/" "\\\\" _fixedName "${name}")

	source_group("${_fixedName}" FILES ${files})

endfunction(SET_PROJECT_SOURCE_GROUP)

###############################################################################

# Makro tworz¹ce prawdziwe cie¿ki plików
# Parametry
#	relIn Lista wejciowa z nazwami plików do przetworzenia wzglêdna dla path
#	out Lista wyjciowa z cie¿kami plików po przetworzeniu (najprawdopodobniej bezwzglêdna
#   path cie¿ka w której powinny znajdowaæ siê pliki wejsciowe
macro(SET_PROJECT_ADDITIONAL_INSTALLS)

	if(DEFINED PROJECT_ADDITIONAL_INSTALLS_SET)
		TARGET_NOTIFY(PROJECT_ADDITIONAL_INSTALLS "Dodatkowe instalacje projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_PROJECT_ADDITIONAL_INSTALLS mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam dodatkowe pliki instalacji ${ARGN}")
	else()
		set(PROJECT_ADDITIONAL_INSTALLS_SET 1)		
		set(PROJECT_ADDITIONAL_INSTALLS ${ARGN})		
	endif()

endmacro(SET_PROJECT_ADDITIONAL_INSTALLS)


###############################################################################

# Makro tworz¹ce prawdziwe cie¿ki plików
# Parametry
#	relIn Lista wejciowa z nazwami plików do przetworzenia wzglêdna dla path
#	out Lista wyjciowa z cie¿kami plików po przetworzeniu (najprawdopodobniej bezwzglêdna
#   path cie¿ka w której powinny znajdowaæ siê pliki wejsciowe
macro(GENERATE_FILE_PATHS relIn out path)

	set(${out} "")
	# ustalamy poprawn¹ cie¿kê do pliku wzglêdem aktualnego katalogu
	foreach(value ${${relIn}})
		set(CANDIDATE_PATH "${path}/${value}")
		list(APPEND ${out} "${CANDIDATE_PATH}")		
	endforeach()

endmacro(GENERATE_FILE_PATHS)

###############################################################################

# Makro pozwalaj¹ce konfigurowaæ zale¿noci kolejnego projektu dodawanego przez ADD_PROJECT
# Parametry
#	name - Nazwa opcji
#	description - Tekstowy opis dzia³ania opcji
#	enabled - Czy domylnie opcja zaznaczona czy nie
#	deps - lista zale¿noci gdy opcja zaznaczona
macro(ADD_PROJECT_VARIABLE_DEPENDENCIES name description enabled deps)
	
	ADD_PROJECT_VARIABLE_DEPENDENCIES_EXT(${name} "${description}" ${enabled} "${deps}" "")
	
endmacro(ADD_PROJECT_VARIABLE_DEPENDENCIES)

###############################################################################

# Makro pozwalaj¹ce konfigurowaæ zale¿noci kolejnego projektu dodawanego przez ADD_PROJECT
# Parametry
#	name - Nazwa opcji
#	description - Tekstowy opis dzia³ania opcji
#	enabled - Czy domylnie opcja zaznaczona czy nie
#	depsON - lista zale¿noci gdy opcja zaznaczona
#	depsOFF - lista zale¿noci gdy opcja odznaczona
macro(ADD_PROJECT_VARIABLE_DEPENDENCIES_EXT name description enabled depsON depsOFF)
	# generujê opcjê
	CONFIG_OPTION("${name}" "${description}" ${enabled})
	
	_SETUP_INTERNAL_CACHE_VALUE(${name} ${CONFIG_${name}} "Wartosc zmiennej konfiguracyjnej ${name}")
	
	CONFIGURE_PROJECT_DEPENDENCIES_EXT(${name} "${depsON}" "${depsOFF}")
	
endmacro(ADD_PROJECT_VARIABLE_DEPENDENCIES_EXT)

###############################################################################

# Makro pozwalaj¹ce konfigurowaæ zale¿noci kolejnego projektu dodawanego przez ADD_PROJECT
# Parametry
#	name - Nazwa opcji
#	description - Tekstowy opis dzia³ania opcji
#	enabled - Czy domylnie opcja zaznaczona czy nie
#	depsON - lista zale¿noci gdy opcje zaznaczona
#	depsOFF - lista zale¿noci gdy conajmniej jedna z opcji odznaczona
macro(CONFIGURE_PROJECT_DEPENDENCIES_EXT variables depsON depsOFF)
	
	set(state 1)
	
	# sprawdzam czy moge dodaæ zale¿noci
	foreach(v ${variables})
		if(DEFINED ${v})
			if(NOT ${v})
				set(state 0)
			endif()
		else()
			set(state 0)
		endif()
	endforeach()
	
	# wstepnie zak¹³dam ¿e wszystko jest ok
	set(deps ${depsON})
	
	# jeli jednak nie by³o musze zmieniæ listy
	if(NOT state)
		set(deps ${depsOFF})
	endif()
	
	# rozszerzam listê zale¿noci o konfigurowalne
	list(APPEND PROJECT_CONFIGURABLE_DEPENDENCIES ${deps})
	# mankament CMake - brak mo¿liwoci mapowania listy do listy, trzeba wprowadziæ dodatkow¹ zmienn¹
	
	if(NOT DEFINED PROJECT_DEPENDENCIES_CONFIG_ID)
		set(PROJECT_DEPENDENCIES_CONFIG_ID 0)
	endif()
	
	set(PROJECT_DEPENDENCIES_CONFIG_${PROJECT_DEPENDENCIES_CONFIG_ID}_VARIABLES ${variables})
	set(PROJECT_DEPENDENCIES_CONFIG_${PROJECT_DEPENDENCIES_CONFIG_ID}_DEPS_ON ${depsON})
	set(PROJECT_DEPENDENCIES_CONFIG_${PROJECT_DEPENDENCIES_CONFIG_ID}_DEPS_OFF ${depsOFF})
	# podwy¿szam id kolejnej konfiguracji
	math(EXPR PROJECT_DEPENDENCIES_CONFIG_ID "${PROJECT_DEPENDENCIES_CONFIG_ID} + 1")
	
endmacro(CONFIGURE_PROJECT_DEPENDENCIES_EXT)

###############################################################################

# Makro pozwalaj¹ce konfigurowaæ zale¿noci kolejnego projektu dodawanego przez ADD_PROJECT
# Parametry
#	name - Nazwa opcji
#	description - Tekstowy opis dzia³ania opcji
#	enabled - Czy domylnie opcja zaznaczona czy nie
#	depsON - lista zale¿noci gdy opcje zaznaczona
macro(CONFIGURE_PROJECT_DEPENDENCIES variables deps)
	
	CONFIGURE_PROJECT_DEPENDENCIES_EXT("${variables}" "${deps}" "")
	
endmacro(CONFIGURE_PROJECT_DEPENDENCIES)

###############################################################################

# Makro dodaj¹ce projekt
# Parametry
#	name Nazwa projektu
#	dodatkowy parametr dependencies Lista zale¿noci
#   dodatkowy parametr za dependencies to string ze cie¿k¹ do projektu
#   dodatkowy parametr to info czy faktycznie projekt dodajemy czy nie
macro(ADD_PROJECT name)

	# sprawdzamy czy faktycznie robimy co z projektem
	set(PROCESS_PROJECT 1)
	if(${ARGC} GREATER 3)	
		if(NOT ${${ARGV3}})			
			set(PROCESS_PROJECT 0)
		endif()
	endif()
	
	# je¿eli faktycznie przetwarzamy
	if(${PROCESS_PROJECT})		
		# szukam czy nie ma ju¿ projektu o takiej nazwie!
		list(FIND SOLUTION_PROJECTS ${name} PROJECT_EXISTS)
		if(PROJECT_EXISTS GREATER -1)
			TARGET_NOTIFY(name "Project with name ${name} already exists! Project names must be unique! Skipping this project.")
		else()			
			set(PROJECT_DEPENDENCIES "")
			# ustawiam projekt do póniejszej konfiguracji
			_APPEND_INTERNAL_CACHE_VALUE(SOLUTION_PROJECTS ${name} "All projects to configure")			
			# je¿eli sa dodatkowe zale¿noci
			if(PROJECT_CONFIGURABLE_DEPENDENCIES)
				list(APPEND PROJECT_DEPENDENCIES "${PROJECT_CONFIGURABLE_DEPENDENCIES}")
			endif()
			# zapamiêtujemy ile konfiguracji zale¿noci dla projektu
			_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_DEPENDENCIES_CONFIGURATIONS_SIZE "${PROJECT_DEPENDENCIES_CONFIG_ID}" "Project ${name} dependencies configurations count")
			
			set(_idx 0)
			# przepisujê i zapamiêtujê sobie konfigurowalne zaleznoci
			while(${PROJECT_DEPENDENCIES_CONFIG_ID} GREATER _idx)
			
				_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_DEPENDENCIES_CONFIG_${_idx}_VARIABLES "${PROJECT_DEPENDENCIES_CONFIG_${_idx}_VARIABLES}"  "Project ${name} configurable dependecies ${_idx} variables")
				_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_DEPENDENCIES_CONFIG_${_idx}_DEPS_ON "${PROJECT_DEPENDENCIES_CONFIG_${_idx}_DEPS_ON}" "Project ${name} configurable dependecies ${_idx} details when ON")
				_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_DEPENDENCIES_CONFIG_${_idx}_DEPS_OFF "${PROJECT_DEPENDENCIES_CONFIG_${_idx}_DEPS_OFF}" "Project ${name} configurable dependecies ${_idx} details when OFF")
				
				math(EXPR _idx "${_idx} + 1")
			
			endwhile()
			
			# dopisuje dodatkowe zale¿noci wspólne, bez wzglêdu na konfiguracje
			if(${ARGC} GREATER 1)
				list(APPEND PROJECT_DEPENDENCIES ${ARGV1})
				# ustawiam globalne zale¿noci do szukania
			endif()			
			
			# aktualizuje globalnie wszystkie zale¿noci wszystkich projektów
			_APPEND_INTERNAL_CACHE_VALUE(SOLUTION_DEPENDENCIES "${PROJECT_DEPENDENCIES}" "Solution all dependencies")

			#usuwam duplikaty z listy zależności
			list(REMOVE_DUPLICATES SOLUTION_DEPENDENCIES)	
			
			# dopisujê dodatkowe zale¿noci dla ca³ej solucji
			if(DEFINED SOLUTION_DEFAULT_DEPENDENCIES)
				list(APPEND PROJECT_DEPENDENCIES ${SOLUTION_DEFAULT_DEPENDENCIES})
			endif()
			
			#usuwam duplikaty z listy zależności
			list(REMOVE_DUPLICATES PROJECT_DEPENDENCIES)
			
			# ustawiam zale¿noci projektu
			_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_DEPENDENCIES "${PROJECT_DEPENDENCIES}" "Project ${name} dependencies")			
			
			# ustawiam cie¿kê projektu
			_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_PATH "${CMAKE_CURRENT_LIST_DIR}/${name}" "Project ${name} path")
			_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_RELATIVE_PATH "${name}" "Project ${name} relative path")
			
			# je¿eli podano extra cie¿kê do projektu
			if(${ARGC} GREATER 2)
				# ustawiam cie¿kê do projektu
				_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_PATH "${CMAKE_CURRENT_LIST_DIR}/${ARGV2}" "Project ${name} path")
				_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_RELATIVE_PATH "${ARGV2}" "Project ${name} relative path")
			endif()			
			
			# ustawiam grupê projektu
			_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_GROUP "" "Project ${name} group name")
			
			if(DEFINED CURRENT_PROJECT_GROUP_NAME)
				_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_GROUP ${CURRENT_PROJECT_GROUP_NAME} "Project ${name} group name")
			endif()
			
			# wstêpnie zak³adam ¿e nie uda³o mi siê skonfigurowaæ projektu
			_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_INITIALISED 0 "Helper telling if project was initialised properly")
		endif()		
	else()
		TARGET_NOTIFY(name "Pomijam projekt ${name}")
	endif()
	
	# resetuje zale¿noci konfiguracyjne
	set(PROJECT_CONFIGURABLE_DEPENDENCIES "")
	set(PROJECT_DEPENDENCIES_CONFIG_ID 0)
endmacro(ADD_PROJECT)

###############################################################################

# Makro inicjalizuj¹ce dany projekt - szuka jego zale¿noci, jeli wszystkie znajdzie to faktycznie konfuguruje projekt
# Wywo³uje siê rekurencyjnie dla naszych projektów aby poprawnie ledziæ/ustawiæ ich zale¿noci i zagwarantowaæ dobr¹ kolejnoæ
# ich inicjalizacji

# Parametry
#	name Nazwa projektu
macro(__INITIALIZE_PROJECT name)	

	if(DEFINED PROJECT_ADD_FINISHED)
		if(${PROJECT_ADD_FINISHED} EQUAL 0)
			# b³¹d konfigurowania projektu - rozpoczêlismy ale nie by³o makra PROJECT_END()!!!
			message(SEND_ERROR "B³¹d konfiguracji: Rozpoczêto konfiguracjê projektu ${PROJECT_NAME} ale nie zakoñczono jej makrem END_PROJECT()")
		endif()
	else()
		# info ¿e rozpoczynamy dodawanie nowego projektu ale jeszcze go nie zamknêlismy makrem END_PROJECT()	
		set(PROJECT_ADD_FINISHED 0)
	endif()
	
	set(ADD_PROJECT_${name}_FAILED 0)
	set(ADD_PROJECT_${name}_MESSAGE)
	
	# publiczne includy	
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_INCLUDE_DIRS "" "Sciezka do includów projektu ${name}")
	# definy tego projektu i projektów + bibliotek od których jest zale¿ny
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_COMPILER_DEFINITIONS "" "Definicje kompilatora projektu ${name}")
	# resetujemy typ projektu
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_TYPE "" "Typ projektu ${name}")
	# nazwa artefaktu projektu
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_TARGETNAME "${name}" "Nazwa artefaktu projektu ${name}")
	
	# aktualizujemy liste aktualnie przetwarzanych projektów
	list(APPEND PROJECTS_BEING_INITIALISED ${name})
	
	# czy dodawanie projektu zakoñczy³o siê b³êdem
	set(PROJECT_${name}_FAILED 0)
	# wiadomoæ projektu
	set(PROJECT_${name}_MESSAGE "")	
	# resetujemy listê zale¿noci
	set(tmp_${name}_dependencies "")
	list(APPEND tmp_${name}_dependencies ${PROJECT_${name}_DEPENDENCIES})
	list(REMOVE_DUPLICATES tmp_${name}_dependencies)
	set(${name}_gain)
	# czy podano zale¿noci
	list(LENGTH tmp_${name}_dependencies depLength)
	if(${depLength} GREATER 0)				
		# szukamy wszystkich brakuj¹cych zale¿noci
		foreach (value ${tmp_${name}_dependencies})
			# sprawdzamy czy przypadkiem zale¿noæ nie jest naszym projektem!				
			list(FIND SOLUTION_PROJECTS ${value} DEPENDENCY_IS_PROJECT)
			if(DEPENDENCY_IS_PROJECT GREATER -1)
				# jest naszym projektem wiêc sprawdzam czy czasem z jego powodu siê nie inicjujemy
				# jeli tak to mamy cykliczn¹ zale¿noc projektów co jest niedopuszczalne
				list(FIND PROJECTS_BEING_INITIALISED ${value} PROJECT_IS_BEING_INITIALISED)
				if(${PROJECT_IS_BEING_INITIALISED} GREATER -1)
					# b³¹d konfigurowania projektu? - rekurencyjne zale¿noci pomiêdzy projektami!!!
					
					# TODO - zdecydowaæ czy dopuszczamy rekurencyjne zale¿noci projektów (z³a praktyka!!)
					# set(ADD_PROJECT_FAILED 1)
					# set(PROJECT_${name}_MESSAGE "Project ${value} has circular dependency with project ${name}, " ${PROJECT_${name}_MESSAGE})
										
					TARGET_NOTIFY(PROJECT_IS_BEING_INITIALISED "Projects ${name} and ${value} have circular dependency! Think about refactoring")
				else()
					# jest naszym projektem wiêc sprawdzam czy byl ju¿ inicjowany, jeli tak to ok
					list(FIND INITIALISED_PROJECTS ${value} PROJECT_IS_INITIALISED)
					if(${PROJECT_IS_INITIALISED} EQUAL -1)
						# projekt nie by³ jeszcze inicjowany
						# inicjujemy go teraz
						__INITIALIZE_PROJECT(${value})
					endif()
					# czy projekt uda³o sie poprawnie zainicjowaæ?
					if(PROJECT_${value}_INITIALISED EQUAL 0)
						set(ADD_PROJECT_${name}_FAILED 1)
						set(ADD_PROJECT_${name}_MESSAGE ${value} ", " ${ADD_PROJECT_${name}_MESSAGE})
					endif()
				endif()
			elseif(NOT LIBRARY_${value}_FOUND)
				# jest to zale¿noæ ale jej nie znaleziono
				set(ADD_PROJECT_${name}_FAILED 1)
				set(ADD_PROJECT_${name}_MESSAGE ${value} ", " ${ADD_PROJECT_${name}_MESSAGE})
				TARGET_NOTIFY(${name} "${value} not found")
			endif()
		endforeach()
		
		list(APPEND tmp_${name}_dependencies ${${name}_gain})
		list(REMOVE_DUPLICATES tmp_${name}_dependencies)
		
		# nadpisujemy ze wzglêdu na wszytkie zale¿noci - podane jawnie i ich niejawne zale¿noci wynikaj¹ce z finderów
		_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${name}_DEPENDENCIES "${tmp_${name}_dependencies}" "Project ${name} dependencies")
	endif()
	
	# usuwam projekt z aktualnie inicjowanych
	list(REMOVE_ITEM PROJECTS_BEING_INITIALISED ${name})	
	
	# zapamiêtujê ¿e projekt ju¿ by³ inicjowany
	_APPEND_INTERNAL_CACHE_VALUE(INITIALISED_PROJECTS ${name} "Helper list with already initialised projects")
	
	# sprawdzamy
	if (ADD_PROJECT_${name}_FAILED)
		# brakuje zale¿noci - wywietlamy komunikat
		message("${name} not included because dependencies are missing: ${ADD_PROJECT_${name}_MESSAGE}")
	else()
	
		# faktycznie probujemy dodawac projekt - znalelismy wszystkie zale¿noci
		# dalej konfigurujemy projekt
		set(CURRENT_PROJECT_NAME ${name})
		add_subdirectory("${PROJECT_${name}_PATH}")
		
		# uda³o nam siê poprawnie skonfigurowaæ projekt
		_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${value}_INITIALISED 1 "Helper telling if project was initialised properly")
	endif()

endmacro(__INITIALIZE_PROJECT)

###############################################################################

# Makro dodaj¹ce projekt testu
# Parametry
#	name Nazwa projektu
#	dependencies Lista zale¿noci
#   dodatkowy parametr za dependencies to string ze cie¿k¹ do projektu testu wzglêdem katalogu tests
#   dodatkowy parametr to info czy faktycznie projekt dodajemy czy nie
macro(ADD_TEST_PROJECT name dependencies)
	
	set(newDependecies ${dependencies})
	
	# rozszerzamy zale¿noci o bilbioteki potrzebne dla testów
	if(DEFINED TESTS_DEPENDENCIES)
		set(newDependecies ${newDependecies} ${TESTS_DEPENDENCIES})		
	endif()
	
	set(PROJECT_${name}_test_IS_TEST 1 PARENT_SCOPE)
	ADD_PROJECT("${name}_test" "${newDependecies}" ${ARGN})
	
endmacro(ADD_TEST_PROJECT)

###############################################################################

macro(ADD_PROJECTS dir)

	add_subdirectory(${dir})
	
endmacro(ADD_PROJECTS)

###############################################################################

# Makro weryfikuj¹ce typ projektu
# type Typ projektu:
# executable - plik wykonywalny
# static - biblioteka ³¹czona statycznie
# dynamic - biblioteka ³¹czona dynamicznie (z libk¹ do importu)
# module - biblioteka ³¹czona dynamicznie przez dll-open
macro(__VERIFY_PROJECT_TYPE type)

	if(NOT (${type} STREQUAL "executable" OR ${type} STREQUAL "static" OR ${type} STREQUAL "dynamic" OR ${type} STREQUAL "module" OR ${type} STREQUAL "header"))
		message(SEND_ERROR "Nieznany typ projektu dla ${PROJECT_NAME}. W³aciwa wartoæ to: executable, static, dynamic, module lub header")
	endif()

endmacro(__VERIFY_PROJECT_TYPE)

###############################################################################

# Makro rozpoczynaj¹ce konfiguracjê konkretnego projektu - powinno byæ wywo³ywane jako pierwsza komenda po ADD_PROJECT zaraz na pocz¹tku pliku konfiguracujnego projektu
# type Patrz makro VERIFY_PROJECT_TYPE
# opcjonalny argument to nazwa wyjciowa naszego artefaktu [dla release, dla debug dodajemy automatycznie d na koniec], domylnie jest to nazwa projektu
macro(BEGIN_PROJECT type)
	# weryfikujemy typ projektu
	__VERIFY_PROJECT_TYPE(${type})	
	#zapamietuje globalnie typ projektu aby pozniej go nie dodawaæ jako zale¿nego w przypadku execow
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_TYPE ${type} "Typ projektu ${CURRENT_PROJECT_NAME}")
	
	# jeli dodatkowy parametr to traktujemy go potencjalnie jako nazwê naszego artefaktu
	if(${ARGC} GREATER 1)
		# sprawdzam czy nazwa taka mo¿e byæ - czy nie jest pusta i czy nie mam ju¿ takiego artefaktu
		string(STRIP ${ARGV1} targetName)
		string(LENGTH ${targetName} targetNameLength)

		if(${targetNameLength} EQUAL 0)
			# pusta nazwa artefaktu - przywracamy domyslna nazwe projektu
			TARGET_NOTIFY(TARGET_TARGETNAME "Uwaga - podano pust¹ nazwê artefaktu dla projektu ${CURRENT_PROJECT_NAME}. Nazwa artefaktu zostaje ustawiona na nazwê projektu: ${TARGET_TARGETNAME}")
		elseif(DEFINED TARGET_NAME_${targetName})
			# zdefiniowano ju¿ tak¹ nazwê artefaktu - potencjalny problem, informuje o tym ale to nie jest krytyczne
			TARGET_NOTIFY(TARGET_TARGETNAME "Uwaga - arterfakt o podanej nazwie: ${targetName} zosta³ ju¿ zdefiniowany dla projektu ${TARGET_NAME_${targetName}}. Mo¿e to powodowaæ b³êdy przy budowie (nadpisywanie artefaktów ró¿nych projektów) i byæ myl¹ce. Zaleca siê stosowanie unikalnych nazw artefaktów.")
		else()
			# nazwa artefaktu wyglada ok
			TARGET_NOTIFY(TARGET_TARGETNAME "W³asna nazwa artefaktu: ${targetName} pomylnie przesz³a weryfikacjê")
			# aktualizujemy ja			
			_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME ${targetName} "Nazwa artefaktu projektu ${CURRENT_PROJECT_NAME}")	
		endif()
		
	endif()

	# je¿eli chemy plik wykonywalny i jestemy na platformie windows to mo¿emy wybraæ czy ma to byæ aplikacja z konsol¹ czy bez
	if(${type} STREQUAL "executable" AND WIN32)
		set(__OPT OFF)
		if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
			set(__OPT ON)
		endif()
		option(PROJECT_${CURRENT_PROJECT_NAME}_WIN32_ENABLE_CONSOLE "Enable console on Win32 for project ${CURRENT_PROJECT_NAME} on artifact ${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME}?" ${__OPT})
	endif()

	# ostrze¿enie jeli nasz projekt jest testowy a nie jest aplikacj¹
	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST AND NOT ${type} STREQUAL "executable")
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien byæ kompilowany do pliku wykonywalnego (typ executable) a nie byæ typu ${type}")
	endif()

	# resetujemy pliki projektowe
	_CLEAR_VARIABLES(
		PUBLIC_H
		PRIVATE_H
		SOURCE_FILES
		CONFIG_FILES
		UI_FILES
		MOC_FILES
		RC_FILES
		RESOURCE_FILES
		PRECOMPILED_H
		PRECOMPILED_SRC
		TRANSLATION_FILES
		DEPLOY_MODIFIABLE_RESOURCES_FILES
		CONFIGURE_PRIVATE_HEADER_FILES
		CONFIGURE_PUBLIC_HEADER_FILES
		PROJECT_PUBLIC_HEADER_PATH
		_GENERATE_PROJECT_TRANSLATIONS
		PROJECT_ADDITIONAL_INSTALLS
	)
	
	string(REPLACE "${SOLUTION_ROOT}/src" ${SOLUTION_INCLUDE_ROOT} PROJECT_PUBLIC_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
	
	set(PROJECT_TRANSLATIONS_FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lang")
	set(PROJECT_SOURCE_FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
	set(PROJECT_UI_FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ui")
	set(PROJECT_CONFIGURATION_FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/configuration")
	set(PROJECT_RESOURCES_FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources")
	set(PROJECT_DEPLOY_RESOURCES_FILES_PATH "${PROJECT_RESOURCES_FILES_PATH}/deploy")
	set(PROJECT_EMBEDDED_RESOURCES_FILES_PATH "${PROJECT_RESOURCES_FILES_PATH}/embedded")
	
	set(PROJECT_PUBLIC_CONFIGURATION_INCLUDES_PATH "${CMAKE_CURRENT_BINARY_DIR}/public_configure_include")
	set(PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH "${CMAKE_CURRENT_BINARY_DIR}/private_configure_include")
	
	set(PROJECT_CONFIGURATION_FILE_ID 0)	

endmacro(BEGIN_PROJECT)

###############################################################################

# Ustawiamy publiczne pliki nag³ówkowe
macro(SET_PUBLIC_HEADERS)	

	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien kompilowaæ siê do pliku wykonywalnego i raczej nie powinien posiadaæ nag³ówków publicznych.")
	endif()
	
	if(DEFINED PUBLIC_HEADERS_SET)
		TARGET_NOTIFY(PUBLIC_HEADERS_SET "Publiczne nag³ówki projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_PUBLIC_HEADERS mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam nag³ówki ${ARGN}")
	else()
		# zapamiêtujemy ¿e ju¿ by³a wo³ana ta metoda podczas konfiguracji aktualnego projektu
		set(PUBLIC_HEADERS_SET 1)
		# nag³ówki publiczne		
		set(PUBLIC_H_RELATIVE ${ARGN})
		GENERATE_FILE_PATHS(PUBLIC_H_RELATIVE PUBLIC_H "${PROJECT_PUBLIC_HEADER_PATH}")
		
		source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
	endif()

endmacro(SET_PUBLIC_HEADERS)

###############################################################################

# Ustawiamy prywatne pliki nag³ówkowe
macro(SET_PRIVATE_HEADERS)

	if(DEFINED PRIVATE_HEADERS_SET)
		TARGET_NOTIFY(PRIVATE_HEADERS_SET "Prywatne nag³ówki projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_PRIVATE_HEADERS mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam nag³ówki ${ARGN}")
	else()
		# zapamiêtujemy ¿e ju¿ by³a wo³ana ta metoda podczas konfiguracji aktualnego projektu
		set(PRIVATE_HEADERS_SET 1)		
		set(PRIVATE_H_RELATIVE ${ARGN})
		GENERATE_FILE_PATHS(PRIVATE_H_RELATIVE PRIVATE_H "${PROJECT_SOURCE_FILES_PATH}")
		
		source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})
	endif()

endmacro(SET_PRIVATE_HEADERS)

###############################################################################

# Ustawiamy pliki ród³owe
macro(SET_SOURCE_FILES)

	if(DEFINED SOURCE_FILES_SET)
		TARGET_NOTIFY(SOURCE_FILES_SET "Pliki ród³owe projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_SOURCE_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(SOURCE_FILES_SET 1)		
		set(SOURCE_FILES_RELATIVE ${ARGN})
		GENERATE_FILE_PATHS(SOURCE_FILES_RELATIVE SOURCE_FILES "${PROJECT_SOURCE_FILES_PATH}")
		source_group("${SOURCEGROUP_SOURCES}" FILES ${SOURCE_FILES})
	endif()

endmacro(SET_SOURCE_FILES)

###############################################################################

# Ustawiamy pliki UI
macro(SET_UI_FILES)

	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien kompilowaæ siê do pliku wykonywalnego konsolowego i raczej nie powinien posiadaæ UI")
	endif()

	if(DEFINED UI_FILES_SET)
		TARGET_NOTIFY(UI_FILES_SET "Pliki UI projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_UI_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(UI_FILES_SET 1)
		set(INPUT_FILES ${ARGN})
		GENERATE_FILE_PATHS(INPUT_FILES UI_FILES "${PROJECT_UI_FILES_PATH}")
		
		source_group("${SOURCEGROUP_UI}" FILES ${UI_FILES})
	endif()

endmacro(SET_UI_FILES)

###############################################################################

# Ustawiamy pliki MOC
macro(SET_MOC_FILES)	

	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien kompilowaæ siê do pliku wykonywalnego konsolowego i raczej nie powinien posiadaæ UI")
	endif()

	if(DEFINED MOC_FILES_SET)
		TARGET_NOTIFY(MOC_FILES_SET "Pliki MOC projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_MOC_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(MOC_FILES_SET 1)
		
		set(_MOC_ALL_SOURCES ${PUBLIC_H} ${PRIVATE_H} ${SOURCE_FILES} ${CONFIGURE_PUBLIC_HEADER_FILES} ${CONFIGURE_PRIVATE_HEADER_FILES})		
		
		# pliki te musz¹ siê znaleæ w którejæ z wersji nag³ówków prywatnych, publicznych, ród³ach lub konfiguracyjnych po konfiguracji
		set(MOC_FILES "")
		foreach(value ${ARGN})
			get_filename_component(_destName "${value}" NAME)
			set(_KEEP_TRYING 1)
		
			foreach(f ${_MOC_ALL_SOURCES})				
			
				if(_KEEP_TRYING EQUAL 1)
					get_filename_component(_srcName "${f}" NAME)
					if("${_srcName}" STREQUAL "${_destName}")
						string(FIND "${f}" "${value}" _idx)
						if(_idx GREATER -1)					
							set(_KEEP_TRYING 0)
							set(filePath "${f}")
						endif()
					endif()					
				endif()
				
			endforeach()			
		
			if(_KEEP_TRYING EQUAL 1)
				TARGET_NOTIFY(_KEEP_TRYING "Plik ${value} nie zosta³ zarejestrowany w projekcie ${CURRENT_PROJECT_NAME} a ma byæ przetwarzany przez MOC z QT. Zarejestruj plik do jednej z podstawowych grup: PUBLIC_HEADERS, PRIVATE_HEADERS, SOURCES, plikach po konfiguracji. Pomijam plik")
			else()
				list(APPEND MOC_FILES "${filePath}")
			endif()
		endforeach()		
	endif()

endmacro(SET_MOC_FILES)

###############################################################################

# Ustawiamy pliki RC
macro(SET_RC_FILES)

	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien kompilowaæ siê do pliku wykonywalnego konsolowego i raczej nie powinien posiadaæ UI")
	endif()

	if(DEFINED RC_FILES_SET)
		TARGET_NOTIFY(RC_FILES_SET "Pliki RC projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_RC_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(RC_FILES_SET 1)
		set(INPUT_FILES ${ARGN})
		GENERATE_FILE_PATHS(INPUT_FILES RC_FILES "${PROJECT_UI_FILES_PATH}")
		
		source_group("${SOURCEGROUP_UI}" FILES ${RC_FILES})
	endif()

endmacro(SET_RC_FILES)

###############################################################################

# Ustawiamy pliki t³umaczeñ
macro(SET_TRANSLATION_FILES)

	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien kompilowaæ siê do pliku wykonywalnego konsolowego i raczej nie powinien posiadaæ UI")
	endif()

	if(DEFINED TRANSLATION_FILES_SET)
		TARGET_NOTIFY(TRANSLATION_FILES_SET "Pliki translacji projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_TRANSLATION_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(TRANSLATION_FILES_SET 1)
		set(INPUT_FILES ${ARGN})
		GENERATE_FILE_PATHS(INPUT_FILES TRANSLATION_FILES "${PROJECT_TRANSLATIONS_FILES_PATH}")

		if(NOT EXISTS "${PROJECT_TRANSLATIONS_FILES_PATH}")
			TARGET_NOTIFY(PROJECT_TRANSLATIONS_FILES_PATH "Katalog ${PROJECT_TRANSLATIONS_FILES_PATH} nie istnieje choæ wskazano pliki t³umaczeñ ${ARGN}. Tworzê podany katalog.")
			file(MAKE_DIRECTORY "${PROJECT_TRANSLATIONS_FILES_PATH}")
		endif()
		
		foreach(f ${TRANSLATION_FILES})
		
			if(NOT EXISTS "${f}")
			
				TARGET_NOTIFY(f "Plik ${f} nie istnieje choæ wskazano go jako plik t³umaczeñ. Tworze pusty plik")
				file(WRITE "${f}" "")
				
			endif()
		
		endforeach()
		
		source_group("${SOURCEGROUP_TRANSLATIONS}" FILES ${TRANSLATION_FILES})
	endif()

endmacro(SET_TRANSLATION_FILES)

###############################################################################

# W³¹cza generowanie t³umaczeñ dla projektu

macro(GENERATE_PROJECT_TRANSLATIONS)

	list(LENGTH SOLUTION_TRANSLATION_LANGUAGES stlLength)
	if(${stlLength} GREATER 0)
		set(_GENERATE_PROJECT_TRANSLATIONS 1)
	endif()

endmacro()

###############################################################################

# Ustawiamy pliki wbudowanych zasobów
macro(SET_EMBEDDED_RESOURCES)

	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien kompilowaæ siê do pliku wykonywalnego konsolowego i raczej nie powinien posiadaæ UI")
	endif()

	if(DEFINED EMBEDDED_RESOURCES_SET)
		TARGET_NOTIFY(EMBEDDED_RESOURCES_SET "Pliki wbudowanych zasobów projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_EMBEDDED_RESOURCES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(EMBEDDED_RESOURCES_SET 1)
		set(INPUT_FILES ${ARGN})
		GENERATE_FILE_PATHS(INPUT_FILES EMBEDDED_RESOURCES_FILES "${PROJECT_EMBEDDED_RESOURCES_FILES_PATH}")
		
		source_group("${SOURCEGROUP_RESOURCES}\\${SOURCEGROUP_EMBEDDED_RESOURCES}" FILES ${EMBEDDED_RESOURCES_FILES})
	endif()

endmacro(SET_EMBEDDED_RESOURCES)

# Ustawiamy pliki dostarczanych zasobów
macro(SET_DEPLOY_RESOURCES)

	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien kompilowaæ siê do pliku wykonywalnego konsolowego i raczej nie powinien posiadaæ UI")
	endif()

	if(DEFINED DEPLOY_RESOURCES_SET)
		TARGET_NOTIFY(DEPLOY_RESOURCES_SET "Pliki dostarczanych zasobów projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_DEPLOY_RESOURCES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(DEPLOY_RESOURCES_SET 1)
		set(INPUT_FILES ${ARGN})
		GENERATE_FILE_PATHS(INPUT_FILES DEPLOY_RESOURCES_FILES "${PROJECT_DEPLOY_RESOURCES_FILES_PATH}")
		
		source_group("${SOURCEGROUP_RESOURCES}\\${SOURCEGROUP_DEPLOY_RESOURCES}" FILES ${DEPLOY_RESOURCES_FILES})
	endif()

endmacro(SET_DEPLOY_RESOURCES)

###############################################################################

# Oznacza które pliki zasobów dostarczanych mog¹ byæ modyfikowane - instalatory
# powinny braæ to pod uwagê i umieszczaæ je w stosownych miejscach (np. AppData dla NSIS i windows)
macro(MARK_DEPLOY_RESOURCES_AS_MODIFIABLE)

	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		TARGET_NOTIFY(PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST "Projekt ${CURRENT_PROJECT_NAME} jest projektem testowym. Powinien kompilowaæ siê do pliku wykonywalnego konsolowego i raczej nie powinien posiadaæ UI")
	endif()

	if(DEFINED MARK_DEPLOY_RESOURCES_AS_MODIFIABLE_SET)
		TARGET_NOTIFY(MARK_DEPLOY_RESOURCES_AS_MODIFIABLE_SET "Pliki dostarczanych zasobów projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_DEPLOY_RESOURCES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(MARK_DEPLOY_RESOURCES_AS_MODIFIABLE_SET 1)
		set(DEPLOY_MODIFIABLE_RESOURCES_FILES "${ARGN}")
	endif()

endmacro(MARK_DEPLOY_RESOURCES_AS_MODIFIABLE)

###############################################################################

# Ustawiamy pliki konfiguracyjne (niekoniecznie CMake musi je potem przetwarzac, ale inne nie bêd¹ mogly byc przetwarzane)
macro(SET_CONFIGURATION_INPUT_FILES)

	if(DEFINED CONFIGURATION_INPUT_FILES_SET)
		TARGET_NOTIFY(CONFIGURATION_INPUT_FILES_SET "Pliki konfiguracyjne projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_CONFIGURATION_INPUT_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(CONFIGURATION_INPUT_FILES_SET 1)
		set(INPUT_FILES ${ARGN})
		GENERATE_FILE_PATHS(INPUT_FILES CONFIGURATION_INPUT_FILES "${PROJECT_CONFIGURATION_FILES_PATH}")
		source_group("${SOURCEGROUP_CONFIGURATION_TEMPLATE_FILES}" FILES ${CONFIGURATION_INPUT_FILES})
	endif()

endmacro(SET_CONFIGURATION_INPUT_FILES)

###############################################################################

# Ustawiamy pliki konfiguracyjne (niekoniecznie CMake musi je potem przetwarzac, ale inne nie bêd¹ mogly byc przetwarzane)
macro(SET_CONFIGURATION_OUTPUT_FILES)

	if(DEFINED CONFIGURATION_OUTPUT_FILES_SET)
		TARGET_NOTIFY(CONFIGURATION_OUTPUT_FILES_SET "Pliki konfiguracyjne projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_CONFIGURATION_OUTPUT_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(CONFIGURATION_OUTPUT_FILES_SET 1)
		set(CONFIGURATION_OUTPUT_FILES ${ARGN})
		source_group("${SOURCEGROUP_CONFIGURATION_INSTANCE_FILES}" FILES ${CONFIGURATION_OUTPUT_FILES})
	endif()

endmacro(SET_CONFIGURATION_OUTPUT_FILES)

###############################################################################

# Ustawiamy pliki resources
macro(SET_DEPLOY_RESOURCES_FILES)

	if(DEFINED DEPLOY_RESOURCES_FILES_SET)
		TARGET_NOTIFY(DEPLOY_RESOURCES_FILES_SET "Pliki resources projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_DEPLOY_RESOURCES_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(DEPLOY_RESOURCES_FILES_SET 1)
		set(INPUT_FILES ${ARGN})
		GENERATE_FILE_PATHS(INPUT_FILES DEPLOY_RESOURCES_FILES "${PROJECT_DEPLOY_RESOURCES_FILES_PATH}")
		source_group("${SOURCEGROUP_RESOURCES_FILES}/deploy" FILES ${DEPLOY_RESOURCES_FILES})
	endif()

endmacro(SET_DEPLOY_RESOURCES_FILES)

###############################################################################

# Ustawiamy pliki resources
macro(SET_EMBEDDED_RESOURCES_FILES)

	if(DEFINED EMBEDDED_RESOURCES_FILES_SET)
		TARGET_NOTIFY(EMBEDDED_RESOURCES_FILES_SET "Pliki resources projektu ${CURRENT_PROJECT_NAME} zosta³y ju¿ ustawione. Makro SET_EMBEDDED_RESOURCES_FILES mo¿e byæ u¿yte tylko raz podczas konfiguracji projektu. Pomijam pliki ${ARGN}")
	else()
		set(EMBEDDED_RESOURCES_FILES_SET 1)
		set(INPUT_FILES ${ARGN})
		GENERATE_FILE_PATHS(INPUT_FILES EMBEDDED_RESOURCES_FILES "${PROJECT_EMBEDDED_RESOURCES_FILES_PATH}")
		source_group("${SOURCEGROUP_RESOURCES_FILES}/embedded" FILES ${EMBEDDED_RESOURCES_FILES})
	endif()

endmacro(SET_EMBEDDED_RESOURCES_FILES)

###############################################################################

# Konfigurujemy publiczne pliki
macro(CONFIGURE_PUBLIC_HEADER inFile outFile)
	
	set(f "${PROJECT_CONFIGURATION_FILES_PATH}/${inFile}")
	
	set(CONFIG_FOUND 0)
	list(FIND CONFIGURATION_INPUT_FILES ${f} CONFIG_FOUND)
	if(CONFIG_FOUND GREATER -1)
		set(OUTPUT_PATH "${PROJECT_PUBLIC_CONFIGURATION_INCLUDES_PATH}/${CURRENT_PROJECT_NAME}/${outFile}")
		configure_file("${f}" "${OUTPUT_PATH}")
		list(APPEND CONFIGURE_PUBLIC_HEADER_FILES "${OUTPUT_PATH}")
	else()
		TARGET_NOTIFY(inFile "Plik ${inFile} nie zosta³ zarejestrowany w projekcie ${CURRENT_PROJECT_NAME} jako typ pliku konfiguracyjnego a ma byæ konfigurowany przez CMake. Zarejestruj plik wsród plików konfiguracyjnych makrem SET_CONFIGURATION_FILES. Pomijam plik")
	endif()

endmacro(CONFIGURE_PUBLIC_HEADER)

###############################################################################

# Konfigurujemy prywatne pliki
macro(CONFIGURE_PRIVATE_HEADER inFile outFile)
	
	set(f "${PROJECT_CONFIGURATION_FILES_PATH}/${inFile}")
	
	set(CONFIG_FOUND 0)
	list(FIND CONFIGURATION_INPUT_FILES ${f} CONFIG_FOUND)
	if(CONFIG_FOUND GREATER -1)
		configure_file("${f}" "${PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH}/${outFile}")
		list(APPEND CONFIGURE_PRIVATE_HEADER_FILES "${PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH}/${outFile}")
	else()
		TARGET_NOTIFY(inFile "Plik ${inFile} nie zosta³ zarejestrowany w projekcie ${CURRENT_PROJECT_NAME} jako typ pliku konfiguracyjnego a ma byæ konfigurowany przez CMake. Zarejestruj plik wsród plików konfiguracyjnych makrem SET_CONFIGURATION_FILES. Pomijam plik")
	endif()	

endmacro(CONFIGURE_PRIVATE_HEADER)

###############################################################################

# Makro ustawiajace naglowki prekompilowane
# Parametry:
	# header Nag³ówek precompilowany
	# source Plik ród³owy na którym kompilujemy nag³ówek
	
MACRO(SET_PRECOMPILED_HEADER header source)

	set(PRECOMPILED_FOUND 1)
	set(HEADER_FOUND 1)
	set(SOURCE_FOUND 1)

	list(FIND CONFIGURE_PRIVATE_HEADER_FILES "${PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH}/${header}" HEADER_FOUND)
	
	if(HEADER_FOUND EQUAL -1)		
		list(FIND PRIVATE_H "${PROJECT_SOURCE_FILES_PATH}/${header}" HEADER_FOUND)
		
		if(HEADER_FOUND EQUAL -1)
			set(HEADER_FOUND 0)
		else()
			set(HEADER_FOUND 1)
			set(HEADER_CANDIDATE "${PROJECT_SOURCE_FILES_PATH}/${header}")
		endif()
		
	else()
		set(HEADER_FOUND 1)
		set(HEADER_CANDIDATE "${PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH}/${header}")
	endif()
	
	if(HEADER_FOUND EQUAL 0)
		TARGET_NOTIFY(HEADER_FOUND "Proba dodania pliku ${header} niezarejestrowanego w projekcie ${CURRENT_PROJECT_NAME} jako nag³owka prekompilowanego. Zarejestruj plik w projekcie do jednej z podstawowych grup a potem ustaw go jako nag³owek prekompilowany. Pomijam prekompilowane nag³owki.")
		set(PRECOMPILED_FOUND 0)
	endif()
	
	list(FIND SOURCE_FILES "${PROJECT_SOURCE_FILES_PATH}/${source}" SOURCE_FOUND)
		
	if(SOURCE_FOUND EQUAL -1)
		set(SOURCE_FOUND 0)
	else()
		set(SOURCE_FOUND 1)
		set(SOURCE_CANDIDATE "${PROJECT_SOURCE_FILES_PATH}/${source}")
	endif()
	
	if(SOURCE_FOUND EQUAL 0)
		TARGET_NOTIFY(SOURCE_FOUND "Proba dodania pliku ${source} niezarejestrowanego w projekcie ${CURRENT_PROJECT_NAME} jako nag³owka prekompilowanego. Zarejestruj plik w projekcie do jednej z podstawowych grup a potem ustaw go jako nag³owek prekompilowany. Pomijam prekompilowane nag³owki.")
		set(PRECOMPILED_FOUND 0)
	endif()
	
	if(${PRECOMPILED_FOUND} EQUAL 1)
		set(PRECOMPILED_H ${HEADER_CANDIDATE})
		set(PRECOMPILED_SRC ${SOURCE_CANDIDATE})
	endif()
	
ENDMACRO(SET_PRECOMPILED_HEADER)

################################################################################

# Pomaga odtwarzaæ hierarchiê plików drzewa róde³ w IDE
# Parametry:
#		files - zbiór plików
#		path - cie¿ka wzglêdem której generujê hierarchiê dla IDE
#		sourceGroup - grupa pod któr¹ mamy odbudowaæ hierarchiê
macro(_REBUILD_SCM_STRUCTURE_IN_IDE files path sourceGroup)
	
	foreach(f ${files})
		
		file(RELATIVE_PATH _path "${path}" "${f}")
		get_filename_component(_relPath "${_path}" PATH)
		string(FIND "${_relPath}" "./" _idx)
		
		if(_idx EQUAL 0)
			SET_PROJECT_SOURCE_GROUP("${sourceGroup}" "${f}")
			#source_group("${sourceGroup}" FILES "${f}")
		else()		
			#source_group("${sourceGroup}//${_fixedRelPath}" FILES "${f}")
			SET_PROJECT_SOURCE_GROUP("${sourceGroup}//${_fixedRelPath}" "${f}")
		endif()
			
	endforeach()
	
endmacro(_REBUILD_SCM_STRUCTURE_IN_IDE)

################################################################################

# Koñczymy dodawanie projektu
macro(END_PROJECT)

	set(TARGET_MOC_SRC)
	set(TARGET_UI_H)
	set(TARGET_RC_SRC)
	set(QM_OUTPUTS "")
	set(PROJECT_PUBLIC_INCLUDES "")
	set(PROJECT_PRIVATE_INCLUDES "")
	
	set(TARGET_H "")
	
	# wszystkie pliki nag³ówkowe
	if(NOT DEFINED PRIVATE_H)
	
		TARGET_NOTIFY(PRIVATE_H "Auto private headers generation")
		file(GLOB_RECURSE PRIVATE_H "${PROJECT_SOURCE_FILES_PATH}/*.h" "${PROJECT_SOURCE_FILES_PATH}/*.hh" "${PROJECT_SOURCE_FILES_PATH}/*.hpp")
		
		_REBUILD_SCM_STRUCTURE_IN_IDE("${PRIVATE_H}" "${PROJECT_SOURCE_FILES_PATH}" "${SOURCEGROUP_PRIVATE_HEADERS}")		
	
	endif()
	
	list(LENGTH PRIVATE_H privateHLength)
	
	if(privateHLength GREATER 0)
		list(APPEND TARGET_H ${PRIVATE_H})
		list(APPEND PROJECT_PRIVATE_INCLUDES ${PROJECT_SOURCE_FILES_PATH})
	endif()
	
	if(NOT DEFINED PUBLIC_H)
	
		TARGET_NOTIFY(PUBLIC_H "Auto public headers generation")
		file(GLOB_RECURSE PUBLIC_H "${PROJECT_PUBLIC_HEADER_PATH}/*.h" "${PROJECT_PUBLIC_HEADER_PATH}/*.hh" "${PROJECT_PUBLIC_HEADER_PATH}/*.hpp")

		_REBUILD_SCM_STRUCTURE_IN_IDE("${PUBLIC_H}" "${PROJECT_PUBLIC_HEADER_PATH}" "${SOURCEGROUP_PUBLIC_HEADERS}")
		
	endif()
	
	list(LENGTH PUBLIC_H publicHLength)	
	if(publicHLength GREATER 0)
		list(APPEND TARGET_H ${PUBLIC_H})
		list(APPEND PROJECT_PUBLIC_INCLUDES ${SOLUTION_INCLUDE_ROOT})
	endif()
	
	if(DEFINED CONFIGURATION_INPUT_FILES)
		list(APPEND TARGET_H ${CONFIGURATION_INPUT_FILES})
		
		if(DEFINED CONFIGURATION_OUTPUT_FILES)
			list(APPEND TARGET_H ${CONFIGURATION_OUTPUT_FILES})
		endif()
		
		if(DEFINED CONFIGURE_PRIVATE_HEADER_FILES)
			list(APPEND TARGET_H ${CONFIGURE_PRIVATE_HEADER_FILES})
			source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${CONFIGURE_PRIVATE_HEADER_FILES})			
			list(APPEND PROJECT_PRIVATE_INCLUDES ${PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH})
		endif()
		
		if(DEFINED CONFIGURE_PUBLIC_HEADER_FILES)
			list(APPEND TARGET_H ${CONFIGURE_PUBLIC_HEADER_FILES})
			source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${CONFIGURE_PUBLIC_HEADER_FILES})			
			list(APPEND PROJECT_PUBLIC_INCLUDES ${PROJECT_PUBLIC_CONFIGURATION_INCLUDES_PATH})
		endif()
		
	endif()
	
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_PUBLIC_HEADERS "${PUBLIC_H}" "Publiczne nag³ówki projektu ${CURRENT_PROJECT_NAME}")
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_CONFIGURABLE_PUBLIC_HEADERS "${CONFIGURE_PUBLIC_HEADER_FILES}" "Konfigurowalne publiczne nag³ówki projektu ${CURRENT_PROJECT_NAME}")
	
	if(NOT DEFINED SOURCE_FILES)
	
		TARGET_NOTIFY(SOURCE_FILES "Auto source files generation")
		file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_FILES_PATH}/*.c" "${PROJECT_SOURCE_FILES_PATH}/*.cpp" "${PROJECT_SOURCE_FILES_PATH}/*.cxx") 
		
		_REBUILD_SCM_STRUCTURE_IN_IDE("${SOURCE_FILES}" "${PROJECT_SOURCE_FILES_PATH}" "${SOURCEGROUP_SOURCES}")		
	
	endif()
	
	set(PROJECT_COMPILER_DEFINITIONS "")
	
	if(NOT ${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "header")	
		# nag³owki prekompilowane
		if(ENABLE_PRECOMPILED_HEADERS)
			if(DEFINED PRECOMPILED_H AND DEFINED PRECOMPILED_SRC)
				if(MSVC)
					# muszê odzyskaæ cie¿kê do precompiled header tak¹ jaka jest faktycznie includowana w PRECOMPILED_SRC
					# w przeciwnym wypadku bêd¹ b³êdy kompilacji typu:
					# error C2857: '#include' statement specified with the /Yc"${PRECOMPILED_H}" command-line option was not found in the source file
					# gdy¿ nazwy nie bêd¹ siê zgadza³y
					
					# sprawdzam czy nie mam precompiled header na nag³ówku publicznym
					
					list(FIND PUBLIC_H ${PRECOMPILED_H} _precompiledIDX)
					
					if(_precompiledIDX GREATER -1)
						file(RELATIVE_PATH PRECOMPILED_H_RELATIVE "${PROJECT_PUBLIC_HEADER_PATH}" "${PRECOMPILED_H}")				
					else()
						# w takim razie mo¿e na prywatnym?
						# TODO
						# co jesli tutaj tez nie znajedê? mo¿e jest konfigurowalny? te¿ w sumie tak mo¿e byæ!
						# trzeba dodaæ dalsze przeszukiwanie i rozszerzyæ makro próbuj¹ce ustawiæ prekompilowane nag³ówki
						list(FIND PRIVATE_H ${PRECOMPILED_H} _precompiledIDX)
						
						if(_precompiledIDX GREATER -1)
							file(RELATIVE_PATH PRECOMPILED_H_RELATIVE "${PROJECT_SOURCE_FILES_PATH}" "${PRECOMPILED_H}")				
						endif()
						
					endif()
					
					list(REMOVE_ITEM SOURCE_FILES "${PRECOMPILED_SRC}")			
					get_filename_component(_basename ${PRECOMPILED_H_RELATIVE} NAME_WE)
					set(_binary "${CMAKE_CURRENT_BINARY_DIR}/${_basename}.pch")			
					set_source_files_properties(${PRECOMPILED_SRC} PROPERTIES COMPILE_FLAGS "/Yc\"${PRECOMPILED_H_RELATIVE}\" /Fp\"${_binary}\"" OBJECT_OUTPUTS "${_binary}")
					set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_FLAGS "/Yu\"${_binary}\" /FI\"${_binary}\" /Fp\"${_binary}\"" OBJECT_DEPENDS "${_binary}")
					list(APPEND SOURCE_FILES "${PRECOMPILED_SRC}")			
				else()
					list(APPEND PROJECT_COMPILER_DEFINITIONS DISABLE_PRECOMPILED_HEADERS)
				endif()
			endif()
		else()
			list(APPEND PROJECT_COMPILER_DEFINITIONS DISABLE_PRECOMPILED_HEADERS)
		endif()
	endif()
	
	set(TARGET_SRC ${SOURCE_FILES} ${TARGET_OBJ})

	# sprawdzamy czy projekt nie jest zale¿ny od Qt
	if(DEFINED UI_FILES OR DEFINED MOC_FILES OR DEFINED RC_FILES OR DEFINED TRANSLATION_FILES)
		
		if(NOT DEFINED LIBRARY_QT_FOUND)
			message(FATAL_ERROR "Projekt jest zale¿ny od biblioteki Qt, której nie znaleziono lub która nie wystepuje wród zale¿noci. Wska¿ bibliotekê Qt lub dodaj j¹ jako zale¿noæ do projektu i przekonfiguruj solucjê CMake.")
		endif()
		
	endif()
	
	# UI
	if(DEFINED UI_FILES)
		list(LENGTH UI_FILES uiLength)
		if(${uiLength} GREATER 0)
			QT5_WRAP_UI(TARGET_UI_H ${UI_FILES})
			source_group("${SOURCEGROUP_GENERATED_UI}" FILES ${TARGET_UI_H})
			list(APPEND TARGET_SRC ${TARGET_UI_H} ${UI_FILES})
			#TODO
			#jak tu wyci¹gaæ cie¿kê do generowanych plików ui_*.h? czy nie powinno tego robiæ makro QT4_WRAP_UI
			#powinnimy rozró¿niaæ widgety publiczne i prywatne aby odpowiednio generowaæ pliki ui_*.h i instalowaæ tylko publiczne
			#to wp³ynie równie¿ na sposób includowania takich plików - publiczne bêd¹ widziane jak publiczne nag³ówki, a prywatne tak jak prywatne nag³ówki
			list(APPEND PROJECT_PUBLIC_INCLUDES "${CMAKE_CURRENT_BINARY_DIR}/.." "${CMAKE_CURRENT_BINARY_DIR}")
		endif()
	endif()
	
	if(NOT DEFINED TRANSLATION_FILES AND _GENERATE_PROJECT_TRANSLATIONS)

		set(transFiles "")
	
		foreach(lang ${SOLUTION_TRANSLATION_LANGUAGES})
		
			list(APPEND transFiles "${CURRENT_PROJECT_NAME}_lang_${lang}.ts")
		
		endforeach()
		
		SET_TRANSLATION_FILES(${transFiles})
	
	endif()
	
	# t³umczenia
	if(DEFINED TRANSLATION_FILES)
	
		list(APPEND TARGET_SRC ${TRANSLATION_FILES})
	
	endif()
	
	# MOC
	if(DEFINED MOC_FILES)		
		list(LENGTH MOC_FILES mocLength)
		if(${mocLength} GREATER 0)			
			QT5_WRAP_CPP(TARGET_MOC_SRC ${MOC_FILES})			
			source_group("${SOURCEGROUP_GENERATED_UI}" FILES ${TARGET_MOC_SRC})			
			list(APPEND TARGET_SRC ${TARGET_MOC_SRC})
		endif()
	endif()
	
	# RC
	if(DEFINED RC_FILES)
		list(LENGTH RC_FILES rcLength)
		if(${rcLength} GREATER 0)
			QT5_ADD_RESOURCES(TARGET_RC_SRC ${RC_FILES})
			source_group("${SOURCEGROUP_GENERATED_UI}" FILES ${TARGET_RC_SRC})
			list(APPEND TARGET_SRC ${TARGET_RC_SRC})
		endif()
	endif()
	
	# resources
	if(NOT DEFINED EMBEDDED_RESOURCES_FILES AND NOT DEFINED DEPLOY_RESOURCES_FILES)
	
		TARGET_NOTIFY("EMBEDDED_RESOURCES_FILES,DEPLOY_RESOURCES_FILES" "Auto resource files generation")
		file(GLOB GENERAL_RESOURCES_FILES "${PROJECT_SOURCE_FILES_PATH}/../resources/*.*")
		list(APPEND TARGET_SRC ${GENERAL_RESOURCES_FILES})		
		source_group("${SOURCEGROUP_RESOURCES}" FILES ${GENERAL_RESOURCES_FILES})
		
	endif()
	
	if(NOT DEFINED EMBEDDED_RESOURCES_FILES)
		
		TARGET_NOTIFY(EMBEDDED_RESOURCES_FILES "Auto embedded resource files generation")
		file(GLOB_RECURSE EMBEDDED_RESOURCES_FILES "${PROJECT_SOURCE_FILES_PATH}/../resources/embedded/*.*") 		
		list(APPEND TARGET_SRC ${EMBEDDED_RESOURCES_FILES})
		
		_REBUILD_SCM_STRUCTURE_IN_IDE("${EMBEDDED_RESOURCES_FILES}" "${PROJECT_SOURCE_FILES_PATH}/../resources/embedded" "${SOURCEGROUP_RESOURCES}//${SOURCEGROUP_EMBEDDED_RESOURCES}")
		
	endif()
	
	if(NOT DEFINED DEPLOY_RESOURCES_FILES)
	
		TARGET_NOTIFY(DEPLOY_RESOURCES_FILES "Auto deploy resource files generation")
		file(GLOB_RECURSE DEPLOY_RESOURCES_FILES "${PROJECT_DEPLOY_RESOURCES_FILES_PATH}/*.*") 		
		
		list(APPEND TARGET_SRC ${DEPLOY_RESOURCES_FILES})

		_REBUILD_SCM_STRUCTURE_IN_IDE("${DEPLOY_RESOURCES_FILES}" "${PROJECT_DEPLOY_RESOURCES_FILES_PATH}" "${SOURCEGROUP_RESOURCES}//${SOURCEGROUP_DEPLOY_RESOURCES}")
	
	endif()
	
	# ustawiam wszystkie pliki projektu
	set(ALL_SOURCES ${TARGET_SRC} ${TARGET_H})
	# faktycznie ustawiam typ projektu
	if(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "header")
	
		add_library(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} STATIC ${ALL_SOURCES})
		set_target_properties(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PROPERTIES LINKER_LANGUAGE CXX)		
	
	elseif(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "executable")
		# plik wykonywalny
		if(WIN32)
			# tutaj mo¿emy mieæ aplikacjê z konsol¹ lub bez
			if(PROJECT_${CURRENT_PROJECT_NAME}_WIN32_ENABLE_CONSOLE)
				# konsola
				add_executable(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} ${ALL_SOURCES})
			else()
				# bez konsoli
				add_executable(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} WIN32 ${ALL_SOURCES})
			endif()
		else()
			# dodajemy zwykly plik wykonywalny dla innych platform
			add_executable(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} ${ALL_SOURCES})
			# instalacja skryptów uruchomieniowych dla linux
			if(UNIX)
				#TODO
				#GENERATE_UNIX_EXECUTABLE_SCRIPT()
			endif()
		endif()

	elseif(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "static")
		
		# biblioteka statyczna
		add_library(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} STATIC ${ALL_SOURCES})

	elseif(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "dynamic")
		# biblioteka dynamiczna
		add_library(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} SHARED ${ALL_SOURCES})
	
		# TODO - do weryfikacji na linuxie
		if(UNIX)
			set_target_properties(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})		
		endif()
		
	elseif(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "module")
	
		# biblioteka dynamiczna
		add_library(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} MODULE ${ALL_SOURCES})
	
	endif()
		
	# ustawiamy nazwe dla artefaktow wersji debug tak aby do nazwy na koniec by³o doklejane d, dla release bez zmian
	set_target_properties(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PROPERTIES DEBUG_POSTFIX "d")
	
	# ustawiamy includy projektu
	# wszystkie includy projektu - publiczne, prywatne + konfigi
	#TODO
	#wymagana reorganizacja includów w strukturze, tak by wymuszaæ podawanie zale¿noci pomiêdzy bibliotekami w celu udostêpniania publicznych nag³ówków
	#struktura includów powinna wygl¹daæ tak: include/dirName/dirName/tu_sa_wlasciwe_includy
	#wtedy dany projekt ma dostêp do swojego: include/dirName
	#a jeli chce u¿yæ innych to musi byæ od nich jawnie uzale¿niony
	#aktualnie ka¿dy ma dostêp do publicznych nag³óków ka¿dego innego projektu	
	list(APPEND PROJECT_PRIVATE_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/src" "${SOLUTION_INCLUDE_ROOT}")

	# prywatne i publiczne includy po konfiguracji
	list(LENGTH CONFIGURE_PRIVATE_HEADER_FILES APPEND_PRIVATE)
	if(${APPEND_PRIVATE} GREATER 0)
		if(EXISTS "${PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH}")
			list(APPEND PROJECT_PRIVATE_INCLUDES "${PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH}")
		else()
			message(SEND_ERROR "Zarejestrowano pliki konfiguracyjne prywatne, ale ich katalog docelowy nie istnieje. Nie mo¿na do³¹czyæ tych plików jako includy")
		endif()
	endif()
	
	list(LENGTH CONFIGURE_PUBLIC_HEADER_FILES APPEND_PUBLIC)
	if(${APPEND_PUBLIC} GREATER 0)
		if(EXISTS "${PROJECT_PUBLIC_CONFIGURATION_INCLUDES_PATH}")
			list(APPEND PROJECT_PUBLIC_INCLUDES "${PROJECT_PUBLIC_CONFIGURATION_INCLUDES_PATH}")
		else()
			message(SEND_ERROR "Zarejestrowano pliki konfiguracyjne publiczne, ale ich katalog docelowy nie istnieje. Nie mo¿na do³¹czyæ tych plików jako includy")
		endif()
	endif()
	
	# ustawiamy grupê projektu jeli by³a podana
	string(LENGTH PROJECT_${CURRENT_PROJECT_NAME}_GROUP PROJECT_GROUP_LENGTH)
	if(PROJECT_GROUP_LENGTH GREATER 0)
		SET_PROPERTY(TARGET ${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PROPERTY FOLDER "${PROJECT_${CURRENT_PROJECT_NAME}_GROUP}")
	endif()
	
	# ustawiam zale¿noci
	set(USED_DEPENDECIES "")
	set(PROJECT_LIBRARIES "")
	list(APPEND PROJECT_COMPILER_DEFINITIONS ${PROJECT_${CURRENT_PROJECT_NAME}_COMPILER_DEFINITIONS})
	set(PROJECT_COMPILER_FLAGS ${PROJECT_${CURRENT_PROJECT_NAME}_COMPILER_FLAGS})
	
	set(ALL_PROJECT_DEPENDENCIES "")
	if(PROJECT_${CURRENT_PROJECT_NAME}_DEPENDENCIES)
		list(APPEND ALL_PROJECT_DEPENDENCIES "${PROJECT_${CURRENT_PROJECT_NAME}_DEPENDENCIES}")
	endif()
	
	set(_dependenciesList)
	
	foreach(value ${PROJECT_${CURRENT_PROJECT_NAME}_DEPENDENCIES})
		
		TARGET_NOTIFY(${CURRENT_PROJECT_NAME} "RAW DEPENDENCY ${value} libraries: ${LIBRARY_${value}_LIBRARIES}")
		
		if(${value} STREQUAL ${CURRENT_PROJECT_NAME} AND NOT DEFINED SELF_DEPENDENCY)
			
			TARGET_NOTIFY(SELF_DEPENDENCY "Projekt ${CURRENT_PROJECT_NAME} nie moze byæ rekurencyjnie zalezny od samego siebie. Pomijam zale¿noæ...")
			set(SELF_DEPENDENCY 1)
			
		else()			
			
			#szukam czy zadana zale¿noæ nie by³a ju¿ dodana
			list(FIND USED_DEPENDECIES ${value} DEPENDENCY_USED)
			if(${DEPENDENCY_USED} GREATER -1 AND NOT DEFINED ${value}_DUPLICATED_DEPENDENCY)
				
				set(${value}_DUPLICATED_DEPENDENCY 1)
				TARGET_NOTIFY(DEPENDENCY_USED "Dla projektu ${CURRENT_PROJECT_NAME} zale¿noæ ${value} ju¿ zosta³a dodana i powtarza siê. Zostanie ona teraz pominiêta.")
				
			else()
			
				# zapamiêtujê ¿e zale¿noc ju¿ zosta³a u¿yta
				list(APPEND USED_DEPENDECIES ${value})
				# czy to nasz projekt czy zale¿noæ?
				list(FIND SOLUTION_PROJECTS ${value} IS_PROJECT)
				
				if(IS_PROJECT GREATER -1)
					
					#nasz projekt
					if(PROJECT_${value}_TYPE STREQUAL "executable" OR PROJECT_${value}_TYPE STREQUAL "module")
						TARGET_NOTIFY(PROJECT_${value}_TYPE "Projekt ${CURRENT_PROJECT_NAME} jest zale¿ny od projektu ${value} który jest plikiem wykonywalnym. Pomijam ten projekt w zale¿nociach")
					else()
						if(NOT PROJECT_${value}_TYPE STREQUAL "header")
							list(APPEND _dependenciesList "${PROJECT_${value}_TARGETNAME}")
							list(APPEND PROJECT_LIBRARIES ${PROJECT_${value}_TARGETNAME})
							list(APPEND PROJECT_LIBRARIES ${PROJECT_${value}_LIBRARIES})
						endif()
						list(APPEND PROJECT_PUBLIC_INCLUDES ${PROJECT_${value}_INCLUDE_DIRS})
						list(APPEND PROJECT_COMPILER_DEFINITIONS ${PROJECT_${value}_COMPILER_DEFINITIONS})
						list(APPEND PROJECT_COMPILER_FLAGS ${PROJECT_${value}_COMPILER_FLAGS})
					endif()
					
					list(APPEND ALL_PROJECT_DEPENDENCIES ${PROJECT_${value}_ALL_DEPENDENCIES})
					
				else()
					# zewnêtrzna biblioteka
					# biblioteki zale¿ne
					
					if(DEFINED LIBRARY_${value}_LIBRARIES)
						list(APPEND PROJECT_LIBRARIES ${LIBRARY_${value}_LIBRARIES})					
					endif()
					
					# dodatkowe definicje wynikaj¹ce z bibliotek zale¿nych
					if(DEFINED ${value}_COMPILER_DEFINITIONS)						
						list(APPEND PROJECT_COMPILER_DEFINITIONS ${${value}_COMPILER_DEFINITIONS})
					endif()
					
					# dodatkowe flagi kompilacji wynikaj¹ce z bibliotek zale¿nych (np. OpenMP)
					if(DEFINED ${value}_COMPILER_FLAGS)						
						list(APPEND PROJECT_COMPILER_FLAGS ${${value}_COMPILER_FLAGS})
					endif()
					
					# includy bibliotek zaleznych
					if(DEFINED ${value}_INCLUDE_DIR)
						list(APPEND PROJECT_PUBLIC_INCLUDES ${${value}_INCLUDE_DIR})
					endif()
					
					if(DEFINED ${value}_ADDITIONAL_INCLUDE_DIRS)
						list(APPEND PROJECT_PUBLIC_INCLUDES ${${value}_ADDITIONAL_INCLUDE_DIRS})
					endif()
					
					list(APPEND ALL_PROJECT_DEPENDENCIES ${LIBRARY_${value}_DEPENDENCIES})
					
				endif()
			endif()
		endif()
	endforeach()
	
	list(LENGTH _dependenciesList _dlLength)
	
	if(_dlLength GREATER 0)
	
		add_dependencies(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} ${_dependenciesList})		
		
	endif()
	
	# includy
	# usuwamy duplikaty
	list(REMOVE_DUPLICATES PROJECT_PUBLIC_INCLUDES)
	list(REMOVE_DUPLICATES PROJECT_PRIVATE_INCLUDES)
	list(REMOVE_DUPLICATES ALL_PROJECT_DEPENDENCIES)
	
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_ALL_DEPENDENCIES "${ALL_PROJECT_DEPENDENCIES}" "Project ${CURRENT_PROJECT_NAME} all dependencies")
	
	if(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "executable" AND WIN32 AND NOT PROJECT_${CURRENT_PROJECT_NAME}_WIN32_ENABLE_CONSOLE)

		list(FIND PROJECT_${CURRENT_PROJECT_NAME}_ALL_DEPENDENCIES "QT" _qtFound)
		if(${_qtFound} GREATER -1)
			list(APPEND PROJECT_LIBRARIES ${QT_MAIN_LIBS})			
		endif()
	
	endif()
	
	# jesli cos jest dodajemy
	list(LENGTH PROJECT_PUBLIC_INCLUDES publicIncludesLength)
	if(publicIncludesLength GREATER 0)
	
		target_include_directories(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PUBLIC ${PROJECT_PUBLIC_INCLUDES})
		
	endif()
	
	list(LENGTH PROJECT_PRIVATE_INCLUDES privateIncludesLength)
	if(privateIncludesLength GREATER 0)
		target_include_directories(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PRIVATE ${PROJECT_PRIVATE_INCLUDES})
	endif()
	
	# zapamietujemy dla kolejnych projektow
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_INCLUDE_DIRS "${PROJECT_PUBLIC_INCLUDES}" "sciezka do includów projektu ${CURRENT_PROJECT_NAME}")
	# biblioteki
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_LIBRARIES "${PROJECT_LIBRARIES}" "Biblioteki zależne ${CURRENT_PROJECT_NAME}")	
	
	# ustawiamy definicje kompilacji projektu wynikaj¹ce z jego zale¿noci
	list(REMOVE_DUPLICATES PROJECT_COMPILER_DEFINITIONS)
	list(LENGTH PROJECT_COMPILER_DEFINITIONS DEF_COUNT)
	if(${DEF_COUNT} GREATER 0)
		#set_target_properties(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PROPERTIES COMPILE_DEFINITIONS "${PROJECT_COMPILER_DEFINITIONS}")
		#TODO
		#rozdielic na public i private
		
		target_compile_definitions(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PUBLIC ${PROJECT_COMPILER_DEFINITIONS})
				
		_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_COMPILER_DEFINITIONS "${PROJECT_COMPILER_DEFINITIONS}" "Definicje kompilatora dla projektu ${CURRENT_PROJECT_NAME}")
	endif()
	
	# ustawiamy flagi kompilacji dla projektu
	if(DEFINED PROJECT_COMPILER_FLAGS)
		list(LENGTH PROJECT_COMPILER_FLAGS flagsLength)
		if(${flagsLength} GREATER 0)
			list(REMOVE_DUPLICATES PROJECT_COMPILER_FLAGS)
			set_target_properties(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PROPERTIES COMPILE_FLAGS "${PROJECT_COMPILER_FLAGS}")			
		endif()
	endif()
	
	# mamy ju¿ wszystkie katalogi z publicznymi includami
	# mogê generowaæ komendy dla t³umaczeñ
	
	# t³umczenia
	if(DEFINED TRANSLATION_FILES)
	
		list(LENGTH TRANSLATION_FILES langLength)
		if(${langLength} GREATER 0)			
	
			set(DEBUG_LANG_OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/resources/lang")
			set(RELEASE_LANG_OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/resources/lang")
	
			foreach(lang ${TRANSLATION_FILES})
				GET_FILENAME_COMPONENT(lang_name ${lang} NAME_WE)
				list(APPEND QM_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/${lang_name}.qm")
				
				add_custom_command(TARGET ${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} PRE_BUILD
					# Debug
					COMMAND ${CMAKE_COMMAND} -E remove "${DEBUG_LANG_OUTPUT}/${lang_name}.qm"
					# Release
					COMMAND ${CMAKE_COMMAND} -E remove "${RELEASE_LANG_OUTPUT}/${lang_name}.qm"
					VERBATIM
				)
				
			endforeach()
			
			# TODO
			# Generalnie jest sporo problemów z t³umaczeniami jak widaæ po iloci TODO poni¿ej.
			# Je¿eli u¿ywamy tr() w pliku ród³owym w ramach namespace (jawnie w klamrach namespace lub niejawnie using namespace)
			# musimy zagwarantowaæ ¿eby lupdate widzia³ publiczny nag³ówek tego ród³a. Robimy to poprzez dodanie
			# prze³¹cznika -I cie¿ka do includów. W przeciwnym wypadku rezygnujemy w kodzie ród³a z using namespace,
			# jawnie zamykamy ca³y kod w klamrach naszego namespace oraz zaraz za includami dodajemy wpis typu:
			#
			# 	/*
			#		TRANSLATOR namespace::MyClass
			#	*/
			#
			# gdzie MyClass to nasza klasa z której wo³amy tr. Zdecydowanie lepiej jednak u¿ywaæ -I !!!
			#
			# http://qt-project.org/doc/qt-4.8/linguist-programmers.html
			
			# TODO
			# jeli mamy za du¿o plików, maj¹ one za d³ugie nazwy to czasem lupdate/konsola VS siê krzaczy
			# wyrzucaj¹c ¿e jakiego pliku nie mo¿e znaleæ. Jesli istnieje to jego cie¿ka jest pewnie gdzie
			# przek³amana w komunikacie. W tym wypadku przeszlismy na skanowanie katalogów w poszukiwaniu zadanych plików.
			# lupdate wewnêtrznie przegl¹da strukturê i komenda jego wywo³ania znacznie siê upraszcza
			
			# Aktualna dokumentacja lupdate - potrzebna przy t³umaczeniach kiedy mamy du¿o plików.
			# By³y problemy z generowanie komend dla VS - by³y chyba zbyt d³ugie i je ucina³o
			
			# Usage:
			#    lupdate [options] [project-file]...
			#    lupdate [options] [source-file|path|@lst-file]... -ts ts-files|@lst-file
			# lupdate is part of Qt's Linguist tool chain. It extracts translatable
			# messages from Qt UI files, C++, Java and JavaScript/QtScript source code.
			# Extracted messages are stored in textual translation source files (typically
			# Qt TS XML). New and modified messages can be merged into existing TS files.
			# Options:
			#    -help  Display this information and exit.
			#    -no-obsolete
			#           Drop all obsolete strings.
			#    -extensions <ext>[,<ext>]...
			#           Process files with the given extensions only.
			#           The extension list must be separated with commas, not with whitespace.
			#           Default: '%1'.
			#    -pluralonly
			#           Only include plural form messages.
			#    -silent
			#           Do not explain what is being done.
			#    -no-sort
			#           Do not sort contexts in TS files.
			#    -no-recursive
			#           Do not recursively scan the following directories.
			#    -recursive
			#           Recursively scan the following directories (default).
			#    -I <includepath> or -I<includepath>
			#           Additional location to look for include files.
			#           May be specified multiple times.
			#    -locations {absolute|relative|none}
			#           Specify/override how source code references are saved in TS files.
			#           Default is absolute.
			#    -no-ui-lines
			#           Do not record line numbers in references to UI files.
			#    -disable-heuristic {sametext|similartext|number}
			#           Disable the named merge heuristic. Can be specified multiple times.
			#    -pro <filename>
			#           Name of a .pro file. Useful for files with .pro file syntax but
			#           different file suffix. Projects are recursed into and merged.
			#    -source-language <language>[_<region>]
			#           Specify the language of the source strings for new files.
			#           Defaults to POSIX if not specified.
			#    -target-language <language>[_<region>]
			#           Specify the language of the translations for new files.
			#           Guessed from the file name if not specified.
			#    -ts <ts-file>...
			#           Specify the output file(s). This will override the TRANSLATIONS
			#           and nullify the CODECFORTR from possibly specified project files.
			#    -codecfortr <codec>
			#           Specify the codec assumed for tr() calls. Effective only with -ts.
			#    -version
			#           Display the version of lupdate and exit.
			#    @lst-file
			#           Read additional file names (one per line) from lst-file.
			
			set(PH_PATH ${PROJECT_PUBLIC_HEADER_PATH})
			
			if(NOT EXISTS ${PH_PATH})
				set(PH_PATH "")
			endif()
			
			set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR})
			
			if(NOT EXISTS ${SRC_PATH})
				set(SRC_PATH "")
			endif()
			
			set(PCH_PATH ${PROJECT_PUBLIC_CONFIGURATION_INCLUDES_PATH})
			
			if(NOT EXISTS ${PCH_PATH})
				set(PCH_PATH "")
			endif()
			
			set(PPCH_PATH ${PROJECT_PRIVATE_CONFIGURATION_INCLUDES_PATH})
			
			if(NOT EXISTS ${PPCH_PATH})
				set(PPCH_PATH "")
			endif()
			
			add_custom_command(TARGET ${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} POST_BUILD
				# TS				
				COMMAND ${QT_LUPDATE_EXECUTABLE} -silent -locations absolute -I ${SOLUTION_INCLUDE_ROOT} -recursive ${PH_PATH} ${SRC_PATH}
				${PCH_PATH} ${PPCH_PATH}
				-extensions h,hh,hpp,c,cc,cpp,ui -ts ${TRANSLATION_FILES}				
				VERBATIM
			)			
			set(_idx 0)
			list(LENGTH TRANSLATION_FILES _size)
			
			set(PROJECT_${CURRENT_PROJECT_NAME}_TRANSLATIONS ${QM_OUTPUTS})
			
			while(_size GREATER _idx)
			
				list(GET QM_OUTPUTS ${_idx} lang)
				list(GET TRANSLATION_FILES ${_idx} translation)
				
				get_filename_component(_name "${lang}" NAME)
			
				add_custom_command(TARGET ${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} POST_BUILD
					# QM
					COMMAND ${QT_LRELEASE_EXECUTABLE} -silent -compress -removeidentical ${translation} -qm ${lang}
					# kopiowanie do odpowiednich katalogów
					COMMAND ${CMAKE_COMMAND} -E copy ${lang} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/resources/lang/${_name}"
					COMMAND ${CMAKE_COMMAND} -E copy ${lang} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/resources/lang/${_name}"
					VERBATIM
				)
				
				math(EXPR _idx "${_idx} + 1")
				
			endwhile()
			
		endif()	
	
	endif()
	
	if(DEFINED DEPLOY_RESOURCES_FILES)
		list(LENGTH DEPLOY_RESOURCES_FILES _depLength)
		if(_depLength GREATER 0)
			set(PROJECT_${CURRENT_PROJECT_NAME}_DEPLOY_RESOURCES ${DEPLOY_RESOURCES_FILES})
		endif()
	endif()
	
	if(DEFINED DEPLOY_MODIFIABLE_RESOURCES_FILES)
		_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_DEPLOY_MODIFIABLE_RESOURCES "${DEPLOY_MODIFIABLE_RESOURCES_FILES}" "Zasoby modyfikowane przez projekt")		
	else()
		unset(PROJECT_${CURRENT_PROJECT_NAME}_DEPLOY_MODIFIABLE_RESOURCES)
	endif()
	
	_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_DEPLOY_RESOURCES_PATH "${PROJECT_DEPLOY_RESOURCES_FILES_PATH}" "Sciezka instalacji zasobow")
	
	# biblioteki do linkowania
	#hack - podwójnie ¿eby dobrze wyznaczy³ zale¿noci pomiêdzy projektami i bibliotekami zale¿nymi (kolejnoæ linkowania)
	
	target_link_libraries(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} ${PROJECT_LIBRARIES})
	
	# info ze poprawnie zakonczylismy dodawanie projektu
	set(PROJECT_ADD_FINISHED 1 PARENT_SCOPE)
	
	# TODO
	if(DEFINED PROJECT_ADDITIONAL_INSTALLS)		
		_SETUP_INTERNAL_CACHE_VALUE(PROJECT_${CURRENT_PROJECT_NAME}_ADDITIONAL_INSTALLS "${PROJECT_ADDITIONAL_INSTALLS}" "Dodatkowe sciezki instalacji")
	endif()
	
	if(CREATE_INSTALLATION)
		_INSTALL_PROJECT(${CURRENT_PROJECT_NAME})
	endif()
	
	# generujemy finder
	if(CONFIG_GENERATE_FINDERS)
		_GENERATE_FINDER(${CURRENT_PROJECT_NAME} "${SOLUTION_GENERATED_FINDERS_DESTINATION}")
	endif()
	
	if(DEFINED PROJECT_${CURRENT_PROJECT_NAME}_IS_TEST)
		add_test(NAME ${CURRENT_PROJECT_NAME} COMMAND ${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME})
	endif()
	set(DEPENDENCIES_AS_PREREQUSITES)
	
endmacro(END_PROJECT)


# Makro ustawiaj¹ce pewn¹ opcjê konfiguracji uwzglêdniaj¹c jej zaleznoci.
# Parametry:
#	name	Nazwa makra.
#	info	Tekstowa informacja o opcji.
#	default	ON / OFF
# 	dependencies - lista opcji zale¿nych wraz z opcjonalnym zaprzeczeniem
macro(CONFIG_DEPENDENT_OPTION name info default dependencies)
	
	_NEGATE_OPTION_STATE(${default})
	
	set(realDependencies "")
	
	foreach(dep ${dependencies})
	
		string(FIND ${dep} "NOT " _negatedIDX)
		
		if( NOT (_negatedIDX EQUAL 0) )
			list(APPEND realDependencies "CONFIG_${dep}")
		else()
			list(APPEND realDependencies "NOT CONFIG_${dep}")
		endif()
	
	endforeach()
	
	CMAKE_DEPENDENT_OPTION(CONFIG_${name} "${info}" ${default}
                         "${realDependencies}" ${_stateNegation})
	
	_SETUP_CONFIG_OPTION(${name} "${info}")	
	
endmacro(CONFIG_DEPENDENT_OPTION)

###############################################################################
# Makro pomocnicze przy generacji finderów - oznacza, które z zale¿noci
# s¹ ostatecznie tylko prerequisitami
# Parametry:
#	deps - lista zale¿noci które s¹ tylko prerequisitami
# Zale¿noci te s¹ potem weryfikowane wzglêdem wszystkich dependency jakie podano
# dla zadanej biblioteki
macro(MARK_AS_PREREQUISITES deps)

	set(DEPENDENCIES_AS_PREREQUSITES ${deps})
	
endmacro(MARK_AS_PREREQUISITES)


###############################################################################
# Makro pomocnicze przy obs³udze zale¿noci konfigurowanych
# Oznacza w jakich plikach nale¿y szukaæ konkretnych definów
# które wp³ywaj¹ na zale¿noci projektu
# Parametry:
#	headerFile - plik ród³owy w którym mamy szukaæ podanych definów
#	configVariables - lista definów konfigurowalnych, które wp³ywaj¹ na zaleznoci projektu
macro(SEARCH_DEPENDENCY_CONFIGURATION_DEFINES headerFile configVariables)

	if(NOT DEFINED _DEPS_ALL_PUBLIC)
		set(_DEPS_ALL_PUBLIC ${PUBLIC_H} ${CONFIGURE_PUBLIC_HEADER_FILES})
	endif()
		
	# pliki te musz¹ siê znaleæ w którejæ z wersji nag³ówków publicznych lubkonfiguracyjnych po konfiguracji
		
	set(_KEEP_TRYING 1)

	foreach(f ${_DEPS_ALL_PUBLIC})			
	
		if(_KEEP_TRYING EQUAL 1)
			string(FIND "${f}" "${headerFile}" _idx)
			if(_idx GREATER -1)
				set(_KEEP_TRYING 0)
				set(filePath "${f}")
			endif()		
		endif()
		
	endforeach()			
		
	if(_KEEP_TRYING EQUAL 1)
		TARGET_NOTIFY(_KEEP_TRYING "Plik ${headerFile} nie zosta³ zarejestrowany w projekcie ${CURRENT_PROJECT_NAME} jako plik publiczny a ma byæ u¿yty do odczytu konfiguracji. Zarejestruj plik do jednej z podstawowych grup: PUBLIC_HEADERS, plikach po konfiguracji. Pomijam plik")
	else()
		set(FILE_${PROJECT_CONFIGURATION_FILE_ID}_DEFINES ${configVariables})
		set(CONFIGURATION_FILE_${PROJECT_CONFIGURATION_FILE_ID} "${headerFile}")
		math(EXPR PROJECT_CONFIGURATION_FILE_ID "${PROJECT_CONFIGURATION_FILE_ID} + 1")
	endif()	
	
endmacro(SEARCH_DEPENDENCY_CONFIGURATION_DEFINES)


###############################################################################
# Makro generuj¹ce finder dla zadanej biblioteki tworzonej za pomoca naszych
# makr : ADD_PROJECT, BEGIN_PROJECT, END_PROJECT
# Parametry:
#	projectName - nazwa projektu dla którego generujemy finder
#	path - cie¿ka gdzie generujemy finder
macro(_GENERATE_FINDER projectName path)
	
	set(FINDER_FILE "${path}/Find${projectName}.cmake")
	
	if(NOT EXISTS path)
		file(MAKE_DIRECTORY ${path})
	endif()		
	
	file(TO_CMAKE_PATH "${PROJECT_${projectName}_RELATIVE_PATH}" HEADER_INSTALL_PATH)
	string(FIND "${HEADER_INSTALL_PATH}" "/" _firstIDX)
	if(_firstIDX GREATER -1)
		string(SUBSTRING "${HEADER_INSTALL_PATH}" 0 ${_firstIDX} _first)
		set(HEADER_INSTALL_PATH "${_first}")
	endif()

	# ustawiamy pocz¹tek findera
	if(${PROJECT_${projectName}_TYPE} STREQUAL "header")
		file(WRITE "${FINDER_FILE}" "FIND_INIT_HEADER(${projectName} \"${HEADER_INSTALL_PATH}/${PROJECT_${projectName}_RELATIVE_PATH}\" \"${HEADER_INSTALL_PATH}\" \"${PROJECT_${projectName}_RELATIVE_PATH}\" \"${PROJECT_${projectName}_RELATIVE_PATH}\"  )")
	else()
		file(WRITE "${FINDER_FILE}" "FIND_INIT2(${projectName} \"${HEADER_INSTALL_PATH}/${PROJECT_${projectName}_RELATIVE_PATH}\" \"${HEADER_INSTALL_PATH}\" \"${PROJECT_${projectName}_RELATIVE_PATH}\" \"${PROJECT_${projectName}_RELATIVE_PATH}\"  )")
	endif()
	
	#szukamy bibliotek
	
	if(${PROJECT_${projectName}_TYPE} STREQUAL "executable")
		
		file(APPEND "${FINDER_FILE}" "\nFIND_EXECUTABLE(${projectName} ${PROJECT_${projectName}_TARGETNAME})")

	elseif(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "static")
		
		file(APPEND "${FINDER_FILE}" "\nFIND_STATIC(${projectName} ${PROJECT_${projectName}_TARGETNAME})")

	elseif(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "dynamic")
		
		file(APPEND "${FINDER_FILE}" "\nFIND_SHARED(${projectName} ${PROJECT_${projectName}_TARGETNAME} ${PROJECT_${projectName}_TARGETNAME})")
		
	elseif(${PROJECT_${CURRENT_PROJECT_NAME}_TYPE} STREQUAL "module")
	
		if(WIN32)
			file(APPEND "${FINDER_FILE}" "\nFIND_DLL(${projectName} ${PROJECT_${projectName}_TARGETNAME})")
		elseif(UNIX)
			file(APPEND "${FINDER_FILE}" "\nFIND_SHARED(${projectName} ${PROJECT_${projectName}_TARGETNAME})")
		endif()
		
	endif()
	
	# zale¿noci projektu - wszystkie
	set(PROJECT_DEPENDENCIES ${PROJECT_${projectName}_DEPENDENCIES})
	# iloc zale¿noci
	list(LENGTH PROJECT_DEPENDENCIES depsLength)
	if(depsLength GREATER 0)			
		
		# resetuje liste prerequisitow
		set(PROJECT_PREREQUISITES "")
		
		# sprawdzam czy s¹ prerequisity
		if(DEFINED DEPENDENCIES_AS_PREREQUSITES)
			# sprawdzam czy wszystkie prerequisity s¹ 
			foreach(prereq ${DEPENDENCIES_AS_PREREQUSITES})
				list(FIND PROJECT_DEPENDENCIES ${prereq} _prereqIndex)
				if(_prereqIndex GREATER -1)
					list(APPEND PROJECT_PREREQUSITES ${prereq})
				else()
					TARGET_NOTIFY(PROJECT_DEPENDENCIES "For project ${projectName} dependency ${prereq} marked as prereqisit but not listed in dependency list in ADD_PROJECT makro. Skiping prerequisit in finder.")
				endif()
			endforeach()
		
			# dzielê faktycznie na dependency i prerequisities
			foreach(prereq ${PROJECT_PREREQUSITES})
				list(REMOVE_ITEM PROJECT_DEPENDENCIES ${prereq})
			endforeach()
			
		endif()
		
		# iloæ konfiguracji dla dodatkowych zale¿noci
		set(_dependencyConfigurationsSize ${PROJECT_${projectName}_DEPENDENCIES_CONFIGURATIONS_SIZE})
		# iloæ plików konfiguracyjnych
		list(LENGTH DEFINE_CONFIGURATION_FILES _configFilesLength)
		
		# sprawdzamy czy iloci sie zgadzaj¹
		if(_configFilesLength EQUAL 0 AND _dependencyConfigurationsSize GREATER 0)
			TARGET_NOTIFY(DEFINE_CONFIGURATION_FILES "Project ${projectName} configurable dependencies defined with configuration variables but none configuration file wes defined within project definition")
		elseif(_configFilesLength GREATER 0 AND _dependencyConfigurationsSize GREATER 0)
			# tu jeszcze nie mam pewnoci - wiem tylko ¿e s¹ konfigurowalne zale¿noci i pliki konfiguracyjne
			# musze sprawdziæ czy iloæ i nazwy siê zgadzaj¹
			# zerujê listê zmiennych konfiguracyjnych zale¿noci
			set(PROJECT_CONFIG_VARIABLES "")
			
			set(_allConfigVariables "")
			
			set(_idx 0)
			# wype³niam liste zmiennych konfiguracyjnych dla zale¿noci
			while(_dependencyConfigurationsSize GREATER _idx)
			
				list(APPEND PROJECT_CONFIG_VARIABLES ${PROJECT_${projectName}_DEPENDENCIES_CONFIG_${_idx}_VARIABLES})
			
				math(EXPR _idx "${_idx} + 1")
			
			endwhile()
			
			list(REMOVE_DUPLICATES PROJECT_CONFIG_VARIABLES)
			#tymczasowa lista zmiennych konfiguracyjnych zaleznoci
			set(tmpDepConfigVars ${PROJECT_CONFIG_VARIABLES})
			
			# sprawdzam zmienne konfiguracyjne
			set(_idx 0)
			while(PROJECT_CONFIGURATION_FILE_ID GREATER _idx)
				set(tmpFileDefines ${FILE_${_idx}_DEFINES})
				foreach(var ${tmpFileDefines})
					
					list(FIND PROJECT_CONFIG_VARIABLES ${var} _varIndex)
					if(_varIndex GREATER -1)
						list(REMOVE_ITEM tmpDepConfigVars ${var})
					else()
						list(REMOVE_ITEM FILE_${_idx}_DEFINES ${var})
						TARGET_NOTIFY(PROJECT_CONFIG_VARIABLES "For project ${projectName} additional dependency configuration variable ${var} introduced, which was not used for customizing dependencies. Skipping variable in finder generation!")
					endif()						
				
				endforeach()
				
				list(LENGTH FILE_${_idx}_DEFINES _length)
				
				if(_length GREATER 0)
					file(APPEND "${FINDER_FILE}" "\nFIND_SOURCE_FILE_DEFINE_CONDITIONS(\"${CONFIGURATION_FILE_${_idx}}\" \"${FILE_${_idx}_DEFINES}\")")
				endif()
				
				math(EXPR _idx "${_idx} + 1")
				
			endwhile()
			
			list(LENGTH tmpDepConfigVars _missingSize)
			
			if(_missingSize GREATER 0)
				TARGET_NOTIFY(PROJECT_CONFIG_VARIABLES "For project ${projectName} dependency configuration variables ${tmpDepConfigVars} are missing for finder generation. This might cause false finder functionality.")					
			endif()
			
			set(FINDER_BODY_APPEND "")
			
			set(_idx 0)
			# dzielê zaleznoci na prerequsities oraz dependecies wraz z odpowiednimi zmiennymi konfiguracyjnymi
			while(_dependencyConfigurationsSize GREATER _idx)
			
				set(prerequsitesON "")
				set(prerequsitesOFF "")
				set(dependenciesON "")
				set(dependenciesOFF "")
				set(appendDeps 0)
				set(appendPrereq 0)
				
				foreach(dep ${PROJECT_${projectName}_DEPENDENCIES_CONFIG_${_idx}_DEPS_ON})
					
					list(FIND PROJECT_DEPENDENCIES ${dep} _depIndex)
				
					if(_depIndex GREATER -1)
						list(APPEND dependenciesON ${dep})
						list(REMOVE_ITEM PROJECT_DEPENDENCIES ${dep})
						set(appendDeps 1)
					else()
						list(APPEND prerequsitesON ${dep})
						list(REMOVE_ITEM PROJECT_PREREQUISITES ${dep})
						set(appendPrereq 1)
					endif()
				
				endforeach()
				
				foreach(dep ${PROJECT_${projectName}_DEPENDENCIES_CONFIG_${_idx}_DEPS_OFF})
				
					list(FIND PROJECT_DEPENDENCIES ${dep} _depIndex)
				
					if(_depIndex GREATER -1)
						list(APPEND dependenciesOFF ${dep})
						list(REMOVE_ITEM PROJECT_DEPENDENCIES ${dep})
						set(appendDeps 1)
					else()
						list(APPEND prerequsitesOFF ${dep})
						list(REMOVE_ITEM PROJECT_PREREQUISITES ${dep})
						set(appendPrereq 1)
					endif()
				
				endforeach()
				
				if(appendDeps OR appendPrereq)
				
					# usuwam zmienne których nie mam w plikach konfiguracyjnych ani w opcjach
					set(conditionalVariables ${PROJECT_${projectName}_DEPENDENCIES_CONFIG_${_idx}_VARIABLES})
					
					foreach(var ${tmpDepConfigVars})
						list(REMOVE_ITEM conditionalVariables ${var})
					endforeach()
				
					if(appendDeps)					
						file(APPEND "${FINDER_FILE}" "\nFIND_CONDITIONAL_DEPENDENCIES(\"${conditionalVariables}\" \"${dependenciesON}\" \"${dependenciesOFF}\")")
					endif()
					
					if(appendPrereq)
						file(APPEND "${FINDER_FILE}" "\nFIND_CONDITIONAL_PREREQUISITES(\"${PROJECT_${projectName}_DEPENDENCIES_CONFIG_${_idx}_VARIABLES}\" \"${prerequsitesON}\" \"${prerequsitesOFF}\")")
					endif()
				
				endif()
			
				math(EXPR _idx "${_idx} + 1")
			
			endwhile()
			
		endif()
		
	endif()
	
	list(LENGTH PROJECT_DEPENDENCIES _depLength)
	
	if(_depLength GREATER 0)
		file(APPEND "${FINDER_FILE}" "\nFIND_DEPENDENCIES(${projectName} \"${PROJECT_DEPENDENCIES}\")")
	endif()
	
	list(LENGTH PROJECT_PREREQUSITES _prereqLength)
	
	if(_prereqLength GREATER 0)
		file(APPEND "${FINDER_FILE}" "\nFIND_PREREQUISITES(${projectName} \"${PROJECT_PREREQUSITES}\")")
	endif()
	
	# ustawiamy koniec findera		
	# zapisuje finder
	file(APPEND "${FINDER_FILE}" "\nFIND_FINISH(${projectName})")
	
endmacro(_GENERATE_FINDER)

###############################################################################

macro(TARGET_NOTIFY var msg)
	if (TARGET_VERBOSE)
		message(STATUS "TARGET>${var}>${msg}")
	endif()
endmacro(TARGET_NOTIFY)
