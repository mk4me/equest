#ifndef HEADER_GUARD_VIDLIB__PCH_H__
#define HEADER_GUARD_VIDLIB__PCH_H__
#ifndef DISABLE_PRECOMPILED_HEADERS

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>

#include <utils/Utils.h>
#include <utils/Debug.h>

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
#include <osgViewer/Renderer>
#include <osgWidget/WindowManager>
#include <osgText/Text>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/EventVisitor>
#include <osgDB/ReadFile>
UTILS_POP_WARNINGS

#endif // DISABLE_PRECOMPILED_HEADERS
#endif // HEADER_GUARD_VIDLIB__PCH_H__
