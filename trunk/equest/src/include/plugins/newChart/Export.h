#ifndef __HEADER_GUARD__NEWCHART_EXPORT_H__
#define __HEADER_GUARD__NEWCHART_EXPORT_H__

#include <utils/Export.h>

#if defined (plugin_newChart_EXPORTS)
#  define NEWCHART_EXPORT UTILS_DECL_EXPORT
#else	  
#  define NEWCHART_EXPORT UTILS_DECL_IMPORT
#endif

#endif  