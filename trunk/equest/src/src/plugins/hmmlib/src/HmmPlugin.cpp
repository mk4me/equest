#include "HmmLibPCH.h"
#include <corelib/IPlugin.h>
#include <loglib/ILog.h>
#include "HmmService.h"

CORE_PLUGIN_BEGIN("HMM", core::UID::GenerateUniqueID("{3D3E1D76-7B4D-4F95-B413-67470D7404B6}"))
    CORE_PLUGIN_ADD_SERVICE(HmmService)
CORE_PLUGIN_END
