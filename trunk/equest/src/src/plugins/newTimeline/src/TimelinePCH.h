/********************************************************************
    created:  2011/07/18
    created:  18:7:2011   14:07
    filename: TimelinePCH.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMELINEPCH_H__
#define HEADER_GUARD_TIMELINE__TIMELINEPCH_H__

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
#include <QtWidgets/QWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QAction>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtGui/QPainter>

#define QT_NO_QT_INCLUDE_WARN
#include <qwt_scale_draw.h>
#include <qwt_slider.h>
#undef QT_NO_QT_INCLUDE_WARN

#endif  //  HEADER_GUARD_TIMELINE__TIMELINEPCH_H__
