# przygotowanie do szukania
FIND_INIT(LOG4CXX log4cxx)

# szukanie
FIND_SHARED(LOG4CXX "log4cxx" "log4cxx")

if(UNIX)
	FIND_DEPENDENCIES(LOG4CXX "APR;APR_UTIL")
endif()

# skopiowanie
FIND_FINISH(LOG4CXX)
