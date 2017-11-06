BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	DFPins.h
	Export.h
	DFProcessors.h
    DFSinks.h
    DFSources.h
)

SET_PRIVATE_HEADERS (
	DFElementsPCH.h
	
)

SET_SOURCE_FILES(
	DFElementsPlugin.cpp
	DFElementsPCH.cpp
	DFPins.cpp
	DFProcessors.cpp
    DFSinks.cpp
    DFSources.cpp
)

SET_MOC_FILES(
	DFSinks.h
)

SET_RC_FILES (
	dfElementsIcons.qrc
)

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( DFElementsPCH.h DFElementsPCH.cpp)

END_PROJECT()