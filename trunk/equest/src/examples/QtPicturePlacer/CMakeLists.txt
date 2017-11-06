BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS (
	PicturePlacerWindow.h
)

SET_SOURCE_FILES (
	main.cpp
	PicturePlacerWindow.cpp
)

SET_MOC_FILES (
    PicturePlacerWindow.h
)

SET_UI_FILES (
	PicturePlacerWindow.ui
)

END_PROJECT()
