BEGIN_PROJECT("executable")

SET_SOURCE_FILES (
	main.cpp
	mainwindow.cpp
	DisableableRenderer.cpp
	QOsgWidgets.cpp
	QOsgWidgetsDeprecated.cpp
	skeletonViewerLogic.cpp
	skeletalVisualizationScheme.cpp
	PointSchemeDrawer.cpp
	LineSchemeDrawer.cpp
	PCH.cpp
)

SET_PRIVATE_HEADERS (
	mainwindow.h
	uniqueCollection.h
	DisableableRenderer.h
	QOsgWidgets.h
	QOsgWidgetsDeprecated.h
	skeletonViewerLogic.h
	skeletalVisualizationScheme.h
	ISchemeDrawer.h
	OsgSchemeDrawer.h
	PointSchemeDrawer.h
	LineSchemeDrawer.h
	PCH.h
)

SET_PRECOMPILED_HEADER(PCH.h PCH.cpp)

SET_MOC_FILES (
	mainwindow.h
	QOsgWidgets.h
)

SET_UI_FILES (
	mainwindow.ui
)

END_PROJECT()

