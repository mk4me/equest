BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
	AnalysisTab.h
	AboutDialog.h
	MdeMainWindow.h
	AnalisisWidget.h
	AnalisisModel.h
	ContextEventFilter.h
	MdeContexts.h
	SummaryWindow.h
	AnalysisTreeContext.h
	VisualizerEventFilter.h
    MdeServiceWindow.h
	ImageTableModel.h
	ImageTableWidget.h
	LoginDialog.h
	MdePCH.h
)

SET_SOURCE_FILES (
	AnalysisTab.cpp
	AboutDialog.cpp
	main.cpp
	MdeMainWindow.cpp
	MdeContexts.cpp
	AnalisisModel.cpp
	AnalisisWidget.cpp
	AnalysisTreeContext.cpp
	SummaryWindow.cpp
	ContextEventFilter.cpp
	VisualizerEventFilter.cpp
    MdeServiceWindow.cpp
	ImageTableModel.cpp
	ImageTableWidget.cpp
	LoginDialog.cpp
	MdePCH.cpp
)

SET_MOC_FILES(
	AboutDialog.h
	AnalysisTab.h
	ContextEventFilter.h
	VisualizerEventFilter.h
	MdeMainWindow.h
	MdeContexts.h
	AnalisisModel.h
	ReportsTab.h
	ReportsThumbnailContext.h
	AnalisisWidget.h
	AnalysisTreeContext.h
	SummaryWindow.h
    MdeServiceWindow.h
	ImageTableWidget.h
	ImageTableModel.h
	LoginDialog.h
)

SET_UI_FILES(
	AboutDialog.ui
	toolboxmaindeffile.ui
	AnalisisWidget.ui
	LoginDialog.ui
)

SET_RC_FILES (
	MdeViewIcons.qrc
)

SET_PRECOMPILED_HEADER(MdePCH.h MdePCH.cpp)

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