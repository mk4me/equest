BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS (
	ChannelsTree.h
	ChannelsTreeItem.h
	SampleWindow.h
	TimeChannelWidget.h
	TestClientChannel.h
	TimelineWidget.h
	TimeCounter.h
	ChannelsTreeHeaderWidget.h
)

SET_SOURCE_FILES (
	ChannelsTree.cpp
	ChannelsTreeItem.cpp
	QtTimelineSample.cpp
	SampleWindow.cpp
	TimeChannelWidget.cpp
	TestClientChannel.cpp
	TimelineWidget.cpp
	TimeCounter.cpp
	ChannelsTreeHeaderWidget.cpp
)

SET_MOC_FILES (
    SampleWindow.h
	ChannelsTree.h
	TimeChannelWidget.h
	TimelineWidget.h
	ChannelsTreeHeaderWidget.h
)

SET_UI_FILES (
	SampleWindow.ui
	TimelineWidget.ui
)

END_PROJECT()
