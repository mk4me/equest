BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	C3DChannels.h
	C3DCollections.h
	EventSerieBase.h
	Export.h
)

SET_PRIVATE_HEADERS (
	C3DParser.h
	C3DPCH.h
)

SET_SOURCE_FILES(
	C3DChannels.cpp
	EventSerieBase.cpp
	C3DPlugin.cpp
	C3DParser.cpp
	C3DPCH.cpp
	C3DCollections.cpp
)

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( C3DPCH.h C3DPCH.cpp)

END_PROJECT()