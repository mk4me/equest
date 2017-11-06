#include "SubjectPCH.h"
#include <plugins/subject/ISubject.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>
#include "SubjectService.h"

CORE_PLUGIN_BEGIN("subject", core::UID::GenerateUniqueID("{F1B8F7D1-1611-4768-A01C-9F54084FFF51}"));
CORE_PLUGIN_ADD_SERVICE(SubjectService);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(PluginSubject::ISubject);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(PluginSubject::ISession);
CORE_PLUGIN_ADD_OBJECT_WRAPPER(PluginSubject::IMotion);
CORE_PLUGIN_END;
