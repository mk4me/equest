BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	AmcParser.h
	AsfParser.h	
	DegreeOfFreedom.h
	Bone.h
	MotionData.h
	Skeleton.h
)

SET_SOURCE_FILES( 
	AmcParser.cpp
	AsfParser.cpp
	DegreeOfFreedom.cpp
	Skeleton.cpp
	Bone.cpp
)

END_PROJECT()