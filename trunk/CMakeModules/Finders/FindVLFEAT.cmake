# przygotowanie do szukania
FIND_INIT(VLFEAT vlfeat)

# szukanie
IF (WIN32)
	FIND_SHARED(VLFEAT "vl" "vl")
ELSE () 
	FIND_SHARED(VLFEAT "libvl" "libvl")
ENDIF () 

# skopiowanie
FIND_FINISH(VLFEAT)

