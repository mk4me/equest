BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS (
	DFLMTypes.h
	Connection.h
	DFInterface.h
	DFModel.h
	DFNode.h
	DFPin.h
	DFSourceNode.h
	Model.h
	Node.h
	Pin.h
)

SET_SOURCE_FILES(
	DFModel.cpp
	DFNode.cpp
	DFPin.cpp
	DFSourceNode.cpp
	DFInterface.cpp
	Model.cpp
	Node.cpp
	Pin.cpp
	Connection.cpp
)

END_PROJECT()