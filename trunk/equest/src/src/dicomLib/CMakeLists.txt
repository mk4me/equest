BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	Export.h
	Dicom.h
	IDicomInternalStruct.h
	DicomImporter.h
	DicomInternalStruct.h
	DicomSplitterModel.h
)

#------------------------------------------------------------------------------
# Ÿród³a
SET_SOURCE_FILES(
	DicomInternalStruct.cpp
	DicomImporter.cpp
	#DicomImporterPCH.cpp
	#DicomParser.cpp
	DicomSplitterModel.cpp
)

SET_PRECOMPILED_HEADER( 
	DicomImporterPCH.h 
	DicomImporterPCH.cpp
)

SET_MOC_FILES(
	DicomSplitterModel.h
)

END_PROJECT()