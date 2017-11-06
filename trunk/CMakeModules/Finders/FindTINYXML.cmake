# przygotowanie do szukania
FIND_INIT(TINYXML tinyxml)

# szukanie
if (WIN32)
	#FIND_STATIC(TINYXML "tinyxmlSTL")
	FIND_SHARED(TINYXML "tinyxml" "tinyxml")
elseif (UNIX)
	FIND_SHARED(TINYXML "libtinyxml" "libtinyxml")
endif()
# koniec
FIND_FINISH(TINYXML)

set(TINYXML_COMPILER_DEFINITIONS TIXML_USE_STL)
