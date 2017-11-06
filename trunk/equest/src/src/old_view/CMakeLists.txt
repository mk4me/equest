BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
	ToolboxMain.h
)
SET_SOURCE_FILES(
	main.cpp
	ToolboxMain.cpp
)

SET_MOC_FILES(
    ToolboxMain.h
)

SET_UI_FILES(
	toolboxmaindeffile.ui
)

GENERATE_PROJECT_TRANSLATIONS()

END_PROJECT()