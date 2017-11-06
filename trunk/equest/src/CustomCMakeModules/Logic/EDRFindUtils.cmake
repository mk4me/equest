# Wyszukuje elementy edrutils dla edr
macro(FIND_INIT_EDRUTILS_MODULE variable dirName)
	if(NOT DEFINED EDRUTILS_INCLUDE_ROOT OR NOT DEFINED EDRUTILS_BUILD_ROOT)
		message(WARNING "Nie ustawiono sciezek dla EDRUTILS, nie mozna dodac ${variable}")
	else()
		FIND_INIT_CUSTOM_MODULE(${variable} ${dirName} ${EDRUTILS_INCLUDE_ROOT} ${EDRUTILS_BUILD_ROOT})
	endif()
endmacro(FIND_INIT_EDRUTILS_MODULE)