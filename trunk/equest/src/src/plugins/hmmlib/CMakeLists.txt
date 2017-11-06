BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	#IllnessUnit.h
	EMGFilter.h
	ConfigurationDialog.h
	ConfigurationPainter.h
	ConfigurationPainterAreas.h
	Export.h
	TreeItemHelper.h
	MultiserieColorStrategy.h
)

SET_SOURCE_FILES(
	EMGFilter.cpp
	#IllnessUnit.cpp
	HmmLibPCH.cpp
	ConfigurationDialog.cpp
	ConfigurationPainter.cpp
	ConfigurationPainterAreas.cpp
	HmmPlugin.cpp
	HmmService.cpp
	SubjectFilterCommand.cpp
	IllnessUnitFilter.cpp
	Measurements.cpp
	ConfiguredFilterCommand.cpp
	MarkersFilter.cpp
	EMGConfigFilter.cpp
	KineticFilter.cpp
	TreeItemHelper.cpp
	TreeBuilder.cpp
)

SET_PRIVATE_HEADERS(
	HmmLibPCH.h
	HmmService.h
	SubjectFilterCommand.h
	IllnessUnitFilter.h
	Measurements.h
	ConfiguredFilterCommand.h
	MarkersFilter.h
	EMGConfigFilter.h
	KineticFilter.h
	TreeBuilder.h
)

SET_UI_FILES(
	configurationDialog.ui
	FilterConfiguratorWidget.ui
)

SET_MOC_FILES(
	ConfigurationDialog.h
	ConfigurationPainter.h
	ConfiguredFilterCommand.h
)
SET_RC_FILES (
	hmmIcons.qrc
)

SET_PRECOMPILED_HEADER (HmmLibPCH.h HmmLibPCH.cpp)

END_PROJECT()
