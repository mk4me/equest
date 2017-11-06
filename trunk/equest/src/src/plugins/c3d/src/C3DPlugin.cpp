#include "C3DPCH.h"
#include <corelib/IPlugin.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/c3d/C3DChannels.h>
#include "C3DParser.h"


CORE_PLUGIN_BEGIN("c3d", core::UID::GenerateUniqueID("{15AC5928-AAE3-4AB3-BFDB-447D4171A4E5}"))
	CORE_PLUGIN_ADD_PARSER(C3DParser);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::ScalarChannelReaderInterface);	
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::VectorChannelReaderInterface);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::VectorChannelCollection);    
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::C3DAnalogChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::EMGChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::GRFChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::MarkerChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::MarkerCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::C3DEventsCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::GRFCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::EMGCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::ForceChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::MomentChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::AngleChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::PowerChannel);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::ForceCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::AngleCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::PowerCollection);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::MomentCollection);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(c3dlib::MovieDelays);
CORE_PLUGIN_END
