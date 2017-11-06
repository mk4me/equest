//#include "DicomImporterPCH.h"
#include <corelib/IPlugin.h>
#include <dicomlib/Dicom.h>
#include "DicomParser.h"
#include "DicomImporterSource.h"

//using namespace dicomImporter;

CORE_PLUGIN_BEGIN("dicomImporter", core::UID::GenerateUniqueID("{03F5BA03-E3E8-4C3A-A3F8-F122F3BEC2DC}"))
	CORE_PLUGIN_ADD_PARSER(dicomImporter::DicomParser);
	CORE_PLUGIN_ADD_SOURCE(dicomImporter::DicomImporterSource);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(DicomImage);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(DcmDataset);
CORE_PLUGIN_END
