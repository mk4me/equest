#include "EquestExporterPCH.h"
#include <corelib/IPlugin.h>
#include "EquestExporterService.h"

CORE_PLUGIN_BEGIN("EquestExporter", core::UID::GenerateUniqueID("{5EC298F9-878C-4325-A00A-0F58F092FA27}"))
	CORE_PLUGIN_ADD_SERVICE(equestExporter::EquestExporterService);
CORE_PLUGIN_END
