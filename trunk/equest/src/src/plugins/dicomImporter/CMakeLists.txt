BEGIN_PROJECT("dynamic")

#SET_PUBLIC_HEADERS (
#	Export.h
	#Dicom.h
	#IDicomInternalStruct.h
#)

SET_PRIVATE_HEADERS (
	DicomParser.h
	#DicomImporterPCH.h
	DicomImporterSource.h
	DicomImporterSourceWidget.h
	#DicomImporter.h
	#DicomInternalStruct.h
	#DicomVisualizer.h
	#DicomSplitterModel.h
)

SET_SOURCE_FILES(
	DicomImporterPlugin.cpp
	DicomImporterSource.cpp
	DicomImporterSourceWidget.cpp
	#DicomInternalStruct.cpp
	#DicomImporter.cpp
	DicomVisualizer.cpp
	#DicomImporterPCH.cpp
	DicomParser.cpp
	#DicomSplitterModel.cpp
)

SET_MOC_FILES(
	DicomImporterSourceWidget.h
	DicomVisualizer.h
	#DicomSplitterModel.h
)
SET_UI_FILES(
	dicomSource.ui
)

SET_RC_FILES (
	dicomRC.qrc
)
#------------------------------------------------------------------------------
# prekompilowany nag³ówek
#SET_PRECOMPILED_HEADER( DicomImporterPCH.h DicomImporterPCH.cpp)

END_PROJECT()