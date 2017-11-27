BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	IEquestExporterService.h
)

SET_PRIVATE_HEADERS (
	AnnotationData.h
	CSVExporter.h
	ExporterModel.h
	IExporter.h
	INIExporter.h
	EquestExporterPCH.h
	EquestExporterService.h
	EquestExporterServiceWidget.h
	PythonExporter.h
	UserAnnotationUpdater.h
)

SET_SOURCE_FILES(
	AnnotationData.cpp
	CSVExporter.cpp
	ExporterModel.cpp
	INIExporter.cpp
	EquestExporterPCH.cpp
	EquestExporterPlugin.cpp
	EquestExporterService.cpp
	EquestExporterServiceWidget.cpp
	PythonExporter.cpp
	UserAnnotationUpdater.cpp
)

SET_MOC_FILES(
	EquestExporterServiceWidget.h
)
SET_UI_FILES(
	EquestExporter.ui
)

SET_RC_FILES (
	EquestExporterRC.qrc
)
#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( EquestExporterPCH.h EquestExporterPCH.cpp)

#------------------------------------------------------------------------------
# DEFINICJE WYNIKAJ¥CE Z KONFLIKTU QT - BOOST (foreach)
# wy³aczamy s³owa kluczowe qt, zastêpujemy je makrami
# u¿ywamy tylko i wy³¹cznie BOOST_FOREACH!!
add_definitions(-DQT_NO_KEYWORDS -Dsignals=Q_SIGNALS -Dslots=Q_SLOTS -Demit=Q_EMIT)

END_PROJECT()