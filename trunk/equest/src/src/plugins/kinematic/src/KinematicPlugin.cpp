#include "PCH.h"
#include "KinematicVisualizer.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "KinematicParser.h"
#include "AsfParser.h"
#include "FbxParser.h"
#include <plugins/kinematic/Wrappers.h>
#include "BvhParser.h"

CORE_PLUGIN_BEGIN("kinematic", core::UID::GenerateUniqueID("{3C0CD7AF-9351-46CC-A5FE-52AA182E1279}"));
    CORE_PLUGIN_ADD_PARSER(KinematicParser);
	CORE_PLUGIN_ADD_PARSER(AsfParser);
	//CORE_PLUGIN_ADD_PARSER(BvhParser);
	CORE_PLUGIN_ADD_PARSER(FbxParser);
	CORE_PLUGIN_ADD_VISUALIZER(KinematicVisualizer);
	//CORE_PLUGIN_ADD_OBJECT_WRAPPER(BVHData);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(kinematic::Skeleton);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(SkeletonStates);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(SkeletonWithStates);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(SkeletonWithStreamData);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(acclaim::Skeleton);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(acclaim::MotionData);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(SkeletonDataStream);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(osg::PositionAttitudeTransform);
CORE_PLUGIN_END;

//using namespace kinematic;
//
//
////! Metoda mapuje logi z biblioteki kinematic na logi zgodne z systemem EDR
////! \param severity typ informacji
////! \param msg przekazywana informacja
//void logCallback(Logger::LogSeverity severity, const std::string& msg)
//{
//    switch ( severity ) {
//        case Logger::Error:
//            PLUGIN_LOG_NAMED_ERROR("kinematic",msg);
//            break;
//        case Logger::Warning:    
//			PLUGIN_LOG_NAMED_WARNING("kinematic",msg);
//            break;
//        case Logger::Info:
//			PLUGIN_LOG_NAMED_INFO("kinematic",msg);
//            break;
//        case Logger::Debug:
//			PLUGIN_LOG_NAMED_DEBUG("kinematic",msg);
//            break;
//        default:
//            UTILS_ASSERT(false, "Nieznany poziom logowania. Wiadomość: %s", msg);
//            break;
//    }
//}
//
//
///*! Struktura widziana tylko w obrębie tego pliku. 
// * Podpina logowanie kinematic do metody mapującej komunikaty na te z EDR-a.
// * Struktura tworzy się automatycznie.
// */
//static struct Initializer
//{
//    Initializer()
//    {
//        Logger::getInstance().setLogCallback(logCallback);
//    }
//} initializer;
//
