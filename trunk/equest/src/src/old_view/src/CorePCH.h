/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   17:40
	filename: CorePCH.h
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
#include <list>
#include <typeinfo>

#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/Enum.h>

UTILS_PUSH_WARNINGS
#include <osg/Node>
#include <osg/Config>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Image>
#include <osg/ImageStream>
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
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
UTILS_POP_WARNINGS

#include <QtCore/QtCore>
#include <QtGui/QtGui>

#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>
#include <boost/any.hpp>
#include <boost/range.hpp>
#include <boost/iterator.hpp>

#include <utils/SmartPtr.h>
#include <loglib/ILog.h>
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
