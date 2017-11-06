# przygotowanie do szukania
FIND_INIT(OPENTHREADS openthreads)

# funkcja wykrywaj¹ce wersjê OT
function(OPENTHREADS_FIND_LIB_VERSION path)    
	# inicjalizacja
	set(OPENTHREADS_VERSION "Unknown" CACHE STRING "Unknown version")
	# próba odczytania wersji z pliku
	if (EXISTS ${path})
		file(READ "${path}" _ot_Version_contents)		
		string(REGEX REPLACE ".*#define.*OPENTHREADS_SOVERSION.*([0-9][0-9]).*"
            "\\1" _ot_version ${_ot_Version_contents})
			
		if (OPENTHREADS_VERSION STREQUAL "Unknown")
			set(OPENTHREADS_VERSION "${_ot_version}" CACHE STRING "The version of OPENTHREADS_VERSION${suffix} which was detected" FORCE)
		endif()
    endif()
	# czy siê uda³o?
	if (OPENTHREADS_VERSION STREQUAL "Unknown")
		message("Unknown version of OPENTHREADS_VERSION. File ${path} could not be read. This may result in further errors.")
	endif()
endfunction(OPENTHREADS_FIND_LIB_VERSION)

# wykrycie wersji OT
OPENTHREADS_FIND_LIB_VERSION("${OPENTHREADS_INCLUDE_DIR}/OpenThreads/Version")

FIND_SHARED(OPENTHREADS "OpenThreads" "<ot${OPENTHREADS_VERSION}-,?>OpenThreads")
FIND_FINISH(OPENTHREADS)
