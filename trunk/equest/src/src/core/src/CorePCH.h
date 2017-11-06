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

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>
#include <thread>

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
#include <functional>

#include <utils/Utils.h>
#include <utils/Debug.h>
#include <utils/Enum.h>

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/any.hpp>
#include <boost/range.hpp>
#include <boost/iterator.hpp>

#include <loglib/Exceptions.h>
#include <corelib/BaseDataTypes.h>
#include <utils/SmartPtr.h>
#include <utils/PtrPolicyStd.h>
#include "Application.h"
#include "ServiceManager.h"
#include "DataManager.h"
#include "ParserManager.h"
#include "StreamDataManager.h"
#include "FileDataManager.h"
#include "RegisteredDataTypesManager.h"
#include "SourceManager.h"
#include "VisualizerManager.h"
#include "ApplicationCommon.h"
#include "PluginApplication.h"
#include "Log.h"
#include "LogInitializer.h"
#include "Plugin.h"
#include "PluginLoader.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

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

//using std::min;
//using std::max;

#endif  // DISABLE_PRECOMPILED_HEADERS
#endif  // __HEADER_GUARD__COREPCH_H__
