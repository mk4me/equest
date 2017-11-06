/********************************************************************
	created:	2013/08/13
	created:	13:8:2013   8:16
	filename: 	Export.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__EXPORT_H__
#define HEADER_GUARD_DICOM__EXPORT_H__

#include <utils/Export.h>

#if defined (plugin_dicom_EXPORTS)
#  define PLUGIN_DICOM_EXPORT UTILS_DECL_EXPORT
#else
#  define PLUGIN_DICOM_EXPORT UTILS_DECL_IMPORT
#endif

#endif
