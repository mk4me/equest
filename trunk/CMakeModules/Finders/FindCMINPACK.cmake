# przygotowanie do szukania
FIND_INIT(CMINPACK cminpack)
FIND_STATIC_EXT(CMINPACK "cminpack" "cminpack")

if (WIN32) 	
	FIND_STATIC_EXT(CMINPACK "cminpack" "cminpack")
elseif (UNIX)
	FIND_SHARED(CMINPACK "cminpack" "cminpack")
endif()

FIND_FINISH(CMINPACK)
