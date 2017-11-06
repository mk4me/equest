/********************************************************************
	created:  2010/10/19
	created:  19:10:2010   11:43
	filename: Export.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__PLUGINSUBJECT_EXPORT_H__
#define __HEADER_GUARD__PLUGINSUBJECT_EXPORT_H__

#include <utils/Export.h>

#if defined (plugin_subject_EXPORTS)
#  define PLUGINSUBJECT_EXPORT UTILS_DECL_EXPORT
#  define PLUGINSUBJECT_EXTERN
#else
#  define PLUGINSUBJECT_EXPORT UTILS_DECL_IMPORT
#  define PLUGINSUBJECT_EXTERN UTILS_EXTERN_DECL
#endif

#endif  // __HEADER_GUARD__PLUGINSUBJECT_EXPORT_H__
