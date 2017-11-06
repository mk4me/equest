/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   17:40
	filename: CoreLibPCH.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__COREPCH_H__
#define __HEADER_GUARD__COREPCH_H__

#ifndef DISABLE_PRECOMPILED_HEADERS
// do momentu obsłużenia nagłówków prekompilowanych na innych platformach
// mechanizm ten musi być wyłączony, inaczej może negatywnie wpłynąć na
// czas kompilacji
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
#include <boost/filesystem.hpp>
#include <boost/cast.hpp>

#include <utils/Utils.h>
UTILS_PUSH_WARNINGS
#include <osg/Node>
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
#include <osg/Timer>
#include <osg/Vec3>
#include <osg/Vec3d>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osgDB/ReadFile>
UTILS_POP_WARNINGS

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QAction>

#include <utils/Debug.h>
#include <utils/Enum.h>
#include <utils/SmartPtr.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/VariantsCollection.h>
#include <utils/Export.h>
#include <utils/Filesystem.h>
#include <corelib/IApplication.h>
#include <corelib/PluginCommon.h>
#include <corelib/IPath.h>
#include <loglib/ILog.h>
#include <corelib/IService.h>
#include <corelib/ISource.h>
#include <corelib/IVisualizer.h>
#include <corelib/IParser.h>
#include <corelib/DataAccessors.h>
#include <corelib/ITransaction.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IFileDataManager.h>
#include <corelib/IFileManagerReader.h>
#include <corelib/IParserManagerReader.h>
#include <corelib/IVisualizerManager.h>
#include <corelib/Visualizer.h>
#include <loglib/Exceptions.h>

// przywrócenie warningów wyłączonych przez OSG
#if defined(_MSC_VER)
#   pragma warning( default : 4244 )
//#   pragma warning( default : 4251 )
#   pragma warning( default : 4275 )
#   pragma warning( default : 4512 )
#   pragma warning( default : 4267 )
#   pragma warning( default : 4702 )
#   pragma warning( default : 4511 )
#endif

#endif  // DISABLE_PRECOMPILED_HEADERS
#endif  // __HEADER_GUARD__COREPCH_H__
