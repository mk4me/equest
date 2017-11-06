#include "MedusaExporterPCH.h"
#include <corelib/IPlugin.h>
#include "MedusaExporterService.h"

CORE_PLUGIN_BEGIN("MedusaExporter", core::UID::GenerateUniqueID("{5EC298F9-878C-4325-A00A-0F58F092FA27}"))
	CORE_PLUGIN_ADD_SERVICE(medusaExporter::MedusaExporterService);
CORE_PLUGIN_END
