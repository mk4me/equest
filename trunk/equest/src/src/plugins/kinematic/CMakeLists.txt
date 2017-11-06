BEGIN_PROJECT("dynamic")


SET_UI_FILES (
	TrajectoriesDialog.ui
)

SET_PRIVATE_HEADERS(
	KinematicParser.h
	AsfParser.h
	BvhParser.h
	KinematicVisualizer.h
	#OsgSchemeDrawer.h
	GRFSerie.h
	FbxParser.h
	MarkerSerie.h
	OsgSerie.h
	BvhSerie.h
	KinematicSerie.h
	SchemeDialog.h
	AbstractSkeletonSerie.h
	SkeletonSerie.h
	SkeletonStreamSerie.h
	SkeletonStateStreamSerie.h
	TrajectoriesDialog.h
	TrajectoriesDrawer.h
	PickHandler.h
	Manipulators.h	
	SkeletalVisualizationSchemeHelper.h	
	VisualizationScheme.h	
	PCH.h	
)

SET_PUBLIC_HEADERS (
	Wrappers.h
	IKinematicVisualizer.h
	Export.h
	JointsItemHelper.h
)

SET_SOURCE_FILES(
	SkeletalVisualizationSchemeHelper.cpp
	AbstractSkeletonSerie.cpp
	KinematicPlugin.cpp
	AsfParser.cpp
	BvhParser.cpp
	FbxParser.cpp
	KinematicParser.cpp
	KinematicSerie.cpp
	KinematicVisualizer.cpp	
	JointsItemHelper.cpp
	GRFSerie.cpp
	MarkerSerie.cpp
	OsgSerie.cpp
	BvhSerie.cpp
	SkeletonSerie.cpp
	SkeletonStreamSerie.cpp
	SkeletonStateStreamSerie.cpp
	TrajectoriesDialog.cpp
	TrajectoriesDrawer.cpp
	SchemeDialog.cpp
	PickHandler.cpp
	Manipulators.cpp
	VisualizationScheme.cpp
	PCH.cpp
)

SET_MOC_FILES(
	KinematicVisualizer.h
	SkeletonSerie.h
	SkeletonStreamSerie.h
	SkeletonStateStreamSerie.h
	MarkerSerie.h
	TrajectoriesDialog.h
	SchemeDialog.h
)


SET_RC_FILES (
	kinematicIcons.qrc
)

SET_PRECOMPILED_HEADER( PCH.h PCH.cpp)

GENERATE_PROJECT_TRANSLATIONS()

END_PROJECT()
