# przygotowanie do szukania
FIND_INIT(GLUT glut)

# szukanie
IF (WIN32)
	FIND_SHARED(GLUT "glut32" "glut32")
ELSE () 
	FIND_SHARED(GLUT "libglut" "libglut")
ENDIF () 

# skopiowanie
FIND_FINISH(GLUT)

