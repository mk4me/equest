BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	INewChartVisualizer.h
	INewChartSerie.h
	Wrappers.h
	Export.h
	NewChartItemHelper.h
	INewChartCurveGenerator.h
)

SET_PRIVATE_HEADERS (
	NewChartVisualizer.h
	INewChartSeriePrivate.h
	NewChartSerie.h
	NewChartStreamSerie.h
	NewChartPicker.h
	NewChartMarker.h
	NewChartState.h
	NewChartValueMarker.h
	NewChartVerticals.h
	NewChartLabelState.h
	NewChartEventStateData.h
	NewChartCurve.h
	NewChartEvents.h
	NewChartLegend.h
	NewChartLegendItem.h
	NewChartSeriesData.h
	NewChartScaleDrawer.h
	NewChartHelpers.h
	StatsTable.h
	Scales.h
	NewChartDot.h
	NewChartDotFloating.h
	NewChartDotFixed.h
	NewChartLabel.h
	MovingAverageGenerator.h
	SliceGenerator.h
	NewChartPCH.h
)

SET_SOURCE_FILES (
	NewChartPlugin.cpp	
	NewChartVisualizer.cpp
	NewChartSerie.cpp
	NewChartStreamSerie.cpp
	NewChartPicker.cpp
	NewChartMarker.cpp
	NewChartState.cpp
	NewChartValueMarker.cpp
	NewChartVerticals.cpp
	NewChartLabelState.cpp
	NewChartCurve.cpp
	NewChartEvents.cpp
	NewChartItemHelper.cpp
	NewChartLegend.cpp
	NewChartScaleDrawer.cpp
	NewChartSeriesData.cpp
	NewChartEventStateData.cpp
	NewChartLegendItem.cpp
	NewChartHelpers.cpp
	StatsTable.cpp
	Scales.cpp
	NewChartDot.cpp
	NewChartDotFloating.cpp
	NewChartLabel.cpp
	MovingAverageGenerator.cpp
	SliceGenerator.cpp
	NewChartPCH.cpp
)

SET_MOC_FILES (
	NewChartVisualizer.h
	NewChartState.h
	NewChartPicker.h
	NewChartValueMarker.h
	NewChartVerticals.h
	NewChartLabelState.h
	NewChartLegend.h
	NewChartLegendItem.h
	NewChartHelpers.h
	StatsTable.h
)

SET_UI_FILES (
	statsTable.ui
)

SET_RC_FILES (
	newChartIcons.qrc
)

SET_PRECOMPILED_HEADER( NewChartPCH.h NewChartPCH.cpp)

GENERATE_PROJECT_TRANSLATIONS()

END_PROJECT()
