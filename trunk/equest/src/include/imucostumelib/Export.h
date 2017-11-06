/********************************************************************
	created:  2014/01/09
	created:  9:1:2014   8:29
	filename: Export.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_IMUCOSTUME__EXPORT_H__
#define HEADER_GUARD_IMUCOSTUME__EXPORT_H__

#include <utils/Export.h>

#if defined (imucostumelib_EXPORTS)
#  define IMUCOSTUME_EXPORT UTILS_DECL_EXPORT
#else
#  define IMUCOSTUME_EXPORT UTILS_DECL_IMPORT
#endif

#endif	//	HEADER_GUARD_IMUCOSTUME__EXPORT_H__
