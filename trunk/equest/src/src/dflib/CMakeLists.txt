BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS (
	Connection.h
	IDFNode.h
	IDFPin.h
	IConnection.h
	INode.h
	IPin.h
	IModel.h
	LoggerHelpers.h
	IDFFeatures.h
	ModelBase.h
	DFModelRunner.h
	STDFModelRunner.h
	IDFLogger.h
	ModelImpl.h
	Model.h
	Node.h
	Pin.h
	MRModelInterfaceVerifier.h
	MRModelPinRequirementsVerifier.h
	MRModelLogicVerifier.h
)

SET_PRIVATE_HEADERS (
	IMRNode.h
	MRNode.h
	MRNodeImpl.h
	MRPin.h
	MRConnection.h
	DFModelRunnerImpl.h
	ISTMRNode.h
	STMRNode.h
	STMRNodeImpl.h
	STMRPin.h
	STMRConnection.h
	STDFModelRunnerImpl.h
)

SET_SOURCE_FILES(
	DFModelRunner.cpp
	MRNode.cpp
	MRNodeImpl.cpp
	MRPin.cpp
	Model.cpp
	Node.cpp
	Pin.cpp
	Connection.cpp
	LoggerHelpers.cpp
	ModelBase.cpp
	ModelImpl.cpp
	MRConnection.cpp
	DFModelRunnerImpl.cpp
	MRModelInterfaceVerifier.cpp
	MRModelPinRequirementsVerifier.cpp
	MRModelLogicVerifier.cpp
	STDFModelRunner.cpp
	STDFModelRunnerImpl.cpp
	STMRNode.cpp
	STMRNodeImpl.cpp
	STMRPin.cpp
	STMRConnection.cpp
)

END_PROJECT()
