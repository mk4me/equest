BEGIN_PROJECT("dynamic")

SET_PRIVATE_HEADERS(
	PythonService.h
	PythonConsole.h
	PythonHighlighter.h
	PythonLogic.h
	PythonPCH.h
	PythonProcessingBlock.h
)

SET_PUBLIC_HEADERS(
	PythonPluginUtils.h
	PythonEditor.h
	PythonDataChannel.h
	Export.h
)

SET_SOURCE_FILES (
	PythonService.cpp
	PythonConsole.cpp
	PythonPlugin.cpp
	PythonPluginUtils.cpp
	PythonHighlighter.cpp
	PythonLogic.cpp
	PythonEditor.cpp
	PythonPCH.cpp
	PythonDataChannel.cpp
	PythonProcessingBlock.cpp
)

SET_MOC_FILES (
	PythonHighlighter.h
	PythonService.h
	PythonEditor.h
	PythonConsole.h
)

SET_PRECOMPILED_HEADER( PythonPCH.h PythonPCH.cpp)

add_definitions(-DQT_NO_KEYWORDS)


END_PROJECT()