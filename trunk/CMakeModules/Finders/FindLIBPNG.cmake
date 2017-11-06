# przygotowanie do szukania
FIND_INIT(LIBPNG libpng)

# szukanie
FIND_SHARED(LIBPNG "libpng16<lib,?>" "libpng16<lib,?>")

if(WIN32)
	FIND_PREREQUISITES(LIBPNG "ZLIB")
elseif(UNIX)
	FIND_DEPENDENCIES(LIBPNG "ZLIB")
endif()

# skopiowanie
FIND_FINISH(LIBPNG)