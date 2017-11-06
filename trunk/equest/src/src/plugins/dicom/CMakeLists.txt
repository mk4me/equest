BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	Export.h
	Dicom.h
	ILayer.h
	ILayeredImage.h
	IDicomInternalStruct.h
	Annotations.h
	ValueLayer.h
	AnnotationStatusFilter.h
	AnnotationStatusManager.h
	DicomPerspective.h
)

SET_PRIVATE_HEADERS (
	MessageDialog.h
	IDicomService.h
	AnnotationsDelegate.h
	LayeredStateMachine.h
	#DicomParser.h
	DicomPCH.h
	#DicomVisualizer.h
	#DicomImporter.h
	DicomService.h
	DicomLoader.h
	DescriptionMaker.h
	DicomInternalStruct.h
	PointsState.h
	EditState.h
	AbstractEditState.h
	NormalState.h
	MoveState.h
	LayeredSerie.h
	BackgroundLayer.h
	PointsLayer.h
	LayeredImage.h
	LayeredImageVisualizer.h
	LayeredImageVisualizerView.h
	LayeredModelView.h
	PngParser.h
    Serializers.h
	spline.h
	LayersXmlParser.h
	InternalXmlParser.h
    qstring_serialization.h
)

SET_SOURCE_FILES(
	MessageDialog.cpp
	AnnotationsDelegate.cpp
	Annotations.cpp
	LayeredStateMachine.cpp
	PointsState.cpp
	NormalState.cpp
	MoveState.cpp
	EditState.cpp
	AbstractEditState.cpp
	DescriptionMaker.cpp
	LayeredSerie.cpp
	DicomPlugin.cpp
	DicomService.cpp
	DicomLoader.cpp
	#DicomSource.cpp
	#DicomSourceWidget.cpp
	DicomInternalStruct.cpp
	#DicomImporter.cpp
	BackgroundLayer.cpp
	PointsLayer.cpp
	LayeredImage.cpp
	LayeredImageVisualizer.cpp
	LayeredImageVisualizerView.cpp
	PngParser.cpp
	LayeredModelView.cpp
	spline.cpp
	AnnotationStatusManager.cpp
	DicomPerspective.cpp
	LayersXmlParser.cpp
	InternalXmlParser.cpp
	DicomPCH.cpp
	AnnotationStatusFilter.cpp
)

SET_MOC_FILES(
	MessageDialog.h
	AnnotationsDelegate.h
	LayeredStateMachine.h
	LayeredImageVisualizer.h
	#DicomVisualizer.h
	#DicomSourceWidget.h
	LayeredImageVisualizerView.h
	LayeredModelView.h
	PointsState.h
	EditState.h
	AbstractEditState.h
	NormalState.h
)
SET_UI_FILES(
	layeredImageVisualizer.ui
	#dicomSource.ui
	messageDialog.ui
)

SET_RC_FILES (
	dicomRC.qrc
)
#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( DicomPCH.h DicomPCH.cpp)
#------------------------------------------------------------------------------
# DEFINICJE WYNIKAJ¥CE Z KONFLIKTU QT - BOOST (foreach)
# wy³aczamy s³owa kluczowe qt, zastêpujemy je makrami
# u¿ywamy tylko i wy³¹cznie BOOST_FOREACH!!
add_definitions(-DQT_NO_KEYWORDS -Dsignals=Q_SIGNALS -Dslots=Q_SLOTS -Demit=Q_EMIT)

END_PROJECT()