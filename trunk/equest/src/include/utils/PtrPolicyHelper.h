/********************************************************************
	created:  2011/02/15
	created:  15:2:2011   9:41
	filename: PtrPolicyHelper.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__PTRPOLICYHELPER_H__
#define HEADER_GUARD_UTILS__PTRPOLICYHELPER_H__

#include <type_traits>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

    template <class T> struct is_ptr_policy : public std::false_type {};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif  // HEADER_GUARD_UTILS__PTRPOLICYHELPER_H__
