BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	Export.h
	IFtpSession.h
	IWSDLService.h
	CURLFTPHelper.h
	Types.h
	CURLManager.h
	IFtpOperations.h
	CURLUtils.h
	EncryptionUtils.h
)

SET_SOURCE_FILES(
	CURLFTPHelper.cpp
	CURLManager.cpp
	CURLUtils.cpp
	EncryptionUtils.cpp
)

END_PROJECT()