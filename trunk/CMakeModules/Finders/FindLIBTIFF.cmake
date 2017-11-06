# przygotowanie do szukania
FIND_INIT(LIBTIFF libtiff)

# szukanie
FIND_SHARED_EXT(LIBTIFF "libtiff<_i,?>" "libtiff<d,?><_i,?><d,?>" "libtiff" "libtiff<d,?>")

if(WIN32)
	FIND_PREREQUISITES(LIBTIFF "ZLIB")
elseif(UNIX)
	FIND_DEPENDENCIES(LIBTIFF "ZLIB")
endif()

# skopiowanie
FIND_FINISH(LIBTIFF)