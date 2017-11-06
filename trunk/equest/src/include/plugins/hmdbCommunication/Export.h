/********************************************************************
	created:	2013/07/26
	created:	26:7:2013   8:49
	filename: 	Export.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWCOMMUNICATION__EXPORT_H__
#define HEADER_GUARD_NEWCOMMUNICATION__EXPORT_H__

#include <utils/Export.h>

#if defined (plugin_hmdbCommunication_EXPORTS)
#  define HMDBCOMMUNICATION_EXPORT UTILS_DECL_EXPORT
#else
#  define HMDBCOMMUNICATION_EXPORT UTILS_DECL_IMPORT
#endif


#endif
