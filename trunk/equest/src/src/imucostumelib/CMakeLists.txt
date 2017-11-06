BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	ImuCostume.h
	ImuSensor.h
	CostumeRawIO.h
	CostumeCANopenIO.h
	CANopenSensor.h
	Export.h	
	CANopenFrame.h
	ProtocolSendBufferHelper.h
	ProtocolReadBufferHelper.h
)

SET_SOURCE_FILES( 
	ImuCostume.cpp
	ImuSensor.cpp
	CostumeRawIO.cpp
	CostumeCANopenIO.cpp
	CANopenSensor.cpp
	ProtocolSendBufferHelper.cpp
	ProtocolReadBufferHelper.cpp
)

END_PROJECT()