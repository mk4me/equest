/********************************************************************
 created:  2015/10/05	19:31:30
 filename: memory.h
 author:	  Mateusz Janiak

 purpose:
 *********************************************************************/
#ifndef __HEADER_GUARD_UTILS__MEMORY_H__
#define __HEADER_GUARD_UTILS__MEMORY_H__

#include <utils/Macros.h>
#include <atomic>
#include <cstddef>
#ifdef UTILS_USE_BOOST_SMART_POINTERS

namespace MainSpace = boost;
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/unique_ptr.hpp>

#elif defined UTILS_CXX0X

namespace MainSpace = std;
#include <memory>

#else

#error "No smart pointer policy choosen."

#endif

namespace utils {
class shared_ptr_void;

class weak_ptr_void {
public:
	weak_ptr_void();
	weak_ptr_void(const weak_ptr_void& r);
	weak_ptr_void(const shared_ptr_void& r);
	weak_ptr_void(weak_ptr_void&& r);

	~weak_ptr_void();

	weak_ptr_void& operator=(const weak_ptr_void& r);
	weak_ptr_void& operator=(const shared_ptr_void& r);

	void reset();

	void swap(weak_ptr_void& r);

	long use_count() const;

	bool expired() const;

	shared_ptr_void lock() const;

	template<class Y>
	bool owner_before(const MainSpace::weak_ptr<Y>& other) const {
		return impl.owner_before(other);
	}

	template<class Y>
	bool owner_before(const MainSpace::shared_ptr<Y>& other) const {
		return impl.owner_before(other);
	}

private:

	MainSpace::weak_ptr<char> impl;
};

template<class Deleter>
Deleter* get_deleter(const shared_ptr_void& p);

template<typename U>
bool operator==(const shared_ptr_void& lhs,
		const MainSpace::shared_ptr<U>& rhs);
template<typename U>
bool operator!=(const shared_ptr_void& lhs,
		const MainSpace::shared_ptr<U>& rhs);
template<typename U>
bool operator<(const shared_ptr_void& lhs, const MainSpace::shared_ptr<U>& rhs);
template<typename U>
bool operator>(const shared_ptr_void& lhs, const MainSpace::shared_ptr<U>& rhs);
template<typename U>
bool operator<=(const shared_ptr_void& lhs,
		const MainSpace::shared_ptr<U>& rhs);
template<typename U>
bool operator>=(const shared_ptr_void& lhs,
		const MainSpace::shared_ptr<U>& rhs);
template<typename U>
bool operator==(const MainSpace::shared_ptr<U>& lhs,
		const shared_ptr_void& rhs);
template<typename U>
bool operator!=(const MainSpace::shared_ptr<U>& lhs,
		const shared_ptr_void& rhs);
template<typename U>
bool operator<(const MainSpace::shared_ptr<U>& lhs, const shared_ptr_void& rhs);
template<typename U>
bool operator>(const MainSpace::shared_ptr<U>& lhs, const shared_ptr_void& rhs);
template<typename U>
bool operator<=(const MainSpace::shared_ptr<U>& lhs,
		const shared_ptr_void& rhs);
template<typename U>
bool operator>=(const MainSpace::shared_ptr<U>& lhs,
		const shared_ptr_void& rhs);
bool operator==(const shared_ptr_void& lhs, std::nullptr_t rhs);
bool operator==(std::nullptr_t lhs, const shared_ptr_void& rhs);
bool operator!=(const shared_ptr_void& lhs, std::nullptr_t rhs);
bool operator!=(std::nullptr_t lhs, const shared_ptr_void& rhs);
bool operator<(const shared_ptr_void& lhs, std::nullptr_t rhs);
bool operator<(std::nullptr_t lhs, const shared_ptr_void& rhs);
bool operator>(const shared_ptr_void& lhs, std::nullptr_t rhs);
bool operator>(std::nullptr_t lhs, const shared_ptr_void& rhs);
bool operator<=(const shared_ptr_void& lhs, std::nullptr_t rhs);
bool operator<=(std::nullptr_t lhs, const shared_ptr_void& rhs);
bool operator>=(const shared_ptr_void& lhs, std::nullptr_t rhs);
bool operator>=(std::nullptr_t lhs, const shared_ptr_void& rhs);
template<class U, class V>
std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os,
		const shared_ptr_void& ptr);

bool atomic_is_lock_free(const shared_ptr_void* p);
shared_ptr_void atomic_load(const shared_ptr_void* p);
shared_ptr_void atomic_load_explicit(const shared_ptr_void* p,
		std::memory_order mo);
void atomic_store(shared_ptr_void* p, shared_ptr_void r);
void atomic_store_explicit(shared_ptr_void* p, shared_ptr_void r,
		std::memory_order mo);
shared_ptr_void atomic_exchange(shared_ptr_void* p, shared_ptr_void r);
shared_ptr_void atomic_exchange_explicit(shared_ptr_void* p, shared_ptr_void r,
		std::memory_order mo);
bool atomic_compare_exchange_weak(shared_ptr_void* p, shared_ptr_void* expected,
		shared_ptr_void desired);
bool atomic_compare_exchange_strong(shared_ptr_void* p,
		shared_ptr_void* expected, shared_ptr_void desired);
bool atomic_compare_exchange_strong_explicit(shared_ptr_void* p,
		shared_ptr_void* expected, shared_ptr_void desired,
		std::memory_order success, std::memory_order failure);
bool atomic_compare_exchange_weak_explicit(shared_ptr_void* p,
		shared_ptr_void* expected, shared_ptr_void desired,
		std::memory_order success, std::memory_order failure);

struct hash {
	std::size_t operator()(const shared_ptr_void & val) const;
};

class shared_ptr_void {	// class for reference counted resource management

	friend class weak_ptr_void;

	template<class Deleter>
	friend Deleter* get_deleter(const shared_ptr_void& p);

	friend struct hash;

	template<typename U>
	friend bool operator==(const shared_ptr_void& lhs,
			const MainSpace::shared_ptr<U>& rhs);
	template<typename U>
	friend bool operator!=(const shared_ptr_void& lhs,
			const MainSpace::shared_ptr<U>& rhs);
	template<typename U>
	friend bool operator<(const shared_ptr_void& lhs,
			const MainSpace::shared_ptr<U>& rhs);
	template<typename U>
	friend bool operator>(const shared_ptr_void& lhs,
			const MainSpace::shared_ptr<U>& rhs);
	template<typename U>
	friend bool operator<=(const shared_ptr_void& lhs,
			const MainSpace::shared_ptr<U>& rhs);
	template<typename U>
	friend bool operator>=(const shared_ptr_void& lhs,
			const MainSpace::shared_ptr<U>& rhs);
	template<typename U>
	friend bool operator==(const MainSpace::shared_ptr<U>& lhs,
			const shared_ptr_void& rhs);
	template<typename U>
	friend bool operator!=(const MainSpace::shared_ptr<U>& lhs,
			const shared_ptr_void& rhs);
	template<typename U>
	friend bool operator<(const MainSpace::shared_ptr<U>& lhs,
			const shared_ptr_void& rhs);
	template<typename U>
	friend bool operator>(const MainSpace::shared_ptr<U>& lhs,
			const shared_ptr_void& rhs);
	template<typename U>
	friend bool operator<=(const MainSpace::shared_ptr<U>& lhs,
			const shared_ptr_void& rhs);
	template<typename U>
	friend bool operator>=(const MainSpace::shared_ptr<U>& lhs,
			const shared_ptr_void& rhs);
	friend bool operator==(const shared_ptr_void& lhs, std::nullptr_t rhs);
	friend bool operator==(std::nullptr_t lhs, const shared_ptr_void& rhs);
	friend bool operator!=(const shared_ptr_void& lhs, std::nullptr_t rhs);
	friend bool operator!=(std::nullptr_t lhs, const shared_ptr_void& rhs);
	friend bool operator<(const shared_ptr_void& lhs, std::nullptr_t rhs);
	friend bool operator<(std::nullptr_t lhs, const shared_ptr_void& rhs);
	friend bool operator>(const shared_ptr_void& lhs, std::nullptr_t rhs);
	friend bool operator>(std::nullptr_t lhs, const shared_ptr_void& rhs);
	friend bool operator<=(const shared_ptr_void& lhs, std::nullptr_t rhs);
	friend bool operator<=(std::nullptr_t lhs, const shared_ptr_void& rhs);
	friend bool operator>=(const shared_ptr_void& lhs, std::nullptr_t rhs);
	friend bool operator>=(std::nullptr_t lhs, const shared_ptr_void& rhs);
	template<class U, class V>
	friend std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os,
			const shared_ptr_void& ptr);

	friend bool atomic_is_lock_free(const shared_ptr_void* p);
	friend shared_ptr_void atomic_load(const shared_ptr_void* p);
	friend shared_ptr_void atomic_load_explicit(const shared_ptr_void* p,
			std::memory_order mo);
	friend void atomic_store(shared_ptr_void* p, shared_ptr_void r);
	friend void atomic_store_explicit(shared_ptr_void* p, shared_ptr_void r,
			std::memory_order mo);
	friend shared_ptr_void atomic_exchange(shared_ptr_void* p,
			shared_ptr_void r);
	friend shared_ptr_void atomic_exchange_explicit(shared_ptr_void* p,
			shared_ptr_void r, std::memory_order mo);
	friend bool atomic_compare_exchange_weak(shared_ptr_void* p,
			shared_ptr_void* expected, shared_ptr_void desired);
	friend bool atomic_compare_exchange_strong(shared_ptr_void* p,
			shared_ptr_void* expected, shared_ptr_void desired);
	friend bool atomic_compare_exchange_strong_explicit(shared_ptr_void* p,
			shared_ptr_void* expected, shared_ptr_void desired,
			std::memory_order success, std::memory_order failure);
	friend bool atomic_compare_exchange_weak_explicit(shared_ptr_void* p,
			shared_ptr_void* expected, shared_ptr_void desired,
			std::memory_order success, std::memory_order failure);

public:
	using element_type = void;
	template<typename T>
	class InnerDeleter {
	public:

		InnerDeleter(const T & deleter) :
				deleter_(deleter) {
		}
		~InnerDeleter() {
		}

		void operator()(char* p) {
			deleter_(static_cast<void*>(p));
		}

		T * deleter() {
			return &deleter_;
		}

	private:

		T deleter_;
	};

protected:

	shared_ptr_void(MainSpace::shared_ptr<char> other);

public:

	shared_ptr_void();

	template<class Deleter>
	shared_ptr_void(void* ptr, Deleter d) :
			impl(static_cast<char*>(ptr), InnerDeleter<Deleter>(d)) {
	}

	template<class Deleter, class Alloc>
	shared_ptr_void(void* ptr, Deleter d, Alloc alloc) :
			impl(static_cast<char*>(ptr), InnerDeleter<Deleter>(d), alloc) {
	}

	template<class Deleter>
	shared_ptr_void(std::nullptr_t, Deleter d) :
			impl(nullptr, InnerDeleter<Deleter>(d)) {
	}

	template<class Deleter, class Alloc>
	shared_ptr_void(std::nullptr_t, Deleter d, Alloc alloc) :
			impl(nullptr, InnerDeleter<Deleter>(d), alloc) {
	}

	shared_ptr_void(const shared_ptr_void& r);

	shared_ptr_void(shared_ptr_void&& r);

	explicit shared_ptr_void(const weak_ptr_void& r);

	template<class Deleter>
	shared_ptr_void(MainSpace::unique_ptr<void, Deleter>&& r) :
			impl(static_cast<char*>(r.release()),
					InnerDeleter<decltype(r.get_deleter())>(r.get_deleter())) {
	}

	~shared_ptr_void();

	shared_ptr_void& operator=(const shared_ptr_void& r);

	shared_ptr_void& operator=(shared_ptr_void&& r);

	template<class Deleter>
	shared_ptr_void& operator=(MainSpace::unique_ptr<void, Deleter>&& r) {
		*this = shared_ptr_void(r.release(), r.get_deleter());
		return *this;
	}

	void reset();
	//void reset(void* ptr) { impl.reset(ptr, *get_deleter(impl)); }
	template<class Deleter>
	void reset(void* ptr, Deleter d) {
		impl.reset(static_cast<char*>(ptr), InnerDeleter<Deleter>(d));
	}
	template<class Deleter, class Alloc>
	void reset(void* ptr, Deleter d, Alloc alloc) {
		impl.reset(static_cast<char*>(ptr), InnerDeleter<Deleter>(d), alloc);
	}

	void swap(shared_ptr_void& r);

	void* get() const;

	void* operator->() const;

	long use_count() const;

	bool unique() const;

	explicit operator bool() const;

	template<class Y>
	bool owner_before(const MainSpace::shared_ptr<Y>& other) const {
		return impl.owner_before(other);
	}

	template<class Y>
	bool owner_before(const MainSpace::weak_ptr<Y>& other) const {
		return impl.owner_before(other);
	}

private:

	MainSpace::shared_ptr<char> impl;
};

template<class Deleter>
Deleter* get_deleter(const shared_ptr_void& p) {
	Deleter * ret = nullptr;

	auto deleter = get_deleter<shared_ptr_void::InnerDeleter<Deleter>>(p.impl);
	if (deleter != nullptr) {
		ret = deleter->deleter();
	}

	return ret;
}

template<typename U>
bool operator==(const shared_ptr_void& lhs, const std::shared_ptr<U>& rhs) {
	return lhs.impl == rhs;
}

template<typename U>
bool operator!=(const shared_ptr_void& lhs, const std::shared_ptr<U>& rhs) {
	return lhs.impl != rhs;
}

template<typename U>
bool operator<(const shared_ptr_void& lhs, const std::shared_ptr<U>& rhs) {
	return lhs.impl < rhs;
}

template<typename U>
bool operator>(const shared_ptr_void& lhs, const std::shared_ptr<U>& rhs) {
	return lhs.impl > rhs;
}

template<typename U>
bool operator<=(const shared_ptr_void& lhs, const std::shared_ptr<U>& rhs) {
	return lhs.impl <= rhs;
}

template<typename U>
bool operator>=(const shared_ptr_void& lhs, const std::shared_ptr<U>& rhs) {
	return lhs.impl >= rhs;
}

template<typename U>
bool operator==(const std::shared_ptr<U>& lhs, const shared_ptr_void& rhs) {
	return rhs == lhs;
}

template<typename U>
bool operator!=(const std::shared_ptr<U>& lhs, const shared_ptr_void& rhs) {
	return rhs != lhs;
}

template<typename U>
bool operator<(const std::shared_ptr<U>& lhs, const shared_ptr_void& rhs) {
	return rhs < lhs;
}

template<typename U>
bool operator>(const std::shared_ptr<U>& lhs, const shared_ptr_void& rhs) {
	return rhs > lhs;
}

template<typename U>
bool operator<=(const std::shared_ptr<U>& lhs, const shared_ptr_void& rhs) {
	return rhs <= lhs;
}

template<typename U>
bool operator>=(const std::shared_ptr<U>& lhs, const shared_ptr_void& rhs) {
	return rhs >= lhs;
}

template<class U, class V>
std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os,
		const shared_ptr_void& ptr) {
	os << ptr.impl;
	return os;
}

void swap(shared_ptr_void& lhs, shared_ptr_void& rhs);
}

#ifdef UTILS_USE_BOOST_SMART_POINTERS

namespace boost

#elif defined UTILS_CXX0X

namespace std

#endif
{
	template<>
	class weak_ptr<void> : public utils::weak_ptr_void
	{
	public:

		template <typename... Args>
		weak_ptr(Args&&... args) : utils::weak_ptr_void(std::forward<Args>(args)...)
		{

		}
	};

	template<>
	class shared_ptr<void> : public utils::shared_ptr_void
	{
	public:

		template <typename... Args>
		shared_ptr(Args&&... args) : utils::shared_ptr_void(std::forward<Args>(args)...)
		{

		}
	};
}

#endif	// __HEADER_GUARD_UTILS__MEMORY_H__
