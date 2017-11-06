BEGIN_PROJECT("dynamic")

SET_PRIVATE_HEADERS(
	StreamOsgWidget.h
	VideoParser.h
	VideoVisualizer.h
	VideoPCH.h
)

SET_PUBLIC_HEADERS(
	Wrappers.h
)

SET_SOURCE_FILES (
	StreamOsgWidget.cpp
	VideoPlugin.cpp
	VideoParser.cpp
	VideoVisualizer.cpp
	VideoPCH.cpp
)

SET_RC_FILES(
	videoIcons.qrc
)

SET_PRECOMPILED_HEADER( VideoPCH.h VideoPCH.cpp )

GENERATE_PROJECT_TRANSLATIONS()

END_PROJECT()