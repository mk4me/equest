#include "CommunicationPCH.h"
#include <corelib/IPlugin.h>
#include <corelib/PluginCommon.h>

#include "HMDBService.h"
#include "HMDBSource.h"

#include <plugins/hmdbCommunication/IPatient.h>

CORE_PLUGIN_BEGIN("hmdbCommunication", core::UID::GenerateUniqueID("{26AC5928-AAE3-4AB3-BFDB-447D4171A4E5}"))
	CORE_PLUGIN_ADD_SERVICE(hmdbCommunication::HMDBService);
	CORE_PLUGIN_ADD_SOURCE(hmdbCommunication::HMDBSource);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(hmdbCommunication::IPatient);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(hmdbCommunication::AntropometricData);
CORE_PLUGIN_END
