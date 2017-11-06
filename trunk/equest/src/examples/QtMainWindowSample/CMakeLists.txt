BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS (
	SampleWindow.h
)

SET_SOURCE_FILES (
	QtMainWindowSample.cpp
	SampleWindow.cpp
)

SET_MOC_FILES (
    SampleWindow.h
)

SET_UI_FILES (
	SampleWindow.ui
)

END_PROJECT()
