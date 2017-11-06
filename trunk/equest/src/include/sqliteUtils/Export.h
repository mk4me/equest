/********************************************************************
	created:  2014/09/24	11:17:26
	filename: Export.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__EXPORT_H__
#define __HEADER_GUARD_SQLITEUTILS__EXPORT_H__

#include <utils/Export.h>

#if defined (sqliteUtils_EXPORTS)
#  define SQLITEUTILS_EXPORT UTILS_DECL_EXPORT
#else
#  define SQLITEUTILS_EXPORT UTILS_DECL_IMPORT
#endif

#endif	// __HEADER_GUARD_SQLITEUTILS__EXPORT_H__