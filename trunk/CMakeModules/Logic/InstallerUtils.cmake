# inicjalizacja logowania wiadomosci modulu installer
INIT_VERBOSE_OPTION(INSTALLER "Print installer verbose info?")	

###############################################################################
# Automatyczne generowanie warningu kiedy instalujemy do ścieżek bezwzględnych!!
###############################################################################

set(CPACK_WARN_ON_ABSOLUTE_INSTALL_DESTINATION ON)

###############################################################################
# Makro obsługujące generowanie komunikatów diagnostycznych dla makr instalacji
# Parametry:
#		var - nazwa zmiennej
#		msg - wiadomość
macro(INSTALLER_NOTIFY var msg)
	if (INSTALLER_VERBOSE)
		message(STATUS "INSTALLER>${var}>${msg}")
	endif()
endmacro(INSTALLER_NOTIFY)


###############################################################################
# Sprawdza czy obiekt jest bezposrednio instalowalna
#	Parametry:
# 			variable - zmienna której ustawiamy 0 lub 1 w zależności czy biblioteka dla danej konfiguracji jest instalowalna
#			type - typ instalacji dla którego sprawdzamy czy biblioteka jest instalowalna
#			library - biblioteka która sprawdzamy		
macro(IS_INSTALLABLE variable type object)

	list(FIND SOLUTION_PROJECTS ${object} _isProject)
	
	if(_isProject GREATER -1)
	
		IS_PROJECT_INSTALLABLE( "${variable}" "${type}" "${object}" )
	
	else()
	
		IS_LIBRARY_INSTALLABLE( "${variable}" "${type}" "${object}" )
	
	endif()

endmacro(IS_INSTALLABLE)

###############################################################################
# Makro obsługujące generowanie komunikatów diagnostycznych dla makr instalacji
# Parametry:
#		name - nazwa typu instalacji
#		display - Wyswietlana nazwa typu instalacji
macro(_CPACK_ADD_INSTALL_TYPE name display)

	string(TOUPPER "${name}" _name)
	list(APPEND CPACK_ALL_INSTALL_TYPES ${_name})
	set(CPACK_INSTALL_TYPE_${_name}_DISPLAY_NAME "${display}")
	
endmacro(_CPACK_ADD_INSTALL_TYPE)

###############################################################################
# Makro obsługujące generowanie komunikatów diagnostycznych dla makr instalacji
# Parametry:
#		var - zmienna do ktorej trafi nazwa komponentu CPack
#		name - nazwa komponentu
macro(_CPACK_COMPONENT_NAME var name)
	
	string(TOUPPER ${name} _name)
	set(${var} "CPACK_COMPONENT_${_name}")
	
endmacro(_CPACK_COMPONENT_NAME)

###############################################################################
# Makro obsługujące generowanie komunikatów diagnostycznych dla makr instalacji
# Parametry:
#		var - zmienna do ktorej trafi nazwa komponentu CPack
#		name - nazwa komponentu
macro(_CPACK_COMPONENT_GROUP_NAME var name)
	
	string(TOUPPER "${name}" _name)
	set(${var} "CPACK_COMPONENT_GROUP_${_name}")
	
endmacro(_CPACK_COMPONENT_GROUP_NAME)

###############################################################################
# Makro generuje instalatory
# Parametry:
#		variable - nazwa listy którą będziemy modyfikować
#		type	- typ komponentu: dev lub product
#		[componentsNames] - lista komponentów
macro(_GENERATE_INSTALLERS)

	# czy generowano instalacje
	if(CREATE_INSTALLATION)
	
		# generowanie instalatorów
		CONFIG_OPTION(CREATE_INSTALLERS "Czy konfigurować instalatory?" OFF)
		
		set(SOLUTION_INSTALLERS "")
		
		if(CREATE_INSTALLERS)
		
			foreach(p ${SOLUTION_INSTALLERS_DIRECTORIES})
				file(GLOB_RECURSE installersFiles "${p}/Installer*.cmake")
	
				foreach(f ${installersFiles})
					include(${f})
				endforeach()
	
			endforeach()
			
			foreach(installer ${SOLUTION_INSTALLERS})
			
				_GENERATE_INSTALLER("${installer}")
			
			endforeach()
			
		endif()
	
	endif()
	
endmacro(_GENERATE_INSTALLERS)

###############################################################################
# Makro generuje liste zależnych komponentów na bazie ich nazw
# Parametry:
#		variable - nazwa listy którą będziemy modyfikować
#		type	- typ komponentu: dev lub product
#		[componentsNames] - lista komponentów
macro(_GENERATE_DEPENDENT_COMPONENTS variable type)

	set(${variable} "")
	
	foreach(c ${ARGN})
		_COMPONENT_NAME(__c ${c} ${type})
		if(DEFINED __c)
			list(APPEND ${variable} ${__c})
			unset(__c)
		endif()	
	endforeach()
	
endmacro(_GENERATE_DEPENDENT_COMPONENTS)

###############################################################################
# Makro generuje liste zależnych komponentów na bazie ich nazw
# Parametry:
#		variable - nazwa listy którą będziemy modyfikować
#		type	- typ komponentu: dev lub product
#		[componentsNames] - lista komponentów
macro(_GENERATE_CPACK_DEPENDENT_COMPONENTS variable)

	set(${variable} "")

	foreach(c ${ARGN})
		string(TOUPPER "${c}" c)
		list(APPEND ${variable} "${c}")
	endforeach()
	
endmacro(_GENERATE_CPACK_DEPENDENT_COMPONENTS)

###############################################################################
# Makro dodaje skroty
# Parametry:
#		name - unikalna nazwa skrotu
#		path - sciezka gdzie utworzony zostanie link
#		object - sciezka obiektu w ramach instalacji do ktorego tworzymy link
#		icon - ikona dla skrotu
macro(ADD_STARTMENU_SHORTCUT name path object icon)
	ADD_STARTMENU_SHORTCUT_EXT(${name} "${path}" "${object}" "${icon}" "currentUser")
endmacro(ADD_STARTMENU_SHORTCUT)

###############################################################################
# Makro dodaje skroty
# Parametry:
#		name - unikalna nazwa skrotu
#		path - sciezka gdzie utworzony zostanie link
#		object - sciezka obiektu w ramach instalacji do ktorego tworzymy link
#		icon - ikona dla skrotu
#		userContext - dla jakiego użytkownika to robię: current | all | admin
macro(ADD_STARTMENU_SHORTCUT_EXT name path object icon userContext)

	list(FIND INSTALLER_${INSTALLER_NAME}_STARTMENU_SHORTCUTS ${name} _sIDX)
	
	if(_sIDX EQUAL -1)

		_APPEND_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_STARTMENU_SHORTCUTS ${name} "Installer start menu shourtcuts")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_STARTMENU_SHORTCUT_${name}_PATH "${path}" "Sciezka skrotu")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_STARTMENU_SHORTCUT_${name}_OBJECT "${object}" "Sciezka elementu wskazywanego")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_STARTMENU_SHORTCUT_${name}_ICON "${icon}" "Ikona skrotu")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_STARTMENU_SHORTCUT_${name}_USERCONTEXT "${userContext}" "Kontekst skrotu")
	
	else()
	
		INSTALLER_NOTIFY(INSTALLER_${INSTALLER_NAME}_STARTMENU_SHORTCUTS "Start Menu shourtcut ${name} already registered - skipping.")
		
	endif()
	
endmacro(ADD_STARTMENU_SHORTCUT_EXT)

###############################################################################
# Makro dodaje skroty
# Parametry:
#		name - unikalna nazwa skrotu
#		path - sciezka gdzie utworzony zostanie link
#		object - sciezka obiektu w ramach instalacji do ktorego tworzymy link
#		icon - ikona dla skrotu
macro(ADD_DESKTOP_SHORTCUT name path object icon)
	ADD_DESKTOP_SHORTCUT_EXT(${name} "${path}" "${object}" "${icon}" "currentUser")
endmacro(ADD_DESKTOP_SHORTCUT)

###############################################################################
# Makro dodaje skroty
# Parametry:
#		name - unikalna nazwa skrotu
#		path - sciezka gdzie utworzony zostanie link
#		object - sciezka obiektu w ramach instalacji do ktorego tworzymy link
#		icon - ikona dla skrotu
#		userContext - dla jakiego użytkownika to robię: currentUser | all | admin
macro(ADD_DESKTOP_SHORTCUT_EXT name path object icon userContext)

	list(FIND INSTALLER_${INSTALLER_NAME}_DESKTOP_SHORTCUTS ${name} _sIDX)
	
	if(_sIDX EQUAL -1)

		_APPEND_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_DESKTOP_SHORTCUTS ${name} "Installer desktop shourtcuts")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_DESKTOP_SHORTCUT_${name}_PATH "${path}" "Sciezka skrotu")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_DESKTOP_SHORTCUT_${name}_OBJECT "${object}" "Sciezka elementu wskazywanego")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_DESKTOP_SHORTCUT_${name}_ICON "${icon}" "Ikona skrotu")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_DESKTOP_SHORTCUT_${name}_USERCONTEXT "${userContext}" "Kontekst skrotu")
	
	else()
	
		INSTALLER_NOTIFY(INSTALLER_${INSTALLER_NAME}_DESKTOP_SHORTCUTS "Start Menu shourtcut ${name} already registered - skipping.")
		
	endif()
	
endmacro(ADD_DESKTOP_SHORTCUT_EXT)

###############################################################################
# Makro ustawia krótki opis instalowanego produktu
# Parametry:
#		description - nazwa producenta
macro(SET_INSTALLER_SHORT_DESCRIPTION description)
	
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_SHORT_DESCRIPTION "${description}" "Krótki opis instalowanego produktu")
	
endmacro(SET_INSTALLER_SHORT_DESCRIPTION)

###############################################################################
# Makro ustawia producenta produktu
# Parametry:
#		vendor - nazwa producenta
macro(SET_INSTALLER_VENDOR vendor)
	
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_VENDOR "${vendor}" "Dostawca instalatora/produktu")
	
endmacro(SET_INSTALLER_VENDOR)

###############################################################################
# Makro ustawia wersję produktu
# Parametry:
#		version - wersja produktu
macro(SET_INSTALLER_VERSION version)
	
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_VERSION "${version}" "Wersja instalatora produktu")
	
endmacro(SET_INSTALLER_VERSION)

###############################################################################
# Makro ustawia wersję produktu
# Parametry:
#		version - wersja produktu
macro(SET_INSTALLER_VERSION_EXT major minor patch)
	
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_VERSION_MAJOR "${major}" "Wersja główna instalatora produktu")
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_VERSION_MINOR "${minor}" "Wersja dodatkowa instalatora produktu")
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_VERSION_PATCH "${patch}" "Wersja poprawki instalatora produktu")
	
endmacro(SET_INSTALLER_VERSION_EXT)

###############################################################################
# Makro ustawia licencję produktu
# Parametry:
#		license - ścieżka do pliku z licencją
macro(SET_INSTALLER_LICENSE license)
	
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_LICENSE_FILE "${license}" "Plik licencji")
	
endmacro(SET_INSTALLER_LICENSE)

###############################################################################
# Makro ustawia czy pokazywac lub nie licencje aplikacji
# Parametry:
#		value - wartosc 0 lub 1
macro(SET_INSTALLER_SHOW_LICENSE value)
	
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_SHOW_LICENSE "${value}" "Czy pokazac licencje")
	
endmacro(SET_INSTALLER_SHOW_LICENSE)

###############################################################################
# Makro ustawia powitanie instalatora produktu
# Parametry:
#		welcome - ścieżka do pliku z powitaniem
macro(SET_INSTALLER_WELCOME welcome)

	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_WELCOME_FILE "${welcome}" "Plik z tekstem powitalnym")
	
endmacro(SET_INSTALLER_WELCOME)

###############################################################################
# Makro ustawia krótki opis produktu po instalacji
# Parametry:
#		readme - ścieżka do pliku z opisem
macro(SET_INSTALLER_README readme)

	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_README_FILE "${readme}" "Plik z informacjami")
	
endmacro(SET_INSTALLER_README)

###############################################################################
# Makro ustawia krótki opis produktu w trakcie instalacji
# Parametry:
#		readme - ścieżka do pliku z opisem
macro(SET_INSTALLER_DESCRIPTION description)

	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_DESCRIPTION_FILE "${description}" "Plik z opisem")
	
endmacro(SET_INSTALLER_DESCRIPTION)


###############################################################################
# Makro ustawia ikony instalacji
# Parametry:
#		productIco - ścieżka do ikony produktu
#		uninstallIco - ścieżka do ikony wyinstalowującej produkt
macro(SET_INSTALLER_BRANDING_IMAGE brandingImage)

	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_BRANDING_IMAGE "${brandingImage}" "Plik z obrazkiem instalatora")
	
endmacro(SET_INSTALLER_BRANDING_IMAGE)

###############################################################################
# Makro ustawia ikony instalacji
# Parametry:
#		productIco - ścieżka do ikony produktu
macro(SET_INSTALLER_PRODUCT_ICON productIco)

	SET_INSTALLER_PRODUCT_ICONS(${productIco} ${productIco})
	
endmacro(SET_INSTALLER_PRODUCT_ICON)

###############################################################################
# Makro ustawia ikony instalacji
# Parametry:
#		productIco - ścieżka do ikony produktu
#		uninstallIco - ścieżka do ikony wyinstalowującej produkt
macro(SET_INSTALLER_PRODUCT_ICONS productIco uninstallIco)

	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_PRODUCT_ICON "${productIco}" "Plik z obrazkiem zainstalowanej aplikacji")
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_PRODUCT_UNINSTALL_ICON "${uninstallIco}" "Plik z obrazkiem wyinstalowanej aplikacji")
	
endmacro(SET_INSTALLER_PRODUCT_ICONS)

###############################################################################
# Makro ustawia dodatkowe informacje o produkcie
# Parametry:
#		helpLink - link do pomocy
#		aboutLink - link do strony produktu
#		vendorContact - kontakt do producenta
macro(SET_INSTALLER_ADDITIONAL_INFO helpLink aboutLink vendorContact)	

	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_ADDITIONAL_INFO_HELP_LINK "${helpLink}" "Link do strony z pomocą produktu")
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_ADDITIONAL_INFO_ABOUT_LINK "${aboutLink}" "Link do strony z informacją o produkcie")
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_ADDITIONAL_INFO_VENDOR_CONTACT "${vendorContact}" "Kontakt z producentem")
	
endmacro(SET_INSTALLER_ADDITIONAL_INFO)

###############################################################################
# Makro pozwala uruchomić aplikację po instalacji
# Parametry:
#		app - ścieżka do aplikacji, którą user będzie mógł wystartować po zakończeniu instalacji
macro(SET_INSTALLER_FINISH_RUN_APP app)	

	set(_app "${app}")

	if(DEFINED PROJECT_${app}_TARGETNAME AND "${PROJECT_${app}_TYPE}" STREQUAL "executable")
	
		set(_app "${PROJECT_${app}_TARGETNAME}")
		
	endif()

	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_FINISH_RUN_APP "${_app}" "Aplikacja do uruchomienia po zakończeniu instalacji")
	
endmacro(SET_INSTALLER_FINISH_RUN_APP)

###############################################################################
# Makro ustawia czy instalować tłumaczenia czy nie
# Parametry:
#		value - wartosc 0 lub 1
macro(SET_INSTALLER_INCLUDE_TRANSLATIONS value)
	
	_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_INCLUDE_TRANSLATIONS "${value}" "Czy instalować tłumaczenia")
	
endmacro(SET_INSTALLER_INCLUDE_TRANSLATIONS)

###############################################################################
# Makro pomocnicze przy dodawaniu typów instalacji
# Parametry:
#		name - nazwa typu instalacji
#		displayName - wyświetlana nazwa
macro(ADD_INSTALLATION_TYPE name displayName)
	
	# szukam typu instalacji	
	list(FIND INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPES "${name}" _listIDX)
	
	if(_listIDX GREATER -1)
		# znalazłem - zgłaszam info o pominięciu
		INSTALLER_NOTIFY(INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPE_${name}_DISPLAY "Instalation type ${name} already defined with display name : < ${INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPE_${name}_DISPLAY} >. Skipping...")
	else()
		# nie znalazłem - dodaję
		list(LENGTH INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPES _l)
		if(${_l} GREATER 0)
			_APPEND_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPES "${name}" "Typy instalacji")
		else()
			_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPES "${name}" "Typy instalacji")
		endif()
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPE_${name}_DISPLAY "${displayName}" "Nazwa wyświetlana dla typu instalacji")
	endif()
	
endmacro(ADD_INSTALLATION_TYPE)

###############################################################################
# Makro otwierające grupę dla elementów instalacji
# Parametry:
#		name - nazwa grupy
#		displayName - wyświetlana nazwa
#		description - opis
#		[options] - czy ma być gałąź domyślnie rozwinięta i czy ma byc pogrubiona
macro(BEGIN_INSTALLER_GROUP name displayName description)

	list(FIND INSTALLER_${INSTALLER_NAME}_GROUPS ${name} _groupIDX)	
	
	if(_groupIDX GREATER -1)
		message(FATAL_ERROR "Installation group with name ${name} already defined! Can not create two groups with the same name")
	else()

		_APPEND_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_GROUPS "${name}" "Grupy instalacji")
	
		list(APPEND _CURRENT_INSTALLER_GROUPS ${name})
		
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_GROUP_${name}_DISPLAY "${displayName}" "Nazwa wyświetlana grupy")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_GROUP_${name}_DESCRIPTION "${description}" "Opis grupy")
		
		# sprawdzam czy mam rodzica i ustawiam jeśli trzeba
		list(LENGTH _CURRENT_INSTALLER_GROUPS _groupsLength)
		
		math(EXPR _groupsLength "${_groupsLength} - 2")
		
		if(_groupsLength GREATER -1)
			list(GET _CURRENT_INSTALLER_GROUPS ${_groupsLength} _PARENT)
			_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_GROUP_${name}_PARENT "${_PARENT}" "Rodzic grupy")
		endif()
		
		set(_EXPANDED OFF)
		
		if(${ARGC} GREATER 3)
		
			string(TOUPPER "${ARGN}" _options)
			
			string(FIND "${_options}" "EXPANDED" _foundOption)
		
			if(_foundOption GREATER -1)
				_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_GROUP_${name}_EXPANDED ON "Czy grupa ma byc rozwinieta")
			endif()
			
			string(FIND "${_options}" "BOLD" _foundOption)
		
			if(_foundOption GREATER -1)
				_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_GROUP_${name}_BOLD ON "Czy grupa ma byc wytluszczona")
			endif()
		endif()
		
	endif()
	
endmacro(BEGIN_INSTALLER_GROUP)

###############################################################################
# Makro zamykające grupę dla elementów instalacji
macro(END_INSTALLER_GROUP)

	list(LENGTH _CURRENT_INSTALLER_GROUPS _groupsLength)
	
	if(_groupsLength GREATER 0)
		
		math(EXPR _groupsLength "${_groupsLength} - 1")
		
		list(REMOVE_AT _CURRENT_INSTALLER_GROUPS ${_groupsLength})		
		
	endif()
	
endmacro(END_INSTALLER_GROUP)

###############################################################################
# Makro dodające target/projekt do instalacji
# Parametry:
#		name - nazwa projektu
#		displayName - wyświetlana nazwa
#		description - opis
#		[options] - opcje [HIDDEN - ukryty | REQUIRED - wymagany | DISABLED - domyślnie wyłączony z instalacji]
#		[installTypes] - typy instalacji w jakich ma się pojawić ten projekt
macro(ADD_INSTALLER_GROUP_COMPONENT name displayName description)
	
	list(FIND INSTALLER_${INSTALLER_NAME}_COMPONENTS ${name} _componentIDX)
	
	if(_componentIDX EQUAL -1)
	
		# element jeszcze nie był dodawany - moge konfigurować
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_COMPONENT_${name}_DISPLAY "${displayName}" "Nazwa wyświetlana komponentu")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_COMPONENT_${name}_DESCRIPTION "${description}" "Opis komponentu")		
		
		# ustawiam opcje jeśli podano
		if(${ARGC} GREATER 3)
			_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_COMPONENT_${name}_OPTIONS "${ARGV3}" "Opcje komponentu")			
		endif()		
		
		# ustawiam typy instalacji jesli podano
		if(${ARGC} GREATER 4)
			set(_installTypes "")			
			string(REPLACE " " ";" _inputInstallType "${ARGV4}")
			
			foreach(_it ${_inputInstallType})
				list(FIND INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPES "${_it}" _itFound)
				if(_itFound GREATER -1)
					list(APPEND _installTypes "${_it}")
				else()
					INSTALLER_NOTIFY(name "Install type ${_it} not registered within installer. Skipping this install type for component ${name}")
				endif()
			endforeach()
		
			_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_COMPONENT_${name}_INSTALLTYPES "${_installTypes}" "Typy instalacji komponentu")			
		endif()
		
		# ustawiam grupę jeśli zdefiniowano
		list(LENGTH _CURRENT_INSTALLER_GROUPS _groupsLength)
		
		math(EXPR _groupsLength "${_groupsLength} - 1")
		
		if(_groupsLength GREATER -1)
			list(GET _CURRENT_INSTALLER_GROUPS ${_groupsLength} _GROUP)
			_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_COMPONENT_${name}_GROUP "${_GROUP}" "Grupa komponentu")
		endif()
		
		_APPEND_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_COMPONENTS "${name}" "Komponenty instalatora")
		
	else()
		# komponent był już dodany
		INSTALLER_NOTIFY(name "Element ${name} already configured for installer. Skipping...")
	endif()
	
endmacro(ADD_INSTALLER_GROUP_COMPONENT)

###############################################################################
# Makro rozpoczynające blok generowania instalatora
# Parametry:
#		name - Nazwa instalatora
#		displayName - Nazwa wyświetlana instalatora
#		outputName - Plik wyjściowy instalatora
# 		type - Typ instalatora [product, dev]
macro(BEGIN_INSTALLER name displayName outputName type)

	# jeżeli już generowałem instalację to zgłaszam błąd
	if(DEFINED _INSTALLER_STARTED)
		message(FATAL_ERROR "Installer configuration already started, can not begin new one bofore finishing previous one")
	endif()
	
	list(FIND SOLUTION_INSTALLERS ${name} installerIDX)
	
	if(installerIDX GREATER -1)
		INSTALLER_NOTIFY(name "Installer ${name} was already configured. Skipping...")
	else()

		# zapamiętuje nazwę instalatora
		set(INSTALLER_NAME ${name})
		# nazwa wyświetlana instalatora
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_DISPLAY_NAME "${displayName}" "Wyswietlana nazwa instalatora")
		# zeruję liste aktualnych grup na potrzby obslugi rodzicow
		set(_CURRENT_INSTALLER_GROUPS "")
		# nazwa pliku wyjsviowego instalatora
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_OUTPUT_CONFIG_NAME "${outputName}" "Plik konfiguracyjny generowanego instalatora")		
		# typ instalacji
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_TYPE "${type}" "Typ instalatora")
		# elementy instalacji
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_COMPONENTS "" "Komponenty instalatora")
		# domyslny producent
		SET_INSTALLER_VENDOR("PJWSTK")
		# domyślna wersja
		SET_INSTALLER_VERSION_EXT(0 0 1)
		# domyslne info
		SET_INSTALLER_ADDITIONAL_INFO("http://hm.pjwstk.edu.pl" "http://hmkb.pjwstk.edu.pl" "Marek.Kulbacki@pjwstk.edu.pl")
		# domyslnie nie pokazujemy licencji		
		SET_INSTALLER_SHOW_LICENSE(0)
		# domyślnie nie instalujemy tłumaczeń
		SET_INSTALLER_INCLUDE_TRANSLATIONS(0)
		
		set(STARTMENU_SHORTCUTS "")
		set(DESKTOP_SHORTCUTS "")
		
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_INSTALLATION_TYPES "" "Typy instalacji")
		_SETUP_INTERNAL_CACHE_VALUE(INSTALLER_${INSTALLER_NAME}_GROUPS "" "Grupy instalacji")		
		
		# zapamiętuję że już rozpoczalem konfigurację instalacji
		set(_INSTALLER_STARTED 1)
		
		CONFIG_OPTION(INSTALLER_${INSTALLER_NAME}_FORCE_UNINSTALL "Czy przed instalacja aplikacja musi byc odinstalowana" ON)
	
	endif()
	
endmacro(BEGIN_INSTALLER)

###############################################################################
# Makro kończące blok konfigurowania instalacji
macro(END_INSTALLER)

	if(NOT DEFINED _INSTALLER_STARTED OR _INSTALLER_STARTED EQUAL 0)
		message(FATAL_ERROR "Installer was not initialized properly - please ensure macro BEGIN_INSTALLER has been invoked previously")
	endif()
	
	unset(_INSTALLER_STARTED)
	
	list(APPEND SOLUTION_INSTALLERS ${INSTALLER_NAME})
	
endmacro(END_INSTALLER)

###############################################################################
# Makro czyści ogólne zmienne CPacka
macro(_CLEAR_CPACK_COMMON_VARIABLES)

	_CLEAR_VARIABLES(
		CPACK_ABSOLUTE_DESTINATION_FILES
		CPACK_CMAKE_GENERATOR
		CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY
		CPACK_CREATE_DESKTOP_LINKS
		CPACK_GENERATOR
		CPACK_INCLUDE_TOPLEVEL_DIRECTORY
		CPACK_INSTALLED_DIRECTORIES
		CPACK_INSTALL_CMAKE_PROJECTS
		CPACK_INSTALL_COMMANDS
		CPACK_INSTALL_SCRIPT
		CPACK_MONOLITHIC_INSTALL
		CPACK_OUTPUT_CONFIG_FILE
		CPACK_PACKAGE_DESCRIPTION_FILE
		CPACK_PACKAGE_DESCRIPTION_SUMMARY
		CPACK_PACKAGE_DIRECTORY
		CPACK_PACKAGE_EXECUTABLES
		CPACK_PACKAGE_FILE_NAME
		CPACK_PACKAGE_ICON
		CPACK_PACKAGE_INSTALL_DIRECTORY
		CPACK_PACKAGE_INSTALL_REGISTRY_KEY
		CPACK_PACKAGE_NAME
		CPACK_PACKAGE_VENDOR
		CPACK_PACKAGE_VERSION
		CPACK_PACKAGE_VERSION_MAJOR
		CPACK_PACKAGE_VERSION_MINOR
		CPACK_PACKAGE_VERSION_PATCH
		CPACK_PACKAGING_INSTALL_PREFIX
		CPACK_PROJECT_CONFIG_FILE
		CPACK_RESOURCE_FILE_LICENSE
		CPACK_RESOURCE_FILE_README
		CPACK_RESOURCE_FILE_WELCOME
		CPACK_SET_DESTDIR
		CPACK_SOURCE_GENERATOR
		CPACK_SOURCE_IGNORE_FILES
		CPACK_SOURCE_OUTPUT_CONFIG_FILE
		CPACK_SOURCE_PACKAGE_FILE_NAME
		CPACK_SOURCE_STRIP_FILES
		CPACK_STRIP_FILES
		CPACK_SYSTEM_NAME
		CPACK_TOPLEVEL_TAG
		CPACK_WARN_ON_ABSOLUTE_INSTALL_DESTINATION
	)

endmacro(_CLEAR_CPACK_COMMON_VARIABLES)

###############################################################################
# Makro czyści zmienne odpowiedzialne za instalowane komponenty
macro(_CLEAR_CPACK_COMPONENTS_VARIABLES)

	foreach(comp ${CPACK_COMPONENTS_ALL})
		_CLEAR_VARIABLES(
			CPACK_COMPONENT_${comp}_DEPENDS
			CPACK_COMPONENT_${comp}_DESCRIPTION
			CPACK_COMPONENT_${comp}_DISPLAY_NAME
			CPACK_COMPONENT_${comp}_GROUP
			CPACK_COMPONENT_${comp}_REQUIRED
		)
	endforeach()
	
	_CLEAR_VARIABLES(
		CPACK_COMPONENTS_ALL
		CPACK_COMPONENTS_GROUPING
	)

endmacro(_CLEAR_CPACK_COMPONENTS_VARIABLES)

###############################################################################
# Makro czyści zmienne generatora Bundle
macro(_CLEAR_CPACK_BUNDLE_GENERATOR)

	_CLEAR_VARIABLES(	
		CPACK_BUNDLE_ICON
		CPACK_BUNDLE_NAME
		CPACK_BUNDLE_PLIST
		CPACK_BUNDLE_STARTUP_COMMAND
	)

endmacro(_CLEAR_CPACK_BUNDLE_GENERATOR)

###############################################################################
# Makro czyści zmienne generatora Cygwin
macro(_CLEAR_CPACK_CYGWIN_GENERATOR)

	_CLEAR_VARIABLES(
		CPACK_CYGWIN_BUILD_SCRIPT
		CPACK_CYGWIN_PATCH_FILE
		CPACK_CYGWIN_PATCH_NUMBER
	)

endmacro(_CLEAR_CPACK_CYGWIN_GENERATOR)

###############################################################################
# Makro czyści zmienne generatora Debian
macro(_CLEAR_CPACK_DEBIAN_GENERATOR)

	_CLEAR_VARIABLES(
		CPACK_DEBIAN_PACKAGE_ARCHITECTURE
		CPACK_DEBIAN_PACKAGE_BREAKS
		CPACK_DEBIAN_PACKAGE_CONFLICTS
		CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
		CPACK_DEBIAN_PACKAGE_DEBUG
		CPACK_DEBIAN_PACKAGE_DEPENDS
		CPACK_DEBIAN_PACKAGE_DESCRIPTION
		CPACK_DEBIAN_PACKAGE_ENHANCES
		CPACK_DEBIAN_PACKAGE_HOMEPAGE
		CPACK_DEBIAN_PACKAGE_MAINTAINER
		CPACK_DEBIAN_PACKAGE_NAME
		CPACK_DEBIAN_PACKAGE_PREDEPENDS
		CPACK_DEBIAN_PACKAGE_PRIORITY
		CPACK_DEBIAN_PACKAGE_PROVIDES
		CPACK_DEBIAN_PACKAGE_RECOMMENDS
		CPACK_DEBIAN_PACKAGE_REPLACES
		CPACK_DEBIAN_PACKAGE_SECTION
		CPACK_DEBIAN_PACKAGE_SHLIBDEPS
		CPACK_DEBIAN_PACKAGE_SUGGESTS
		CPACK_DEBIAN_PACKAGE_VERSION
	)

endmacro(_CLEAR_CPACK_DEBIAN_GENERATOR)

###############################################################################
# Makro czyści zmienne generatora DragNDrop
macro(_CLEAR_CPACK_DRAGNDROP_GENERATOR)

	_CLEAR_VARIABLES(
		CPACK_COMMAND_HDIUTIL
		CPACK_COMMAND_REZ
		CPACK_COMMAND_SETFILE
		CPACK_DMG_BACKGROUND_IMAGE
		CPACK_DMG_DS_STORE
		CPACK_DMG_FORMAT
		CPACK_DMG_VOLUME_NAME
	)

endmacro(_CLEAR_CPACK_DRAGNDROP_GENERATOR)

###############################################################################
# Makro czyści zmienne generatora NSIS
macro(_CLEAR_CPACK_NSIS_GENERATOR)

	_CLEAR_VARIABLES(
		CPACK_NSIS_COMPRESSOR
		CPACK_NSIS_CONTACT
		CPACK_NSIS_CREATE_ICONS_EXTRA
		CPACK_NSIS_DELETE_ICONS_EXTRA
		CPACK_NSIS_DISPLAY_NAME
		CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL
		CPACK_NSIS_EXECUTABLES_DIRECTORY
		CPACK_NSIS_EXTRA_INSTALL_COMMANDS
		CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS
		CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
		CPACK_NSIS_HELP_LINK
		CPACK_NSIS_INSTALLED_ICON_NAME
		CPACK_NSIS_INSTALLER_MUI_ICON_CODE
		CPACK_NSIS_INSTALL_ROOT
		CPACK_NSIS_MENU_LINKS
		CPACK_NSIS_MODIFY_PATH
		CPACK_NSIS_MUI_FINISHPAGE_RUN
		CPACK_NSIS_MUI_ICON
		CPACK_NSIS_MUI_UNIICON
		CPACK_NSIS_PACKAGE_NAME
		CPACK_NSIS_URL_INFO_ABOUT
	)

endmacro(_CLEAR_CPACK_NSIS_GENERATOR)

###############################################################################
# Makro czyści zmienne generatora OSX PackageGenerator
macro(_CLEAR_CPACK_OSXPACKAGEGENERATOR_GENERATOR)

	_CLEAR_VARIABLES(
		CPACK_OSX_PACKAGE_VERSION
	)

endmacro(_CLEAR_CPACK_OSXPACKAGEGENERATOR_GENERATOR)

###############################################################################
# Makro czyści zmienne generatora RPM
macro(_CLEAR_CPACK_RPM_GENERATOR)

	_CLEAR_VARIABLES(
		CPACK_RPM_CHANGELOG_FILE
		CPACK_RPM_COMPRESSION_TYPE
		CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST
		CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
		CPACK_RPM_GENERATE_USER_BINARY_SPECFILE_TEMPLATE
		CPACK_RPM_PACKAGE_ARCHITECTURE
		CPACK_RPM_PACKAGE_DEBUG
		CPACK_RPM_PACKAGE_DESCRIPTION
		CPACK_RPM_PACKAGE_GROUP
		CPACK_RPM_PACKAGE_LICENSE
		CPACK_RPM_PACKAGE_NAME
		CPACK_RPM_PACKAGE_OBSOLETES
		CPACK_RPM_PACKAGE_PROVIDES
		CPACK_RPM_PACKAGE_RELEASE
		CPACK_RPM_PACKAGE_RELOCATABLE
		CPACK_RPM_PACKAGE_REQUIRES
		CPACK_RPM_PACKAGE_SUGGESTS
		CPACK_RPM_PACKAGE_SUMMARY
		CPACK_RPM_PACKAGE_URL
		CPACK_RPM_PACKAGE_VENDOR
		CPACK_RPM_PACKAGE_VERSION
		CPACK_RPM_POST_INSTALL_SCRIPT_FILE
		CPACK_RPM_PRE_INSTALL_SCRIPT_FILE
		CPACK_RPM_SPEC_INSTALL_POST
		CPACK_RPM_SPEC_MORE_DEFINE
		CPACK_RPM_USER_BINARY_SPECFILE
		CPACK_RPM_USER_FILELIST
	)

endmacro(_CLEAR_CPACK_RPM_GENERATOR)

###############################################################################
# Makro czyści zmienne generatora WiX
macro(_CLEAR_CPACK_WIX_GENERATOR)

	_CLEAR_VARIABLES(
		CPACK_WIX_CULTURES
		CPACK_WIX_LICENSE_RTF
		CPACK_WIX_PRODUCT_GUID
		CPACK_WIX_PRODUCT_ICON
		CPACK_WIX_PROGRAM_MENU_FOLDER
		CPACK_WIX_TEMPLATE
		CPACK_WIX_UI_BANNER
		CPACK_WIX_UI_DIALOG
		CPACK_WIX_UPGRADE_GUID
	)

endmacro(_CLEAR_CPACK_WIX_GENERATOR)

###############################################################################
# Makro czyści wszystkie zmienne CPacka
macro(_CLEAR_CPACK_VARIABLES)

    _CLEAR_CPACK_COMMON_VARIABLES()
	_CLEAR_CPACK_COMPONENTS_VARIABLES()
	_CLEAR_CPACK_BUNDLE_GENERATOR()
	_CLEAR_CPACK_CYGWIN_GENERATOR()
	_CLEAR_CPACK_DEBIAN_GENERATOR()
	_CLEAR_CPACK_DRAGNDROP_GENERATOR()
	_CLEAR_CPACK_NSIS_GENERATOR()
	_CLEAR_CPACK_OSXPACKAGEGENERATOR_GENERATOR()
	_CLEAR_CPACK_RPM_GENERATOR()
	_CLEAR_CPACK_WIX_GENERATOR()

endmacro(_CLEAR_CPACK_VARIABLES)

###############################################################################
# Makro generujace plik konfiguracyjny instalatora
# Parametry:
#	name - nazwa instalatora
function(_GENERATE_INSTALLER name)
	
	set(CPACK_PACKAGE_NAME "${name}")	
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "${name}")
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${name}")
	set(CPACK_PACKAGE_FILE_NAME "${name}")
	
	set(CPACK_PACKAGE_VENDOR "${INSTALLER_${name}_VENDOR}")		
	set(CPACK_PACKAGE_VERSION "${INSTALLER_${name}_VERSION}")
	set(CPACK_PACKAGE_MAJOR "${INSTALLER_${name}_VERSION_MAJOR}")
	set(CPACK_PACKAGE_MINOR "${INSTALLER_${name}_VERSION_MINOR}")
	set(CPACK_PACKAGE_PATCH "${INSTALLER_${name}_VERSION_PATCH}")
	
	if(IS_ABSOLUTE "${INSTALLER_${name}_OUTPUT_CONFIG_NAME}")
		set(CPACK_OUTPUT_CONFIG_FILE "${INSTALLER_${name}_OUTPUT_CONFIG_NAME}")
	elseif(DEFINED SOLUTION_INSTALLERS_OUTPUT_PATH AND EXISTS "${SOLUTION_INSTALLERS_OUTPUT_PATH}")
		set(CPACK_OUTPUT_CONFIG_FILE "${SOLUTION_INSTALLERS_OUTPUT_PATH}/${INSTALLER_${name}_OUTPUT_CONFIG_NAME}")
	else()
		set(CPACK_OUTPUT_CONFIG_FILE "${INSTALLER_${name}_OUTPUT_CONFIG_NAME}")
	endif()
		
	_SETUP_ABSOLUTE_PATH(CPACK_RESOURCE_FILE_LICENSE INSTALLER_${name}_LICENSE_FILE)
	_SETUP_ABSOLUTE_PATH(CPACK_RESOURCE_FILE_WELCOME INSTALLER_${name}_WELCOME_FILE)
	_SETUP_ABSOLUTE_PATH(CPACK_RESOURCE_FILE_README INSTALLER_${name}_README_FILE)
	_SETUP_ABSOLUTE_PATH(CPACK_PACKAGE_DESCRIPTION_FILE INSTALLER_${name}_DESCRIPTION_FILE)

	# konfigurujemy grupy, typy instalacji, przynależność do grup, zależności
	
	if(WIN32)
	
		if(${INSTALLER_${name}_SHOW_LICENSE} EQUAL 0)
			_SETUP_INTERNAL_CACHE_VALUE(CPACK_NSIS_CUSTOM_SHOW_LICENSE_CODE "Abort" "Kod anulujący stronę z licencją")
			set_property(CACHE CPACK_NSIS_CUSTOM_SHOW_LICENSE_CODE PROPERTY STRINGS "Abort")
		else()
			unset(CPACK_NSIS_CUSTOM_SHOW_LICENSE_CODE CACHE)
		endif()
	
		# Very important part! CPACK_MONOLITHIC_INSTALL should never be defined for NSIS generator
		if(DEFINED CPACK_MONOLITHIC_INSTALL)
			INSTALLER_NOTIFY(CPACK_MONOLITHIC_INSTALL "CPACK_MONOLITHIC_INSTALL defined! For windows NSIS installer generator this value should not be defined! Removing CPACK_MONOLITHIC_INSTALL from variables")
			unset(CPACK_MONOLITHIC_INSTALL)
		endif()
		
		set(CPACK_NSIS_COMPONENT_INSTALL ON)			
	
		set(CPACK_NSIS_DISPLAY_NAME "${INSTALLER_${name}_DISPLAY_NAME}")
		set(CPACK_NSIS_PACKAGE_NAME "${name}")
		set(CPACK_NSIS_COMPRESSOR lzma)
		SET(CPACK_NSIS_MODIFY_PATH ON)
	
		#_SETUP_PATH(CPACK_NSIS_INSTALLED_ICON_NAME INSTALLER_${name}_PRODUCT_ICON)
		#_SETUP_PATH(CPACK_NSIS_MUI_ICON INSTALLER_${name}_PRODUCT_ICON)
		#_SETUP_PATH(CPACK_NSIS_MUI_UNIICON INSTALLER_${name}_PRODUCT_UNINSTALL_ICON)
		#_SETUP_VALUE(CPACK_PACKAGE_ICON INSTALLER_${name}_BRANDING_IMAGE)
		
		_SETUP_VALUE(CPACK_NSIS_HELP_LINK INSTALLER_${name}_ADDITIONAL_INFO_HELP_LINK)
		_SETUP_VALUE(CPACK_NSIS_URL_INFO_ABOUT INSTALLER_${name}_ADDITIONAL_INFO_ABOUT_LINK CPACK_NSIS_URL_INFO_ABOUT)
		_SETUP_VALUE(CPACK_NSIS_CONTACT INSTALLER_${name}_ADDITIONAL_INFO_VENDOR_CONTACT)
		_SETUP_VALUE(CPACK_PACKAGE_DESCRIPTION_SUMMARY INSTALLER_${INSTALLER_NAME}_SHORT_DESCRIPTION)
		_SETUP_VALUE(CPACK_NSIS_MUI_FINISHPAGE_RUN INSTALLER_${name}_FINISH_RUN_APP)
		
		set(_tmpModulePath ${CMAKE_MODULE_PATH})
		set(CMAKE_MODULE_PATH ${CMAKE_ORIGINAL_MODULE_PATH})

		set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "")
		set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "")
		set(CPACK_NSIS_CREATE_ICONS_EXTRA "")
		set(CPACK_NSIS_DELETE_ICONS_EXTRA "")
		
		_SETUP_VALUE(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL CONFIG_INSTALLER_${name}_FORCE_UNINSTALL)
		
		foreach(s ${INSTALLER_${name}_STARTMENU_SHORTCUTS})		
		
			NSIS_SWITCH_USER_CONTEXT(userContext "${INSTALLER_${name}_STARTMENU_SHORTCUT_${s}_USERCONTEXT}")
			NSIS_STARTMENU_PATH(shortcutPath "${INSTALLER_${name}_STARTMENU_SHORTCUT_${s}_PATH}")
			NSIS_CONVERT_PATH(shortcutPath "${shortcutPath}")			
			NSIS_CONVERT_PATH(objectPath "${objectPath}")
			NSIS_CONVERT_PATH(iconPath "${INSTALLER_${INSTALLER_NAME}_STARTMENU_SHORTCUT_${name}_ICON}")
			NSIS_INSTALLER_SHORTCUT_EXT(installShourtcutCommand uninstalShourtcutCommand "${shortcutPath}" "${objectPath}" "${iconPath}")			
			
			list(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA " ${userContext} ${installShourtcutCommand}")
			list(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA " ${userContext} ${uninstalShourtcutCommand}")
		
		endforeach()
		
		foreach(s ${INSTALLER_${name}_DESKTOP_SHORTCUTS})
		
			NSIS_SWITCH_USER_CONTEXT(userContext "${INSTALLER_${name}_DESKTOP_SHORTCUT_${s}_USERCONTEXT}")
			NSIS_DESKTOP_PATH(shortcutPath "${INSTALLER_${name}_DESKTOP_SHORTCUT_${s}_PATH}")
			NSIS_CONVERT_PATH(shortcutPath "${shortcutPath}")			
			NSIS_CONVERT_PATH(objectPath "${objectPath}")
			NSIS_CONVERT_PATH(iconPath "${INSTALLER_${INSTALLER_NAME}_DESKTOP_SHORTCUT_${name}_ICON}")
			NSIS_INSTALLER_SHORTCUT_EXT(installShourtcutCommand uninstalShourtcutCommand "${shortcutPath}" "${objectPath}" "${iconPath}")			
			
			list(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA " ${userContext} ${installShourtcutCommand}")
			list(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA " ${userContext} ${uninstalShourtcutCommand}")
		
		endforeach()
		
		# modyfikowalne zasoby aplikacji
		foreach(prj ${INSTALLER_${name}_COMPONENTS})
			_INSTALL_NSIS_MODIFYABLE_RESOURCES("${PROJECT_${prj}_DEPLOY_RESOURCES_PATH}" "${CPACK_PACKAGE_VENDOR}/${name}" ${PROJECT_${prj}_DEPLOY_MODIFIABLE_RESOURCES})
		endforeach()
		
	elseif(UNIX)
	
		# dla unix domyslnie deb
		# TODO
		# dodać mechanizm zeby to zmienić!
	
		set(CPACK_BINARY_DEB ON)
		set(CPACK_BINARY_RPM OFF)
		set(CPACK_BINARY_STGZ OFF)
		set(CPACK_BINARY_TBZ2 OFF)
		set(CPACK_BINARY_TGZ OFF)
		set(CPACK_BINARY_TZ OFF)
		
	elseif(OSX)
	
		_SETUP_PATH(CPACK_BUNDLE_ICON INSTALLER_${name}_PRODUCT_ICON)
		set(CPACK_BUNDLE_NAME "${CPACK_PACKAGE_NAME}")

		# TODO
		# zainicjować poprawnie CPACK_BUNDLE_PLIST i CPACK_BUNDLE_STARTUP_COMMAND wg dokumentacji dla generatora bundle
	
	else()
	
		# TODO
		# dodać inicjalizację dla Cygwin, DragNDrop, PackageManager, RPM, WiX
		
	endif()
	
	set(CPACK_COMPONENTS_ALL "")
	
	# typy instalacji
	foreach(installType ${INSTALLER_${name}_INSTALLATION_TYPES})
		_CPACK_ADD_INSTALL_TYPE("${installType}" "${INSTALLER_${name}_INSTALLATION_TYPE_${installType}_DISPLAY}")		
	endforeach()
	
	# grupy
	foreach(group ${INSTALLER_${name}_GROUPS})
		
		_CPACK_COMPONENT_GROUP_NAME(_groupName "${group}")
		
		_SETUP_VALUE("${_groupName}_EXPANDED" INSTALLER_${name}_GROUP_${group}_EXPANDED)
		_SETUP_VALUE("${_groupName}_BOLD_TITLE" INSTALLER_${name}_GROUP_${group}_BOLD)
		_SETUP_VALUE("${_groupName}_PARENT" INSTALLER_${name}_GROUP_${group}_PARENT)
		_SETUP_VALUE("${_groupName}_DISPLAY_NAME" INSTALLER_${name}_GROUP_${group}_DISPLAY)
		_SETUP_VALUE("${_groupName}_DESCRIPTION" INSTALLER_${name}_GROUP_${group}_DESCRIPTION)	
		
	endforeach()
	
	set(INSTALLED_RAW_COMPONENTS "")
	
	# komponenty grup
	foreach(prj ${INSTALLER_${name}_COMPONENTS})
	
		# komponent wg typu instalacji - produkt czy dev
		_COMPONENT_NAME(_componentName "${prj}" "${INSTALLER_${name}_TYPE}")
		
		if(NOT DEFINED _componentName)
			# TODO
			# error - nie potrafię odtworzyc nazwy komponentu
		endif()
		
		_CPACK_COMPONENT_NAME(_CPackComponentName "${_componentName}")		
		
		#TODO buforować wyniki i ustawiać kiedy faktycznie instalujemy zależność
		
		if(DEFINED INSTALLER_${name}_COMPONENT_${prj}_OPTIONS)
			
			string(TOUPPER "${INSTALLER_${name}_COMPONENT_${prj}_OPTIONS}" _options)
			
			string(FIND "${_options}" "HIDDEN" _hiddenFound)
			
			if(${_hiddenFound} GREATER -1)
				set(${_CPackComponentName}_HIDDEN ON)
			endif()
			
			string(FIND "${_options}" "REQUIRED" _requiredFound)
			
			if(${_requiredFound} GREATER -1)
				set(${_CPackComponentName}_REQUIRED ON)
			endif()
			
			string(FIND "${_options}" "DISABLED" _disabledFound)
			
			if(${_disabledFound} GREATER -1)
				if(${_requiredFound} GREATER -1)
					INSTALLER_NOTIFY(prj "Component ${prj} marked as both: REQUIRED and DISABLED, which are contrary options. Skipping DISABLED option...")
				else()
					set(${_CPackComponentName}_DISABLED ON)
				endif()
			endif()
					
		# TODO
		#ARCHIVE_FILE			
		#DOWNLOADED
		endif()
		
		string(TOUPPER "${INSTALLER_${name}_COMPONENT_${prj}_INSTALLTYPES}" _installTypes)
		string(TOUPPER "${INSTALLER_${name}_COMPONENT_${prj}_GROUP}" _group)
		
		if(${INSTALLER_${name}_INCLUDE_TRANSLATIONS} AND DEFINED PROJECT_${prj}_TRANSLATIONS)
			_PROJECT_TRANSLATION_COMPONENT_NAME(_translationComponentName "${prj}" "${INSTALLER_${name}_TYPE}")
			list(APPEND CPACK_COMPONENTS_ALL ${_translationComponentName})
			_CPACK_COMPONENT_NAME(_CPackTranslationComponentName "${_translationComponentName}")
			_SETUP_VALUE(${_CPackTranslationComponentName}_INSTALL_TYPES _installTypes)
			_SETUP_VALUE(${_CPackTranslationComponentName}_DISPLAY_NAME "Translations for project ${prj}")
			_SETUP_VALUE(${_CPackTranslationComponentName}_DESCRIPTION "Translations for project ${prj}")
			_SETUP_VALUE(${_CPackTranslationComponentName}_GROUP _group)
			set(${_CPackTranslationComponentName}_HIDDEN ON)
			if(DEFINED ${_CPackComponentName}_REQUIRED)
				set(${_CPackTranslationComponentName}_REQUIRED ON)
			endif()
		endif()
		
		IS_PROJECT_INSTALLABLE(_prjInstallable "${INSTALLER_${name}_TYPE}" ${prj})
		
		set(_isInstallable 0)
		
		if(_prjInstallable EQUAL 1)
			list(APPEND CPACK_COMPONENTS_ALL ${_componentName})		
			list(LENGTH PROJECT_${prj}_DEPENDENCIES _depLength)
			
			if(_depLength GREATER 0)
			
				set(INSTALLABLE_${prj}_COMPONENTS "")
			
				foreach(ld ${PROJECT_${prj}_DEPENDENCIES})
						
					IS_INSTALLABLE(_isInstallable "${INSTALLER_${name}_TYPE}" "${ld}")
					
					if(_isInstallable EQUAL 1)
						list(APPEND INSTALLABLE_${prj}_COMPONENTS "${ld}")
					else()
						list(APPEND INSTALLABLE_${prj}_COMPONENTS ${INSTALLABLE_${ld}_COMPONENTS})
					endif()
				
				endforeach()
				
				list(REMOVE_DUPLICATES INSTALLABLE_${prj}_COMPONENTS)
			
				_GENERATE_DEPENDENT_COMPONENTS(COMPONENT_DEPENDS ${INSTALLER_${name}_TYPE} ${INSTALLABLE_${prj}_COMPONENTS})
				_GENERATE_CPACK_DEPENDENT_COMPONENTS(COMPONENT_DEPENDS ${COMPONENT_DEPENDS})		
				_SETUP_VALUE(${_CPackComponentName}_DEPENDS COMPONENT_DEPENDS)				
				
			endif()			
			_SETUP_VALUE(${_CPackComponentName}_INSTALL_TYPES _installTypes)
			_SETUP_VALUE(${_CPackComponentName}_DISPLAY_NAME INSTALLER_${name}_COMPONENT_${prj}_DISPLAY)
			_SETUP_VALUE(${_CPackComponentName}_DESCRIPTION INSTALLER_${name}_COMPONENT_${prj}_DESCRIPTION)			
			_SETUP_VALUE(${_CPackComponentName}_GROUP _group)
			
			if(${INSTALLER_${name}_INCLUDE_TRANSLATIONS} AND DEFINED PROJECT_${prj}_TRANSLATIONS)				
				_SETUP_VALUE(${_CPackTranslationComponentName}_DEPENDS ${_CPackComponentName})				
			endif()			
		endif()
		
		set(toVerify ${PROJECT_${prj}_DEPENDENCIES})
		
		list(LENGTH toVerify _continueCheck)
		
		while(_continueCheck GREATER 0)
		
			set(tmpToVerify ${toVerify})
			set(toVerify "")
		
			# generuje liste brakujacych zaleznosci - moga to byc zewnetrzne biblioteki albo nasze projeky ktorych jawnie nie dodano do instalatora!
			foreach(dep ${tmpToVerify})				
			
				# nie ma zaleznosci albo wszystkie juz dodano - moge dodawac bez problemu
				_COMPONENT_NAME(_locComponentName "${dep}" "${INSTALLER_${name}_TYPE}")							
				_CPACK_COMPONENT_NAME(_locCPackComponentName "${_locComponentName}")
				
				# sprawdzam czy juz czasem tego nie obsluzylismy? lub ma byc wymagane a nie byl wymagany
				list(FIND INSTALLED_RAW_COMPONENTS ${dep} _depIDX)
				if(_depIDX EQUAL -1 OR (DEFINED ${_CPackComponentName}_REQUIRED AND NOT DEFINED ${_locCPackComponentName}_REQUIRED))
				
					# zaleznosci tej zaleznosci
					set(depDependencies "")
					# czy zaleznosc jest instalowalna
					set(_depInstallable 0)
					
					# musze sprawdzic czy to projekt - nie ma go na liscie do instalacji ale aktualny projekt jest od niego zalezny!
					list(FIND SOLUTION_PROJECTS ${dep} _depIDX)
					
					if(_depIDX GREATER -1)							
						# projekt
						
						if(${INSTALLER_${name}_INCLUDE_TRANSLATIONS} AND DEFINED PROJECT_${dep}_TRANSLATIONS)
							_PROJECT_TRANSLATION_COMPONENT_NAME(_depTranslationComponentName "${dep}" "${INSTALLER_${name}_TYPE}")
						endif()
						
						# musze sprawdzic jego zaleznosci							
						list(APPEND depDependencies ${PROJECT_${dep}_DEPENDENCIES})
						IS_PROJECT_INSTALLABLE(_depInstallable "${INSTALLER_${name}_TYPE}" ${dep})
					
					else()
						# biblioteka
						# musze sprawdzic jej zaleznosci
						
						if(${INSTALLER_${name}_INCLUDE_TRANSLATIONS} AND DEFINED LIBRARY_${dep}_TRANSLATIONS)
							_LIBRARY_TRANSLATION_COMPONENT_NAME(_depTranslationComponentName "${dep}" "${INSTALLER_${name}_TYPE}")
						endif()
						
						if(DEFINED LIBRARY_${dep}_DEPENDENCIES)
							list(APPEND depDependencies ${LIBRARY_${dep}_DEPENDENCIES})								
						endif()
						
						if(DEFINED LIBRARY_${dep}_PREREQUISITES)
							list(APPEND depDependencies ${LIBRARY_${dep}_PREREQUISITES})
						endif()							
						
						IS_LIBRARY_INSTALLABLE(_depInstallable "${INSTALLER_${name}_TYPE}" ${dep})
						
					endif()
					
					if(DEFINED _depTranslationComponentName)
						_CPACK_COMPONENT_NAME(_CPackTranslationDepComponentName "${_depTranslationComponentName}")
						set(${_CPackTranslationDepComponentName}_REQUIRED ON)
						unset(_depTranslationComponentName)
					endif()
					
					set(_depsOK 1)						
							
					foreach(ld ${depDependencies})
					
						list(FIND INSTALLED_RAW_COMPONENTS ${ld} _ldIDX)
						
						if(_ldIDX EQUAL -1)
						
							list(FIND INSTALLER_${name}_COMPONENTS ${ld} _ldIDX)
							
							if(_ldIDX EQUAL -1)
						
								# tej zaleznosci jeszcze nie dodawalem
								set(_depsOK 0)
								list(APPEND toVerify ${ld})									
							endif()
							
						endif()
					
					endforeach()
					
					if(_depsOK EQUAL 1)
						
						set(INSTALLABLE_${dep}_COMPONENTS "")
						
						foreach(ld ${depDependencies})
					
							IS_INSTALLABLE(_isInstallable "${INSTALLER_${name}_TYPE}" "${ld}")
							
							if(_isInstallable EQUAL 1)
								list(APPEND INSTALLABLE_${dep}_COMPONENTS "${ld}")
							else()
								list(APPEND INSTALLABLE_${dep}_COMPONENTS ${INSTALLABLE_${ld}_COMPONENTS})
							endif()
						
						endforeach()
						
						list(REMOVE_DUPLICATES INSTALLABLE_${dep}_COMPONENTS)
						
						list(LENGTH INSTALLABLE_${dep}_COMPONENTS _depLength)
						
						set(_depComponents "")
						
						if(_depLength GREATER 0)							
							_GENERATE_DEPENDENT_COMPONENTS(_depComponents ${INSTALLER_${name}_TYPE} ${INSTALLABLE_${dep}_COMPONENTS})								
							_GENERATE_CPACK_DEPENDENT_COMPONENTS(_depComponents ${_depComponents})
							
							if(DEFINED ${_CPackComponentName}_REQUIRED)
								set(_allRequiredComponents ${INSTALLABLE_${dep}_COMPONENTS})
								set(_reqToVerify ${INSTALLABLE_${dep}_COMPONENTS})								
								list(LENGTH _reqToVerify _l)
								while(_l GREATER 0)
									set(_tmpReqToVerify ${_reqToVerify})
									set(_reqToVerify "")
									foreach(ic ${_tmpReqToVerify})
										list(FIND _allRequiredComponents ${ic} _found)
										if(_found EQUAL -1 AND DEFINED INSTALLABLE_${dep}_COMPONENTS)
											list(APPEND _reqToVerify ${INSTALLABLE_${dep}_COMPONENTS})
											list(APPEND _allRequiredComponents ${INSTALLABLE_${dep}_COMPONENTS})
										endif()										
									endforeach()
									list(LENGTH _reqToVerify _l)
								endwhile()
								
								list(REMOVE_DUPLICATES _allRequiredComponents)
								
								foreach(ic ${_allRequiredComponents})
									_COMPONENT_NAME(_icComponentName "${ic}" "${INSTALLER_${name}_TYPE}")							
									_CPACK_COMPONENT_NAME(_icCPackComponentName "${_icComponentName}")
									
									set(${_icCPackComponentName}_REQUIRED ON)
								endforeach()
							endif()
							
						endif()
						
					endif()
				
					# sprawdzam czy juz czasem tego nie obsluzylismy? jak tak to pomijamy
					list(FIND INSTALLED_RAW_COMPONENTS ${dep} _depIDX)
					if(_depIDX EQUAL -1)
						#sprawdzam czy moze to nie jest itak element instalatora do sprawdzenia pozniej? jak tak to pomijam
						
						list(FIND INSTALLER_${name}_COMPONENTS ${dep} _depIDX)
						if(_depIDX EQUAL -1)
							
							if(_depsOK EQUAL 1)
							
								list(APPEND INSTALLED_RAW_COMPONENTS ${dep})
								
								if(_depInstallable EQUAL 1)
									
									set("${_locCPackComponentName}_HIDDEN" ON)								
									_SETUP_VALUE(${_locCPackComponentName}_DEPENDS _depComponents)							
									
									list(APPEND CPACK_COMPONENTS_ALL ${_locComponentName})						
								
								endif()
								# to już niezależnie ponieważ tych plików nie instalujemy klasycznie poprzez install(...)
								# zaleznosci tez moga byc projektami i miec jakies zasoby edytowalne do instalacji
								if(WIN32 AND DEFINED PROJECT_${dep}_DEPLOY_MODIFIABLE_RESOURCES)										
									_INSTALL_NSIS_MODIFYABLE_RESOURCES("${PROJECT_${dep}_DEPLOY_RESOURCES_PATH}" "${CPACK_PACKAGE_VENDOR}/${name}" ${PROJECT_${dep}_DEPLOY_MODIFIABLE_RESOURCES})
									
								endif()
								
							else()
							
								# brakuje zaleznosci - element tez do weryfikacji
								list(APPEND toVerify ${dep})
							
							endif()
						
						endif()
						
					endif()
					
					if(DEFINED ${_CPackComponentName}_REQUIRED)
						set("${_locCPackComponentName}_REQUIRED" ON)
					endif()
					
				endif()
				
			endforeach()
			
			list(REMOVE_DUPLICATES toVerify)
			list(LENGTH toVerify _continueCheck)
			
		endwhile()		
		
		list(APPEND INSTALLED_RAW_COMPONENTS ${prj})
		
	endforeach()
	
	#set(CMAKE_MODULE_PATH ${CMAKE_ORIGINAL_MODULE_PATH})	
	include(CPack)	
	
	set(CMAKE_MODULE_PATH ${_tmpModulePath})	
	
endfunction(_GENERATE_INSTALLER)