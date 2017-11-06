#ifndef HEADER_GUARD_KINEMATICEDR__PCH_H__
#define HEADER_GUARD_KINEMATICEDR__PCH_H__

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>

#include <functional>
#include <type_traits>
#include <boost/lexical_cast.hpp>

#include <loglib/Exceptions.h>
#include <corelib/IPlugin.h>
#include <utils/SmartPtr.h>

#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/Enum.h>

#include <QtCore/QObject>
#include <QtCore/QtCore>

UTILS_PUSH_WARNINGS
//#include <osg/Array>
//#include <osg/Node>
//#include <osg/AnimationPath>
#include <osg/Config>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/ImageStream>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/Notify>
#include <osg/observer_ptr>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>
#include <osg/Quat>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/Timer>
//#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/MatrixTransform>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/ViewerEventHandlers>
//#include <osgGA/TrackballManipulator>
//#include <osgGA/FlightManipulator>
//#include <osgGA/DriveManipulator>
//#include <osgGA/KeySwitchMatrixManipulator>
//#include <osgGA/OrbitManipulator>
//#include <osgGA/StateSetManipulator>
//#include <osgGA/AnimationPathManipulator>
//#include <osgGA/TerrainManipulator>
#include <osgDB/ReadFile>
UTILS_POP_WARNINGS

#endif //HEADER_GUARD_SKELETON_VIEWER__PCH_H__

