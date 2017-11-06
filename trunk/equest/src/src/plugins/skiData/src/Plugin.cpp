#include <corelib/IPlugin.h>
#include "DataSource.h"
#include "SkiData.h"
#include "SkiMetadata.h"
#include "SkiDataParser.h"
#include "SkiMetadataParser.h"

CORE_PLUGIN_BEGIN("skiData", core::UID::GenerateUniqueID("{3C0CD7AF-BACA-46CC-A5FE-52AA182E1279}"));
CORE_PLUGIN_ADD_SOURCE(skidata::SkiDataSource);
CORE_PLUGIN_ADD_PARSER(skidata::SkiDataParser);
CORE_PLUGIN_ADD_PARSER(skidata::SkiMetadataParser);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(skidata::SkiData);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(skidata::SkiMetadata);
CORE_PLUGIN_END;