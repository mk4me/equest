/********************************************************************
	created:	2015/07/17
	created:	12:17
	filename: 	PythonPluginUtils.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PythonPluginUtils_H__
#define HEADER_GUARD_PYTHON__PythonPluginUtils_H__

#include <plugins/python/python/Export.h>
#include <boost/python/module.hpp>
#include <string>

namespace core {
	class IServiceManager;
	class ISourceManager;
	class IVisualizerManager;
	class IDataManagerReader;
	class IDataHierarchyManager;
}

namespace  python {
	class PLUGIN_PYTHON_EXPORT PythonPluginUtils {
		public:
			virtual ~PythonPluginUtils() {}
		protected:
			core::IServiceManager* getServiceManager();
			core::ISourceManager* getSourceManager();
			core::IDataHierarchyManager* getHierarchyManager();
			core::IVisualizerManager* getVisualizerManager();
			core::IDataManagerReader* getDataManagerReader();
			void pluginLog(const std::string&);
	};
	DEFINE_SMART_POINTERS(PythonPluginUtils);
}
#ifdef _DEBUG
	#define MDE_PYTHON_MODULE(name) BOOST_PYTHON_MODULE(name##d)
#else
	#define MDE_PYTHON_MODULE(name) BOOST_PYTHON_MODULE(name)
#endif

#endif // HEADER_GUARD 
