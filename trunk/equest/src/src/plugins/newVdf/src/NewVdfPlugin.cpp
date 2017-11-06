#include "NewVdfPCH.h"
#include <corelib/IPlugin.h>
#include <plugins/newVdf/NewVdfService.h>
#include <loglib/ILog.h>

CORE_PLUGIN_BEGIN("newVdf", core::UID::GenerateUniqueID("{4A8E064B-BA20-40D0-91CD-8C6A87398BD4}"))
    CORE_PLUGIN_ADD_SERVICE(vdf::NewVdfService)
CORE_PLUGIN_END
