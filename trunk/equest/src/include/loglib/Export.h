/********************************************************************
	created:	2015/07/22
	created:	8:40
	filename: 	Export.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_LOGLIB__Export_H__
#define HEADER_GUARD_LOGLIB__Export_H__

#include <utils/Export.h>

#if defined (loglib_EXPORTS)
#define LOGLIB_EXPORT UTILS_DECL_EXPORT
#else
#define LOGLIB_EXPORT UTILS_DECL_IMPORT
#endif

#endif // HEADER_GUARD 
