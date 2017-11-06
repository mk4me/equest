# przygotowanie do szukania
FIND_INIT(GLEW glew)

# szukanie
IF (WIN32)
	FIND_SHARED(GLEW "glew32<lib,?>" "glew32<lib,?>")
ELSE () 
	FIND_SHARED(GLEW "libGLEW<?>" "libGLEW<?>")
ENDIF () 

# skopiowanie
FIND_FINISH(GLEW)

