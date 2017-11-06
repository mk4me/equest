#ifndef __HEADER_GUARD__KINEMATIC_EXPORT_H__
#define __HEADER_GUARD__KINEMATIC_EXPORT_H__

#include <utils/Export.h>

#if defined (plugin_kinematic_EXPORTS)
#  define KINEMATIC_EXPORT UTILS_DECL_EXPORT
#else	  
#  define KINEMATIC_EXPORT UTILS_DECL_IMPORT
#endif

#endif  