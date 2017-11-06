BEGIN_PYTHON_PROJECT()

SET_PRIVATE_HEADERS(
	MdeBridge.h
)

SET_SOURCE_FILES (
	PythonPlugin.cpp
	MdeBridge.cpp
)



#SET_PROJECT_ADDITIONAL_INSTALLS(
#	plugin_simplePython.pyd
#)

END_PYTHON_PROJECT()