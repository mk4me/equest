/********************************************************************
	created:	2015/01/03
	created:	03:01:2015   8:49
	filename: 	Export.h
	author:		Mateusz Janiak
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_IMU__EXPORT_H__
#define HEADER_GUARD_IMU__EXPORT_H__

#include <utils/Export.h>

#if defined (plugin_imuCostume_EXPORTS)
#  define IMU_EXPORT UTILS_DECL_EXPORT
#else
#  define IMU_EXPORT UTILS_DECL_IMPORT
#endif


#endif
