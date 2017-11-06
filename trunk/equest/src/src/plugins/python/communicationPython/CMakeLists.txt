BEGIN_PYTHON_PROJECT()

SET_PRIVATE_HEADERS(
	CommunicationPython.h
)

SET_SOURCE_FILES (
	PythonPlugin.cpp
	CommunicationPython.cpp
)



#SET_PROJECT_ADDITIONAL_INSTALLS(
#	plugin_simplePython.pyd
#)

END_PYTHON_PROJECT()