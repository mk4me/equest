BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	ITimelineService.h
	VisualizerSerieTimelineChannel.h
	Export.h
)

SET_PRIVATE_HEADERS(
	TimeSliderWidget.h
	TimelineControlsWidget.h
	TimelineService.h
	TimelineWidget.h
	ChannelWidget.h
	ChannelsTreeItem.h
	ChannelCheckbox.h
	TimelinePCH.h
	TimelineControlActionsWidget.h
)

SET_SOURCE_FILES (
	TimelineWidget.cpp
	ChannelWidget.cpp
	ChannelsTreeItem.cpp
	ChannelCheckbox.cpp
	TimelineService.cpp
	TimelinePlugin.cpp
	TimeSliderWidget.cpp
	TimelineControlsWidget.cpp
	TimelinePCH.cpp
	VisualizerSerieTimelineChannel.cpp
	TimelineControlActionsWidget.cpp
)

SET_MOC_FILES (
	TimelineWidget.h
	ChannelWidget.h
	ChannelCheckbox.h
	TimeSliderWidget.h
	TimelineControlsWidget.h
	TimelineControlActionsWidget.h
)

SET_UI_FILES (
	TimelineWidget.ui
	TimelineControlsWidget.ui
	TimelineControlActionsWidget.ui
)

SET_RC_FILES (
	newTimelineIcons.qrc
)
SET_PRECOMPILED_HEADER( TimelinePCH.h TimelinePCH.cpp)

GENERATE_PROJECT_TRANSLATIONS()

END_PROJECT()