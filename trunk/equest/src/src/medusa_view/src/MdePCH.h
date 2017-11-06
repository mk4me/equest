/********************************************************************
	created:	2011/10/12
	created:	12:10:2011   12:35
	filename: 	hmmPCH.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMPCH_H__
#define HEADER_GUARD_HMM__HMMPCH_H__

#include <vector>
#include <list>
#include <stack>
#include <map>

#include <boost/date_time.hpp>
#include <functional>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QElapsedTimer>
#include <QtCore/QThread>

#include <QtWidgets/QWidget>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>

#include <utils/Utils.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <utils/SmartPtr.h>

#include <corelib/IVisualizer.h>
#include <loglib/Exceptions.h>
#include <plugins/c3d/C3DChannels.h>
#include <utils/Push.h>
//#include <timelinelib/IChannel.h>
//#include "IAppUsageContextManager.h"

#include <corelib/BaseDataTypes.h>
#include <corelib/IService.h>
#include <corelib/PluginCommon.h>
#include <corelib/IDataManagerReader.h>
#include <utils/Filesystem.h>

#include <corelib/IPlugin.h>

//#include <plugins/newTimeline/ITimelineService.h>
//#include <plugins/video/Wrappers.h>
//#include <plugins/kinematic/Wrappers.h>


#endif
