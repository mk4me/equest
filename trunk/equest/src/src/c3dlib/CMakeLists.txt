BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	C3DParser.h
	C3DWriter.h
	C3DTypes.h
	C3DCollectionTypes.h
	Export.h
	IForcePlatform.h
	ForcePlatform.h
)


SET_SOURCE_FILES(
	C3DParser.cpp
	C3DWriter.cpp
	C3DTypes.cpp
	C3DCollectionTypes.cpp
	ForcePlatform.cpp
	)


END_PROJECT()