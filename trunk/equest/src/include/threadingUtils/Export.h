/********************************************************************
	created:  2010/10/19
	created:  19:10:2010   11:43
	filename: Export.h
	author:	  Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD__THREADINGUTILS_EXPORT_H__
#define __HEADER_GUARD__THREADINGUTILS_EXPORT_H__

#include <utils/Export.h>

#if defined (threadingUtils_EXPORTS)
#  define THREADINGUTILS_EXPORT UTILS_DECL_EXPORT
#else
#  define THREADINGUTILS_EXPORT UTILS_DECL_IMPORT
#endif

#endif  // __HEADER_GUARD__HMDBSERVICES_EXPORT_H__
