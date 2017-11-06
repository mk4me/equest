/********************************************************************
	created:	2015/07/17
	created:	12:19
	filename: 	Export.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__Export_H__
#define HEADER_GUARD_PYTHON__Export_H__

#include <utils/Export.h>

#if defined (plugin_python_EXPORTS)
#  define PLUGIN_PYTHON_EXPORT UTILS_DECL_EXPORT
#else			 
#  define PLUGIN_PYTHON_EXPORT UTILS_DECL_IMPORT
#endif

#endif  
