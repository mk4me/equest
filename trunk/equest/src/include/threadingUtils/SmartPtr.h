/********************************************************************
	created:  2015/08/24	12:44:44
	filename: SmartPtr.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADING_UTILS__SMARTPTR_H__
#define __HEADER_GUARD_THREADING_UTILS__SMARTPTR_H__

#include <cstddef>
#include <functional>
#include <boost/type_traits.hpp>
#include <utils/SmartPtr.h>
#include <utils/Macros.h>
#include <threadingUtils/ThreadAfinityVerifier.h>

namespace threadingUtils
{
	template<class T>
	class shared_ptr;

	template<class T,	class... _Types>
	shared_ptr<T> make_shared(_Types&&... _Args);

	template<class T,
	class _Alloc,
	class... _Types> inline
		shared_ptr<T> allocate_shared(
		const _Alloc& _Al_arg, _Types&&... _Args);

	template<typename T>
	class weak_ptr;

	template<class _Ty, class _Dx = std::default_delete<_Ty>>
	class unique_ptr;

	// TEMPLATE CLASS shared_ptr
	template<class _Ty>
	class shared_ptr : private ThreadAfinityVerifier
	{	// class for reference counted resource management

		template<typename T>
		friend class shared_ptr;

		template<class T, class... _Types>
		friend shared_ptr<T> make_shared(_Types&&... _Args);

		template<class T,
		class _Alloc,
		class... _Types> inline
		friend shared_ptr<T> allocate_shared(
			const _Alloc& _Al_arg, _Types&&... _Args);

		template<class T>
		friend class weak_ptr;

		template<typename T, class Dd>
		friend class unique_ptr;

	private:

		shared_ptr(utils::shared_ptr<_Ty> _Other) : ptr(_Other)
		{	// construct empty shared_ptr
		}

	public:

		//typedef shared_ptr<_Ty> shared_ptr<_Ty>;

		//shared_ptr<_Ty>* operator&() const = delete;

		shared_ptr() NOEXCEPT
		{	// construct empty shared_ptr
		}

		template<class _Ux>
		explicit shared_ptr(_Ux *_Px) : ptr(_Px)
		{	// construct shared_ptr object that owns _Px			
		}

		template<class _Ux,
		class _Dx>
			shared_ptr(_Ux *_Px, _Dx _Dt) : ptr(_Px, _Dt)
		{	// construct with _Px, deleter			
		}

		shared_ptr(nullptr_t) : ptr(nullptr)
		{	// construct empty shared_ptr
		}

		template<class _Dx>
		shared_ptr(nullptr_t, _Dx _Dt) : ptr(nullptr, _Dt)
		{	// construct with nullptr, deleter
		}

		template<class _Dx,
		class _Alloc>
			shared_ptr(nullptr_t, _Dx _Dt, _Alloc _Ax)
			: ptr(nullptr, _Dt, _Ax)
		{	// construct with nullptr, deleter, allocator			
		}

		template<class _Ux,
		class _Dx,
		class _Alloc>
			shared_ptr(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
			: ptr(_Px, _Dt, _Ax)
		{	// construct with _Px, deleter, allocator			
		}

		template<class _Ty2>
		shared_ptr(const shared_ptr<_Ty2>& _Right, _Ty *_Px)
			: ptr(_Right.ptr, _Px)
		{	// construct shared_ptr object that aliases _Right
			_Right.verifyCurrentThreadAfinity();
		}

		shared_ptr(const shared_ptr<_Ty>& _Other)
			: ptr(_Other.ptr)
		{	// construct shared_ptr object that owns same resource as _Other
			_Other.verifyCurrentThreadAfinity();
		}

		template<class _Ty2,
		class = typename std::enable_if<std::is_convertible<_Ty2 *, _Ty *>::value,
			void>::type>
		shared_ptr(const shared_ptr<_Ty2>& _Other)
		: ptr(_Other.ptr)
		{	// construct shared_ptr object that owns same resource as _Other
			_Other.verifyCurrentThreadAfinity();
		}

		template<class _Ty2>
		explicit shared_ptr(const weak_ptr<_Ty2>& _Other,
			bool _Throw = true)
			: ptr(_Other.ptr, _Throw)
		{	// construct shared_ptr object that owns resource *_Other
			_Other.verifyCurrentThreadAfinity();
		}

		//template<class _Ty2>
		//shared_ptr(auto_ptr<_Ty2>&& _Other)
		//	: ptr(std::move(_Other.ptr))
		//{	// construct shared_ptr object that owns *_Other.get()
		//	try{
		//		_Other.verifyCurrentThreadAfinity();
		//	}
		//	catch (....){
		//		_Other.ptr = std::move(ptr);
		//		throw;
		//	}
		//}

		/*template<class _Ty2>
		shared_ptr(const shared_ptr<_Ty2>& _Other, const std::_Static_tag& _Tag)
			: ptr(_Other.ptr, _Tag)
		{	// construct shared_ptr object for static_pointer_cast
			_Other.verifyCurrentThreadAfinity();
		}

		template<class _Ty2>
		shared_ptr(const shared_ptr<_Ty2>& _Other, const std::_Const_tag& _Tag)
			: ptr(_Other.ptr, _Tag)
		{	// construct shared_ptr object for const_pointer_cast
			_Other.verifyCurrentThreadAfinity();
		}

		template<class _Ty2>
		shared_ptr(const shared_ptr<_Ty2>& _Other, const std::_Dynamic_tag& _Tag)
			: ptr(_Other.ptr, _Tag)
		{	// construct shared_ptr object for dynamic_pointer_cast
			_Other.verifyCurrentThreadAfinity();
		}*/

		shared_ptr(shared_ptr<_Ty>&& _Right)
		{	// construct shared_ptr object that takes resource from _Right			
			_Right.verifyCurrentThreadAfinity();
			ptr = std::move(_Right.ptr);
		}

		template<class _Ty2,
		class = typename std::enable_if<std::is_convertible<_Ty2 *, _Ty *>::value,
			void>::type>
		shared_ptr(shared_ptr<_Ty2>&& _Right)
		{	// construct shared_ptr object that takes resource from _Right			
			_Right.verifyCurrentThreadAfinity();
			ptr = std::move(_Right.ptr);
		}

		template<class _Ux,
		class _Dx>
		shared_ptr(unique_ptr<_Ux, _Dx>&& _Right)	
		{	// construct from unique_ptr			
			_Right.verifyCurrentThreadAfinity();
			ptr = std::move(_Right.ptr);
		}

		template<class _Ux,
		class _Dx>
		shared_ptr<_Ty>& operator=(unique_ptr<_Ux, _Dx>&& _Right)
		{	// move from unique_ptr			
			shared_ptr(std::move(_Right)).swap(*this);
			return (*this);
		}

		shared_ptr<_Ty>& operator=(shared_ptr<_Ty>&& _Right)
		{	// construct shared_ptr object that takes resource from _Right			
			shared_ptr(std::move(_Right)).swap(*this);
			return (*this);
		}

		template<class _Ty2>
		shared_ptr<_Ty>& operator=(shared_ptr<_Ty2>&& _Right)
		{	// construct shared_ptr object that takes resource from _Right			
			shared_ptr(std::move(_Right)).swap(*this);
			return (*this);
		}

		~shared_ptr() NOEXCEPT
		{	// release resource			
		}

		shared_ptr<_Ty>& operator=(const shared_ptr<_Ty>& _Right)
		{	// assign shared ownership of resource owned by _Right			
			shared_ptr(_Right).swap(*this);
			return (*this);
		}

		template<class _Ty2>
		shared_ptr<_Ty>& operator=(const shared_ptr<_Ty2>& _Right)
		{	// assign shared ownership of resource owned by _Right			
			shared_ptr(_Right).swap(*this);
			return (*this);
		}

		//template<class _Ty2>
		//shared_ptr<_Ty>& operator=(auto_ptr<_Ty2>&& _Right)
		//{	// assign ownership of resource pointed to by _Right			
		//	shared_ptr(_STD move(_Right)).swap(*this);
		//	return (*this);
		//}

		void reset()
		{	// release resource and convert to empty shared_ptr object			
			shared_ptr().swap(*this);
		}

		template<class _Ux>
		void reset(_Ux *_Px)
		{	// release, take ownership of _Px			
			shared_ptr(_Px).swap(*this);
		}

		template<class _Ux,
		class _Dx>
		void reset(_Ux *_Px, _Dx _Dt)
		{	// release, take ownership of _Px, with deleter _Dt			
			shared_ptr(_Px, _Dt).swap(*this);
		}

		template<class _Ux,
		class _Dx,
		class _Alloc>
		void reset(_Ux *_Px, _Dx _Dt, _Alloc _Ax)
		{	// release, take ownership of _Px, with deleter _Dt, allocator _Ax
			shared_ptr(_Px, _Dt, _Ax).swap(*this);
		}

		void swap(shared_ptr<_Ty> & _Other)
		{	// swap pointers
			verifyCurrentThreadAfinity();
			_Other.verifyCurrentThreadAfinity();
			ptr.swap(_Other.ptr);
		}

		_Ty *get() const
		{	// return pointer to resource
			verifyCurrentThreadAfinity();
			return ptr.get();
		}

		typename boost::add_reference<_Ty>::type operator*() const
		{	// return reference to resource
			verifyCurrentThreadAfinity();
			return *ptr;
		}

		_Ty *operator->() const
		{	// return pointer to resource
			verifyCurrentThreadAfinity();
			return ptr.operator ->();
		}

		bool unique() const NOEXCEPT
		{	// return true if no other shared_ptr object owns this resource
			return ptr.unique();
		}

		explicit operator bool() const NOEXCEPT
		{	// test if shared_ptr object owns no resource
			return ptr;
		}

	private:
		// Faktyczny obiekt smart pointera
		utils::shared_ptr<_Ty> ptr;
	};

	template<class _Ty1,
	class _Ty2>
		bool operator==(const shared_ptr<_Ty1>& _Left,
		const shared_ptr<_Ty2>& _Right)
	{	// test if shared_ptr == shared_ptr
		return (_Left.get() == _Right.get());
	}

		template<class _Ty1,
	class _Ty2>
		bool operator!=(const shared_ptr<_Ty1>& _Left,
		const shared_ptr<_Ty2>& _Right)
	{	// test if shared_ptr != shared_ptr
		return (!(_Left == _Right));
	}

	/*	template<class _Ty1,
	class _Ty2>
		bool operator<(const shared_ptr<_Ty1>& _Left,
		const shared_ptr<_Ty2>& _Right)
	{	// test if shared_ptr < shared_ptr
		return (std::less<decltype(_Always_false<_Ty1>::value
		? _Left.get() : _Right.get())>()(
		_Left.get(), _Right.get()));
	}*/

		template<class _Ty1,
	class _Ty2>
		bool operator>=(const shared_ptr<_Ty1>& _Left,
		const shared_ptr<_Ty2>& _Right)
	{	// shared_ptr >= shared_ptr
		return (!(_Left < _Right));
	}

		template<class _Ty1,
	class _Ty2>
		bool operator>(const shared_ptr<_Ty1>& _Left,
		const shared_ptr<_Ty2>& _Right)
	{	// test if shared_ptr > shared_ptr
		return (_Right < _Left);
	}

		template<class _Ty1,
	class _Ty2>
		bool operator<=(const shared_ptr<_Ty1>& _Left,
		const shared_ptr<_Ty2>& _Right)
	{	// test if shared_ptr <= shared_ptr
		return (!(_Right < _Left));
	}

		template<class _Ty>
	bool operator==(const shared_ptr<_Ty>& _Left,
		nullptr_t)
	{	// test if shared_ptr == nullptr
		return (_Left.get() == (_Ty *)0);
	}

		template<class _Ty>
	bool operator==(nullptr_t,
		const shared_ptr<_Ty>& _Right)
	{	// test if nullptr == shared_ptr
		return ((_Ty *)0 == _Right.get());
	}

		template<class _Ty>
	bool operator!=(const shared_ptr<_Ty>& _Left,
		nullptr_t _Right)
	{	// test if shared_ptr != nullptr
		return (!(_Left == _Right));
	}

		template<class _Ty>
	bool operator!=(nullptr_t _Left,
		const shared_ptr<_Ty>& _Right)
	{	// test if nullptr != shared_ptr
		return (!(_Left == _Right));
	}

		template<class _Ty>
	bool operator<(const shared_ptr<_Ty>& _Left,
		nullptr_t _Right)
	{	// test if shared_ptr < nullptr
		return (std::less<_Ty *>()(_Left.get(), (_Ty *)0));
	}

		template<class _Ty>
	bool operator<(nullptr_t _Left,
		const shared_ptr<_Ty>& _Right)
	{	// test if nullptr < shared_ptr
		return (std::less<_Ty *>()((_Ty *)0, _Right.get()));
	}

		template<class _Ty>
	bool operator>=(const shared_ptr<_Ty>& _Left,
		nullptr_t _Right)
	{	// test if shared_ptr >= nullptr
		return (!(_Left < _Right));
	}

		template<class _Ty>
	bool operator>=(nullptr_t _Left,
		const shared_ptr<_Ty>& _Right)
	{	// test if nullptr >= shared_ptr
		return (!(_Left < _Right));
	}

		template<class _Ty>
	bool operator>(const shared_ptr<_Ty>& _Left,
		nullptr_t _Right)
	{	// test if shared_ptr > nullptr
		return (_Right < _Left);
	}

		template<class _Ty>
	bool operator>(nullptr_t _Left,
		const shared_ptr<_Ty>& _Right)
	{	// test if nullptr > shared_ptr
		return (_Right < _Left);
	}

		template<class _Ty>
	bool operator<=(const shared_ptr<_Ty>& _Left,
		nullptr_t _Right)
	{	// test if shared_ptr <= nullptr
		return (!(_Right < _Left));
	}

		template<class _Ty>
	bool operator<=(nullptr_t _Left,
		const shared_ptr<_Ty>& _Right)
	{	// test if nullptr <= shared_ptr
		return (!(_Right < _Left));
	}

		template<class _Elem,
	class _Traits,
	class _Ty>
		std::basic_ostream<_Elem, _Traits>&
		operator<<(std::basic_ostream<_Elem, _Traits>& _Out,
		const shared_ptr<_Ty>& _Px)
	{	// write contained pointer to stream
		return (_Out << _Px.get());
	}

	template<class _Ty>
	void swap(shared_ptr<_Ty>& _Left,
		shared_ptr<_Ty>& _Right)
	{	// swap _Left and _Right shared_ptrs
		_Left.swap(_Right);
	}

		template<class _Ty1,
	class _Ty2>
		shared_ptr<_Ty1>
		static_pointer_cast(const shared_ptr<_Ty2>& _Other)
	{	// return shared_ptr object holding static_cast<_Ty1 *>(_Other.get())
		return (shared_ptr<_Ty1>(_Other, _Static_tag()));
	}

		template<class _Ty1,
	class _Ty2>
		shared_ptr<_Ty1>
		const_pointer_cast(const shared_ptr<_Ty2>& _Other)
	{	// return shared_ptr object holding const_cast<_Ty1 *>(_Other.get())
		return (shared_ptr<_Ty1>(_Other, _Const_tag()));
	}

		template<class _Ty1,
	class _Ty2>
		shared_ptr<_Ty1>
		dynamic_pointer_cast(const shared_ptr<_Ty2>& _Other)
	{	// return shared_ptr object holding dynamic_cast<_Ty1 *>(_Other.get())
		return (shared_ptr<_Ty1>(_Other, _Dynamic_tag()));
	}

	// TEMPLATE FUNCTION make_shared
	template<class _Ty,
	class... _Types> inline
		shared_ptr<_Ty> make_shared(_Types&&... _Args)
	{	// make a shared_ptr
		return shared_ptr<_Ty>(utils::make_shared<_Ty>(std::move(_Args)...));
	}

	// TEMPLATE FUNCTION allocate_shared
	template<class _Ty,
	class _Alloc,
	class... _Types> inline
		shared_ptr<_Ty> allocate_shared(
		const _Alloc& _Al_arg, _Types&&... _Args)
	{	// make a shared_ptr
		return shared_ptr<_Ty>(utils::allocate_shared<_Ty>(_Al_arg, std::move(_Args)...));
	}

	// TEMPLATE CLASS weak_ptr
	template<class _Ty>
	class weak_ptr : private ThreadAfinityVerifier
	{	// class for pointer to reference counted resource

		template<typename T>
		friend class shared_ptr;

	public:

		weak_ptr* operator&() const volatile = delete;

		weak_ptr() NOEXCEPT
		{	// construct empty weak_ptr object
		}

		weak_ptr(const weak_ptr& _Other):
		ptr(_Other.ptr)
		{	// construct weak_ptr object for resource pointed to by _Other
			_Other.verifyCurrentThreadAfinity();
		}

		template<class _Ty2,
		class = typename std::enable_if<std::is_convertible<_Ty2 *, _Ty *>::value,
			void>::type>
			weak_ptr(const shared_ptr<_Ty2>& _Other) : ptr(_Other.ptr)
		{	// construct weak_ptr object for resource owned by _Other
			_Other.verifyCurrentThreadAfinity();
		}

			template<class _Ty2,
			class = typename std::enable_if<std::is_convertible<_Ty2 *, _Ty *>::value,
			void>::type>
			weak_ptr(const weak_ptr<_Ty2>& _Other) : ptr(_Other.ptr)
		{	// construct weak_ptr object for resource pointed to by _Other
			_Other.verifyCurrentThreadAfinity();
		}

		~weak_ptr() NOEXCEPT
		{	// release resource
		}

		weak_ptr& operator=(const weak_ptr& _Right)
		{	// assign from _Right
			_Right.verifyCurrentThreadAfinity();
			ptr = _Right.ptr;
			return (*this);
		}

		template<class _Ty2>
		weak_ptr& operator=(const weak_ptr<_Ty2>& _Right)
		{	// assign from _Right
			_Right.verifyCurrentThreadAfinity();
			ptr = _Right.ptr;
			return (*this);
		}

		template<class _Ty2>
		weak_ptr& operator=(const shared_ptr<_Ty2>& _Right)
		{	// assign from _Right
			_Right.verifyCurrentThreadAfinity();
			ptr = _Right.ptr;
			return (*this);
		}

		void reset()
		{	// release resource, convert to null weak_ptr object
			verifyCurrentThreadAfinity();
			ptr.reset();
		}

		void swap(weak_ptr& _Other)
		{	// swap pointers
			verifyCurrentThreadAfinity();
			_Other.verifyCurrentThreadAfinity();
			ptr.swap(_Other.ptr);
		}

		bool expired() const NOEXCEPT
		{	// return true if resource no longer exists
			return ptr.expired();
		}

		shared_ptr<_Ty> lock() const
		{	// convert to shared_ptr
			verifyCurrentThreadAfinity();
			return (shared_ptr<_Ty>(*this, false));
		}

	private:
		//! Faktyczny smart pointer
		utils::weak_ptr<_Ty> ptr;
	};

	template<class _Ty>
	void swap(weak_ptr<_Ty>& _W1, weak_ptr<_Ty>& _W2)
	{	// swap contents of _W1 and _W2
		_W1.swap(_W2);
	}

		// TEMPLATE CLASS enable_shared_from_this
	template<class _Ty>
	class enable_shared_from_this : private utils::enable_shared_from_this<_Ty>, private ThreadAfinityVerifier
	{	// provide member functions that create shared_ptr to this
	public:

		shared_ptr<_Ty> shared_from_this()
		{	// return shared_ptr
			verifyCurrentThreadAfinity();
			return (shared_ptr<_Ty>(_Wptr));
		}

		shared_ptr<const _Ty> shared_from_this() const
		{	// return shared_ptr
			verifyCurrentThreadAfinity();
			return (shared_ptr<const _Ty>(_Wptr));
		}

	protected:
		enable_shared_from_this() NOEXCEPT
		{	// construct (do nothing)
		}

		enable_shared_from_this(const enable_shared_from_this& _Other)
		{	// construct (do nothing)
			_Other.verifyCurrentThreadAfinity();
		}

		enable_shared_from_this&
		operator=(const enable_shared_from_this& _Other)
		{	// assign (do nothing)
			verifyCurrentThreadAfinity();
			_Other.verifyCurrentThreadAfinity();
			return (*this);
		}

		~enable_shared_from_this() NOEXCEPT
		{	// destroy (do nothing)
		}

		mutable weak_ptr<_Ty> _Wptr;
	};

	// TEMPLATE CLASS unique_ptr SCALAR
	template<class _Ty,
	class _Dx>	// = default_delete<_Ty>
	class unique_ptr : private ThreadAfinityVerifier
	{	// non-copyable pointer to an object
	public:
		typedef typename utils::unique_ptr<_Ty>::pointer pointer;
		typedef _Ty element_type;
		typedef _Dx deleter_type;

		typedef unique_ptr<_Ty, _Dx> __Myt;

		unique_ptr<_Ty, _Dx>* operator&() const volatile = delete;

		unique_ptr() NOEXCEPT
		{	// default construct			
		}

		unique_ptr(nullptr_t) NOEXCEPT
		{	// null pointer construct			
		}

		unique_ptr<_Ty, _Dx>& operator=(nullptr_t)
		{	// assign a null pointer
			reset();
			return (*this);
		}

		explicit unique_ptr(pointer _Ptr) NOEXCEPT
			: ptr(_Ptr)
		{	// construct with pointer			
		}

		unique_ptr(pointer _Ptr,
			typename std::conditional<std::is_reference<_Dx>::value, _Dx,
			const typename std::remove_reference<_Dx>::type&>::type _Dt) NOEXCEPT
			: ptr(_Ptr, _Dt)
		{	// construct with pointer and (maybe const) deleter&
		}

		unique_ptr(pointer _Ptr,
			typename std::remove_reference<_Dx>::type&& _Dt) NOEXCEPT
			: ptr(_Ptr, std::move(_Dt))
		{	// construct by moving deleter			
		}

		unique_ptr(unique_ptr<_Ty, _Dx>&& _Right)
			: ptr(_Right.release(),
			std::forward<_Dx>(_Right.ptr.get_deleter()))
		{	// construct by moving _Right
		}

		template<class _Ty2,
		class _Dx2,
		class = typename std::enable_if<!std::is_array<_Ty2>::value
			&& std::is_convertible<typename unique_ptr<_Ty2, _Dx2>::pointer,
			pointer>::value
			&& ((std::is_reference<_Dx>::value && std::is_same<_Dx, _Dx2>::value)
			|| (!std::is_reference<_Dx>::value
			&& std::is_convertible<_Dx2, _Dx>::value)),
			void>::type>
			unique_ptr(unique_ptr<_Ty2, _Dx2>&& _Right)
			: ptr(_Right.release(),
			std::forward<_Dx2>(_Right.ptr.get_deleter()))
		{	// construct by moving _Right
		}

		//template<class _Ty2,
		//class = typename enable_if<is_convertible<_Ty2 *, _Ty *>::value
		//	&& is_same<_Dx, default_delete<_Ty> >::value,
		//	void>::type>
		//	unique_ptr(auto_ptr<_Ty2>&& _Right) NOEXCEPT
		//	: ptr(_Right.release())
		//{	// construct by moving _Right
		//	try{
		//		_Right.verifyCurrentThreadAfinity();
		//	}
		//	catch (...){
		//		//TODO
		//		//_Right.ptr = utils::unique_ptr<_Ty2, _Dx2>(dynamic_cast<_Ty2*>(ptr.release()), dynamic_cast<_Dx2*>(ptr.get_deleter()));
		//		throw;
		//	}
		//}

		template<class _Ty2,
		class _Dx2>
			typename std::enable_if<!std::is_array<_Ty2>::value
			&& std::is_convertible<typename unique_ptr<_Ty2, _Dx2>::pointer,
			pointer>::value,
			unique_ptr<_Ty, _Dx>&>::type
			operator=(unique_ptr<_Ty2, _Dx2>&& _Right)
		{	// assign by moving _Right
			_Right.verifyCurrentThreadAfinity();
			reset(_Right.release());
			this->get_deleter() = std::forward<_Dx2>(_Right.ptr.get_deleter());
			return (*this);
		}

		unique_ptr<_Ty, _Dx>& operator=(unique_ptr<_Ty, _Dx>&& _Right)
		{	// assign by moving _Right
			if (this != &_Right)
			{	// different, do the move				
				reset(_Right.release());
				this->get_deleter() = std::forward<_Dx>(_Right.ptr.get_deleter());
			}
			return (*this);
		}

		void swap(unique_ptr<_Ty, _Dx>& _Right)
		{	// swap elements
			_Right.verifyCurrentThreadAfinity();
			ptr.swap(_Right.ptr);			
		}

		~unique_ptr() NOEXCEPT
		{	// destroy the object			
		}

		typename boost::add_reference<_Ty>::type operator*() const
		{	// return reference to object
			verifyCurrentThreadAfinity();
			return *ptr;
		}

		pointer operator->() const
		{	// return pointer to class object
			verifyCurrentThreadAfinity();
			return ptr.operator->();
		}

		pointer get() const
		{	// return pointer to object
			verifyCurrentThreadAfinity();
			return ptr.get();
		}

		explicit operator bool() const
		{	// test for non-null pointer			
			return (bool)ptr;
		}

		pointer release()
		{	// yield ownership of pointer
			verifyCurrentThreadAfinity();
			return ptr.release();
		}

		void reset(pointer _Ptr = pointer())
		{	// establish new pointer
			verifyCurrentThreadAfinity();
			ptr.reset(_Ptr);
		}

		std::remove_reference<_Dx>& get_deleter()
		{
			verifyCurrentThreadAfinity();
			return ptr.get_deleter();
		}

		const std::remove_reference<_Dx>& get_deleter() const
		{
			verifyCurrentThreadAfinity();
			return ptr.get_deleter();
		}

		typename boost::add_reference<_Ty>::type operator[](std::size_t _Idx) const
		{	// return reference to object
			verifyCurrentThreadAfinity();
			return ptr[_Idx];
		}

		unique_ptr(const unique_ptr<_Ty, _Dx>&) = delete;
		unique_ptr<_Ty, _Dx>& operator=(const unique_ptr<_Ty, _Dx>&) = delete;\

	private:

		utils::unique_ptr<_Ty, _Dx> ptr;
	};

	// TEMPLATE FUNCTION make_unique
	template<class _Ty,
	class... _Types> inline
		typename std::enable_if<!std::is_array<_Ty>::value,
		unique_ptr<_Ty> >::type make_unique(_Types&&... _Args)
	{	// make a unique_ptr
		return (unique_ptr<_Ty>(new _Ty(std::forward<_Types>(_Args)...)));
	}

	template<class _Ty> inline
		typename std::enable_if<std::is_array<_Ty>::value && std::extent<_Ty>::value == 0,
		unique_ptr<_Ty> >::type make_unique(std::size_t _Size)
	{	// make a unique_ptr
		typedef typename std::remove_extent<_Ty>::type _Elem;
		return (unique_ptr<_Ty>(new _Elem[_Size]()));
	}

	template<class _Ty,
	class... _Types>
		typename std::enable_if<std::extent<_Ty>::value != 0,
		void>::type make_unique(_Types&&...) = delete;


	template<class _Ty,
	class _Dx>
		void swap(unique_ptr<_Ty, _Dx>& _Left,
		unique_ptr<_Ty, _Dx>& _Right)
	{	// swap _Left with _Right
		_Left.swap(_Right);
	}

		template<class _Ty1,
	class _Dx1,
	class _Ty2,
	class _Dx2>
		bool operator==(const unique_ptr<_Ty1, _Dx1>& _Left,
		const unique_ptr<_Ty2, _Dx2>& _Right)
	{	// test if unique_ptr _Left equals _Right
		return (_Left.get() == _Right.get());
	}

	template<class _Ty1,
	class _Dx1,
	class _Ty2,
	class _Dx2>
		bool operator!=(const unique_ptr<_Ty1, _Dx1>& _Left,
		const unique_ptr<_Ty2, _Dx2>& _Right)
	{	// test if unique_ptr _Left doesn't equal _Right
		return (!(_Left == _Right));
	}

	template<class _Ty1,
	class _Dx1,
	class _Ty2,
	class _Dx2>
		bool operator<(const unique_ptr<_Ty1, _Dx1>& _Left,
		const unique_ptr<_Ty2, _Dx2>& _Right)
	{	// test if unique_ptr _Left precedes _Right
		typedef typename unique_ptr<_Ty1, _Dx1>::pointer _Ptr1;
		typedef typename unique_ptr<_Ty2, _Dx2>::pointer _Ptr2;
		typedef typename std::common_type<_Ptr1, _Ptr2>::type _Common;
		return (std::less<_Common>()(_Left.get(), _Right.get()));
	}

	template<class _Ty1,
	class _Dx1,
	class _Ty2,
	class _Dx2>
		bool operator>=(const unique_ptr<_Ty1, _Dx1>& _Left,
		const unique_ptr<_Ty2, _Dx2>& _Right)
	{	// test if unique_ptr _Left doesn't precede _Right
		return (!(_Left < _Right));
	}

	template<class _Ty1,
	class _Dx1,
	class _Ty2,
	class _Dx2>
		bool operator>(const unique_ptr<_Ty1, _Dx1>& _Left,
		const unique_ptr<_Ty2, _Dx2>& _Right)
	{	// test if unique_ptr _Right precedes _Left
		return (_Right < _Left);
	}

	template<class _Ty1,
	class _Dx1,
	class _Ty2,
	class _Dx2>
		bool operator<=(const unique_ptr<_Ty1, _Dx1>& _Left,
		const unique_ptr<_Ty2, _Dx2>& _Right)
	{	// test if unique_ptr _Right doesn't precede _Left
		return (!(_Right < _Left));
	}

	template<class _Ty,
	class _Dx>
		bool operator==(const unique_ptr<_Ty, _Dx>& _Left,
		nullptr_t) NOEXCEPT
	{	// test if unique_ptr == nullptr
		return (!_Left);
	}

		template<class _Ty,
	class _Dx>
		bool operator==(nullptr_t,
		const unique_ptr<_Ty, _Dx>& _Right) NOEXCEPT
	{	// test if nullptr == unique_ptr
		return (!_Right);
	}

		template<class _Ty,
	class _Dx>
		bool operator!=(const unique_ptr<_Ty, _Dx>& _Left,
		nullptr_t _Right) NOEXCEPT
	{	// test if unique_ptr != nullptr
		return (!(_Left == _Right));
	}

		template<class _Ty,
	class _Dx>
		bool operator!=(nullptr_t _Left,
		const unique_ptr<_Ty, _Dx>& _Right) NOEXCEPT
	{	// test if nullptr != unique_ptr
		return (!(_Left == _Right));
	}

		template<class _Ty,
	class _Dx>
		bool operator<(const unique_ptr<_Ty, _Dx>& _Left,
		nullptr_t _Right)
	{	// test if unique_ptr < nullptr
		typedef typename unique_ptr<_Ty, _Dx>::pointer _Ptr;
		return (std::less<_Ptr>()(_Left.get(), _Right));
	}

	template<class _Ty,
	class _Dx>
		bool operator<(nullptr_t _Left,
		const unique_ptr<_Ty, _Dx>& _Right)
	{	// test if nullptr < unique_ptr
		typedef typename unique_ptr<_Ty, _Dx>::pointer _Ptr;
		return (std::less<_Ptr>()(_Left, _Right.get()));
	}

	template<class _Ty,
	class _Dx>
		bool operator>=(const unique_ptr<_Ty, _Dx>& _Left,
		nullptr_t _Right)
	{	// test if unique_ptr >= nullptr
		return (!(_Left < _Right));
	}

	template<class _Ty,
	class _Dx>
		bool operator>=(nullptr_t _Left,
		const unique_ptr<_Ty, _Dx>& _Right)
	{	// test if nullptr >= unique_ptr
		return (!(_Left < _Right));
	}

	template<class _Ty,
	class _Dx>
		bool operator>(const unique_ptr<_Ty, _Dx>& _Left,
		nullptr_t _Right)
	{	// test if unique_ptr > nullptr
		return (_Right < _Left);
	}

	template<class _Ty,
	class _Dx>
		bool operator>(nullptr_t _Left,
		const unique_ptr<_Ty, _Dx>& _Right)
	{	// test if nullptr > unique_ptr
		return (_Right < _Left);
	}

	template<class _Ty,
	class _Dx>
		bool operator<=(const unique_ptr<_Ty, _Dx>& _Left,
		nullptr_t _Right)
	{	// test if unique_ptr <= nullptr
		return (!(_Right < _Left));
	}

	template<class _Ty,
	class _Dx>
		bool operator<=(nullptr_t _Left,
		const unique_ptr<_Ty, _Dx>& _Right)
	{	// test if nullptr <= unique_ptr
		return (!(_Right < _Left));
	}
}

#endif	// __HEADER_GUARD_THREADING_UTILS__SMARTPTR_H__
