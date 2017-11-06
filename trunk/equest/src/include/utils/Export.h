/********************************************************************
	created:  2010/10/19
	created:  19:10:2010   11:43
	filename: Export.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__UTILS_EXPORT_H__
#define __HEADER_GUARD__UTILS_EXPORT_H__

//! Te definy pochodz¹ z CMake - dodajemy je w Logic/ProjectUtils.cmake w zlae¿noœci od platformy
#ifndef UTILS_DECL_EXPORT
#  if defined(_WINDOWS)
#    define UTILS_DECL_EXPORT __declspec(dllexport)
#  elif defined(__UNIX__)
#    define UTILS_DECL_EXPORT __attribute__((visibility("default")))
#    define UTILS_DECL_HIDDEN __attribute__((visibility("hidden")))
#  endif
#  ifndef UTILS_DECL_EXPORT
#    define UTILS_DECL_EXPORT
#  endif
#endif
#ifndef UTILS_DECL_IMPORT
#  if defined(_WINDOWS)
#    define UTILS_DECL_IMPORT __declspec(dllimport)
#  else
#    define UTILS_DECL_IMPORT
#  endif
#endif
#ifndef UTILS_DECL_HIDDEN
#  define UTILS_DECL_HIDDEN
#endif

# define UTILS_EXTERN_DECL extern

#endif  // __HEADER_GUARD__UTILS_EXPORT_H__
