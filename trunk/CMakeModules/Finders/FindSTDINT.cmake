# stdint
if(WIN32)
	set(STDINT_INCLUDE_DIR "${FIND_LIBRARIES_INCLUDE_ROOT}/stdint/inttypes" CACHE PATH "Location(s) of stdint headers.")
	# sami dostarczamy
	if(MSVC AND NOT MSVC10)
		set(STDINT_ADDITIONAL_INCLUDE_DIRS "${FIND_LIBRARIES_INCLUDE_ROOT}/stdint/stdint")
	endif()
endif(WIN32)

set(LIBRARY_STDINT_FOUND 1)