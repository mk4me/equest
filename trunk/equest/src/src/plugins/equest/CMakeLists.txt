BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	Export.h
	Equest.h
	Annotations.h
	AnnotationStatusFilter.h
	AnnotationStatusManager.h
	EquestPerspective.h
)

SET_PRIVATE_HEADERS (
	MessageDialog.h
	IEquestService.h
	AnnotationsDelegate.h
	EquestPCH.h
	EquestService.h
	DescriptionMaker.h
	AnnotationParser.h
    	Serializers.h
	#LayersXmlParser.h
    qstring_serialization.h
	EquestAnnotationVisualizer.h
	EquestAnnotation.h
)

SET_SOURCE_FILES(
	MessageDialog.cpp
	AnnotationsDelegate.cpp
	Annotations.cpp
	DescriptionMaker.cpp
	EquestPlugin.cpp
	EquestService.cpp
	AnnotationParser.cpp
	AnnotationStatusManager.cpp
	EquestPerspective.cpp
	#LayersXmlParser.cpp
	EquestPCH.cpp
	AnnotationStatusFilter.cpp
	EquestAnnotationVisualizer.cpp
	EquestAnnotation.cpp
)

SET_MOC_FILES(
	MessageDialog.h
	AnnotationsDelegate.h
	EquestAnnotationVisualizer.h
	)
SET_UI_FILES(
	toolbar.ui
	messageDialog.ui
	AnnotationArea.ui
)

SET_RC_FILES (
	equestRC.qrc
)
#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( EquestPCH.h EquestPCH.cpp)
#------------------------------------------------------------------------------
# DEFINICJE WYNIKAJ¥CE Z KONFLIKTU QT - BOOST (foreach)
# wy³aczamy s³owa kluczowe qt, zastêpujemy je makrami
# u¿ywamy tylko i wy³¹cznie BOOST_FOREACH!!
add_definitions(-DQT_NO_KEYWORDS -Dsignals=Q_SIGNALS -Dslots=Q_SLOTS -Demit=Q_EMIT)

END_PROJECT()