BEGIN_PROJECT("executable")

set(prepath "${CMAKE_BINARY_DIR}/src/mde_view/mde_view.dir/Debug/")
set (objFiles
	${prepath}AboutDialog.obj
	${prepath}AnalisisModel.obj
	${prepath}AnalisisWidget.obj
	${prepath}AnalysisTab.obj
	${prepath}AnalysisTreeContext.obj
	${prepath}ContextEventFilter.obj
	${prepath}MdeContexts.obj
	${prepath}MdeMainWindow.obj
	${prepath}MDEPerspectiveWidget.obj
	${prepath}MdeServiceWindow.obj
	${prepath}SummaryWindow.obj
	${prepath}VisualizerEventFilter.obj
	${prepath}moc_AboutDialog.obj
	${prepath}moc_AnalisisModel.obj
	${prepath}moc_AnalisisWidget.obj
	${prepath}moc_AnalysisTab.obj
	${prepath}moc_AnalysisTreeContext.obj
	${prepath}moc_ContextEventFilter.obj
	${prepath}moc_MdeContexts.obj
	${prepath}moc_MdeMainWindow.obj
	${prepath}moc_MDEPerspectiveWidget.obj
	${prepath}moc_MdeServiceWindow.obj
	${prepath}moc_SummaryWindow.obj
	${prepath}moc_VisualizerEventFilter.obj
	${prepath}qrc_MdeViewIcons.obj
)

SET_SOURCE_FILES(
	test.cpp
	AnalisisModelTest.cpp
	VisualizerContextTest.cpp
)

SET_PRIVATE_HEADERS(
	AnalisisModelTest.h
	VisualizerContextTest.h
)

SET_MOC_FILES (
	VisualizerContextTest.h
)

set (TARGET_OBJ ${objFiles})

add_definitions(-DQT_WIDGETS_LIB -DQT_GUI_LIB)

END_PROJECT()