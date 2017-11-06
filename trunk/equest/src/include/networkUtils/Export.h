/********************************************************************
	created:  2014/05/14	20:02:10
	filename: Export.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__EXPORT_H__
#define __HEADER_GUARD_NETWORKUTILS__EXPORT_H__

#include <utils/Export.h>

#if defined (networkUtils_EXPORTS)
#  define NETWORKUTILS_EXPORT UTILS_DECL_EXPORT
#else
#  define NETWORKUTILS_EXPORT UTILS_DECL_IMPORT
#endif

#endif	// __HEADER_GUARD_NETWORKUTILS__EXPORT_H__