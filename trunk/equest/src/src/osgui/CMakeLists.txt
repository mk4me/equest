BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	OsgSceneDump.h
	AspectRatioKeeper.h
	Borderized.h
	Buttonized.h
	ContextMenu.h
	EmbeddedWindow.h
	Grid.h
	KeyboardMapper.h
	KeyboardMapperHandler.h
	LabeledView.h
	OsgWidgetUtils.h
	DisableableRenderer.h
	QOsgWidgets.h
	QOsgWidgetsDeprecated.h
	StyleEx.h
	Toolbar.h
	Tooltip.h
	TooltipHost.h
	Utils2D.h	
	EventCallback.h	
	macroHacks.h	
)

SET_PRIVATE_HEADERS( PCH.h )

SET_SOURCE_FILES(
	OsgSceneDump.cpp
	AspectRatioKeeper.cpp
	ContextMenu.cpp
	DisableableRenderer.cpp
	EmbeddedWindow.cpp
	Grid.cpp
	KeyboardMapper.cpp
	KeyboardMapperHandler.cpp
	OsgWidgetUtils.cpp
	QOsgWidgets.cpp
	QOsgWidgetsDeprecated.cpp
	StyleEx.cpp
	Toolbar.cpp
	Tooltip.cpp
	TooltipHost.cpp
	Utils2D.cpp
	PCH.cpp
)
SET_PRECOMPILED_HEADER(PCH.h PCH.cpp)

END_PROJECT()