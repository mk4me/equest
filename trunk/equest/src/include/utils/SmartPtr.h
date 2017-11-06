/********************************************************************
	created:  2010/10/21
	created:  21:10:2010   16:48
	filename: Ptr.h
	author:	  Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__SMARTPTR_H__
#define __HEADER_GUARD_UTILS__SMARTPTR_H__

#include <utils/memory.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
	////////////////////////////////////////////////////////////////////////////////

#ifdef UTILS_USE_BOOST_SMART_POINTERS

	using boost::enable_shared_from_this;
	using boost::shared_array;
	using boost::shared_ptr;
	using boost::weak_ptr;
	template<typename T> using unique_ptr = boost::unique_ptr<T>;
	using boost::dynamic_pointer_cast;
	using boost::static_pointer_cast;
	using boost::const_pointer_cast;
	using boost::make_shared;
	using boost::make_unique;
	using boost::allocate_shared;

#elif defined UTILS_CXX0X

	template< typename T >
	using array_deleter = std::default_delete<T[]>;

	using std::enable_shared_from_this;
	using std::shared_ptr;
	template<typename T> using shared_array = std::shared_ptr<T>;
	using std::weak_ptr;
	template<typename T, class Dd = std::default_delete<T>> using unique_ptr = std::unique_ptr<T, Dd>;
	using std::dynamic_pointer_cast;
	using std::static_pointer_cast;
	using std::const_pointer_cast;
	using std::make_shared;
	using std::make_unique;
	using std::allocate_shared;	

#endif

	////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#ifdef UTILS_USE_BOOST_SMART_POINTERS

#elif defined UTILS_CXX0X

namespace std
{

template<typename T>
struct less < std::weak_ptr<T> >
{
	bool operator()(const std::weak_ptr<T>& _Left, const std::weak_ptr<T>& _Right) const
	{	// apply operator< to operands
		return (_Left.lock() < _Right.lock());
	}
};

}

#endif

#define DEFINE_SMART_POINTERS_EXT(className, pointerName)                    \
	using pointerName ## Ptr = utils::shared_ptr<className>;                 \
	using pointerName ## ConstPtr = utils::shared_ptr<const className>;      \
	using pointerName ## WeakPtr = utils::weak_ptr<className>;               \
	using pointerName ## ConstWeakPtr = utils::weak_ptr<const className>;

#define DEFINE_SMART_POINTERS(className)                                  \
	DEFINE_SMART_POINTERS_EXT(className, className)

#define DEFINE_SCOPED_SMART_POINTERS(className, targetNamespace)                                    \
	using targetNamespace ## :: ## className ## Ptr = utils::shared_ptr<className>;                  \
	using targetNamespace ## :: ## className ## ConstPtr = utils::shared_ptr<const className>;       \
	using targetNamespace ## :: ## className ## WeakPtr = utils::weak_ptr<className>;                \
	using targetNamespace ## :: ## className ## ConstWeakPtr = utils::weak_ptr<const className>;

#endif  // __HEADER_GUARD_CORE__SMARTPTR_H__
