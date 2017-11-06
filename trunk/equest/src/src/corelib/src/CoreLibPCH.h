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
#include <mutex>
#include <future>
#include <condition_variable>
#include <thread>
#include <functional>

#include <boost/filesystem.hpp>
#include <boost/cast.hpp>

#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/Enum.h>
#include <utils/SmartPtr.h>
#include <utils/Export.h>

#include <loglib/Exceptions.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/VariantsCollection.h>
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

#endif  // DISABLE_PRECOMPILED_HEADERS
#endif  // __HEADER_GUARD__COREPCH_H__
