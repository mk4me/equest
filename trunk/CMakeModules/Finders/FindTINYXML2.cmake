# przygotowanie do szukania
FIND_INIT(TINYXML2 tinyxml2)

# szukanie
if (WIN32)	
	FIND_SHARED(TINYXML2 "tinyxml2" "tinyxml2")
elseif (UNIX)
	FIND_SHARED(TINYXML2 "libtinyxml2" "libtinyxml2")
endif()
# koniec
FIND_FINISH(TINYXML2)