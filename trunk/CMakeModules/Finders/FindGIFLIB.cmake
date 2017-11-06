# przygotowanie do szukania
FIND_INIT(GIFLIB giflib)

# szukanie
if(WIN32)
	#FIND_SHARED(GIFLIB "giflib<4,?>" "giflib<4,?>")
	FIND_STATIC(GIFLIB "libgif<5,?>")
elseif(UNIX)
	FIND_SHARED(GIFLIB "libgif" "libgif")
endif()

# skopiowanie
FIND_FINISH(GIFLIB)

