#include <corelib/IPlugin.h>
#include "FileSource.h"


CORE_PLUGIN_BEGIN("File Source", core::UID::GenerateUniqueID("{979686E5-813B-4ED3-9A09-371FB8C1CA26}"))
	CORE_PLUGIN_ADD_SOURCE(FileSource);
CORE_PLUGIN_END
