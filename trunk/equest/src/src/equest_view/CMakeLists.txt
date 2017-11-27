BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
	AnalysisTab.h
	EquestPerspectiveWidget.h
	AboutDialog.h
	EquestMainWindow.h
	AnalisisWidget.h
	AnalisisModel.h
	ContextEventFilter.h
	EquestContexts.h
	SummaryWindow.h
	AnalysisTreeContext.h
	VisualizerEventFilter.h
    EquestServiceWindow.h
	EquestPCH.h
)

SET_SOURCE_FILES (
	EquestPerspectiveWidget.cpp
	AnalysisTab.cpp
	AboutDialog.cpp
	main.cpp
	EquestMainWindow.cpp
	EquestContexts.cpp
	AnalisisModel.cpp
	AnalisisWidget.cpp
	AnalysisTreeContext.cpp
	SummaryWindow.cpp
	ContextEventFilter.cpp
	VisualizerEventFilter.cpp
    EquestServiceWindow.cpp
	EquestPCH.cpp
)

SET_MOC_FILES(
	AboutDialog.h
	AnalysisTab.h
	ContextEventFilter.h
	VisualizerEventFilter.h
	EquestMainWindow.h
	EquestContexts.h
	AnalisisModel.h
	ReportsTab.h
	ReportsThumbnailContext.h
	AnalisisWidget.h
	AnalysisTreeContext.h
	SummaryWindow.h
    EquestServiceWindow.h
	EquestPerspectiveWidget.h
)

SET_UI_FILES(
	AboutDialog.ui
	toolboxmaindeffile.ui
	AnalisisWidget.ui
)

SET_RC_FILES (
	EquestViewIcons.qrc
)

SET_PRECOMPILED_HEADER(EquestPCH.h EquestPCH.cpp)

GENERATE_PROJECT_TRANSLATIONS()

#------------------------------------------------------------------------------
# DEFINICJE WYNIKAJ¥CE Z KONFLIKTU QT - BOOST (foreach)
# wy³aczamy s³owa kluczowe qt, zastêpujemy je makrami
# u¿ywamy tylko i wy³¹cznie BOOST_FOREACH!!
add_definitions(-DQT_NO_KEYWORDS -Dsignals=Q_SIGNALS -Dslots=Q_SLOTS -Demit=Q_EMIT)

if(CONFIG_DEMO_MODE)
	add_definitions(-DDEMO_MODE)
endif()

if (CONFIG_DEMO_MODE_ONLINE)
	add_definitions(-DDEMO_MODE_ONLINE)
endif()

END_PROJECT()