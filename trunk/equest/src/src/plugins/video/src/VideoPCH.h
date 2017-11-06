/********************************************************************
	created:  2010/09/29
	created:  29:9:2010   14:13
	filename: VideoPCH.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOPCH_H__
#define __HEADER_GUARD__VIDEOPCH_H__
#ifndef DISABLE_PRECOMPILED_HEADERS

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <utils/Utils.h>
UTILS_PUSH_WARNINGS
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/TextureCubeMap>
#include <osg/TexMat>
#include <osg/CullFace>
#include <osg/ImageStream>
#include <osg/io_utils>
#include <osg/Timer>
#include <osg/GLExtensions>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/WindowManager>
#include <osgText/Text>
//#include <osgGA/TrackballManipulator>
//#include <osgGA/StateSetManipulator>
//#include <osgGA/EventVisitor>
//#include <osgDB/ReadFile>
UTILS_POP_WARNINGS

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
//#include <QtOpenGL/QtOpenGL>

#include <utils/Debug.h>
#include <utils/Enum.h>

#include <loglib/Exceptions.h>
#include <corelib/IPlugin.h>

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
#endif  // __HEADER_GUARD__VIDEOPCH_H__
