/********************************************************************
	created:  2010/10/19
	created:  19:10:2010   11:43
	filename: Export.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__PLUGINC3D_EXPORT_H__
#define __HEADER_GUARD__PLUGINC3D_EXPORT_H__

#include <utils/Export.h>

#if defined (plugin_c3d_EXPORTS)
#  define PLUGINC3D_EXPORT UTILS_DECL_EXPORT
#  define PLUGINC3D_EXTERN
#else
#  define PLUGINC3D_EXPORT UTILS_DECL_IMPORT
#  define PLUGINC3D_EXTERN UTILS_EXTERN_DECL
#endif

#endif  // __HEADER_GUARD__PLUGINC3D_EXPORT_H__
