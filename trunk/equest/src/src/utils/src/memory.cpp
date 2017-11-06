#include <utils/memory.h>
#include <atomic>
namespace utils
{
	weak_ptr_void::weak_ptr_void()
	{

	}
	weak_ptr_void::weak_ptr_void(const weak_ptr_void& r) : impl(r.impl)
	{

	}

	weak_ptr_void::weak_ptr_void(const shared_ptr_void& r) : impl(r.impl)
	{

	}

	weak_ptr_void::weak_ptr_void(weak_ptr_void&& r) : impl(std::move(r.impl))
	{

	}

	weak_ptr_void::~weak_ptr_void()
	{

	}

	weak_ptr_void& weak_ptr_void::operator=(const weak_ptr_void& r)
	{
		impl = r.impl;
		return *this;
	}

	weak_ptr_void& weak_ptr_void::operator=(const shared_ptr_void& r)
	{
		impl = r.impl;
		return *this;
	}

	void weak_ptr_void::reset()
	{
		impl.reset();
	}

	void weak_ptr_void::swap(weak_ptr_void& r)
	{
		impl.swap(r.impl);
	}

	long weak_ptr_void::use_count() const
	{
		return impl.use_count();
	}

	bool weak_ptr_void::expired() const
	{
		return impl.expired();
	}

	shared_ptr_void weak_ptr_void::lock() const
	{
		return shared_ptr_void(impl.lock());
	}

	std::size_t hash::operator()(const shared_ptr_void & val) const
	{
		return std::hash<std::shared_ptr<char>>()(val.impl);
	}

	shared_ptr_void::shared_ptr_void(std::shared_ptr<char> other) : impl(other)
	{

	}

	shared_ptr_void::shared_ptr_void()
	{

	}

	shared_ptr_void::shared_ptr_void(const shared_ptr_void& r) : impl(r.impl)
	{

	}

	shared_ptr_void::shared_ptr_void(shared_ptr_void&& r) : impl(std::move(r.impl))
	{

	}

	shared_ptr_void::shared_ptr_void(const weak_ptr_void& r) : shared_ptr_void(r.lock())
	{

	}

	shared_ptr_void::~shared_ptr_void()
	{

	}

	shared_ptr_void& shared_ptr_void::operator=(const shared_ptr_void& r)
	{
		impl = r.impl;
		return *this;
	}

	shared_ptr_void& shared_ptr_void::operator=(shared_ptr_void&& r)
	{
		impl = std::move(r.impl);
		return *this;
	}

	void shared_ptr_void::reset()
	{
		impl.reset();
	}

	void shared_ptr_void::swap(shared_ptr_void& r)
	{
		impl.swap(r.impl);
	}

	void* shared_ptr_void::get() const
	{
		return impl.get();
	}

	void* shared_ptr_void::operator->() const
	{
		return impl.operator->();
	}

	long shared_ptr_void::use_count() const
	{
		return impl.use_count();
	}

	bool shared_ptr_void::unique() const
	{
		return impl.unique();
	}

	shared_ptr_void::operator bool() const
	{
		return impl.operator bool();
	}

	bool operator==(const shared_ptr_void& lhs, std::nullptr_t rhs)
	{
		return lhs.impl == rhs;
	}

	bool operator==(std::nullptr_t lhs, const shared_ptr_void& rhs)
	{
		return rhs == lhs;
	}

	bool operator!=(const shared_ptr_void& lhs, std::nullptr_t rhs)
	{
		return lhs.impl != rhs;
	}

	bool operator!=(std::nullptr_t lhs, const shared_ptr_void& rhs)
	{
		return rhs != lhs;
	}

	bool operator<(const shared_ptr_void& lhs, std::nullptr_t rhs)
	{
		return lhs.impl < rhs;
	}

	bool operator<(std::nullptr_t lhs, const shared_ptr_void& rhs)
	{
		return lhs < lhs;
	}

	bool operator>(const shared_ptr_void& lhs, std::nullptr_t rhs)
	{
		return lhs.impl > rhs;
	}

	bool operator>(std::nullptr_t lhs, const shared_ptr_void& rhs)
	{
		return rhs > lhs;
	}

	bool operator<=(const shared_ptr_void& lhs, std::nullptr_t rhs)
	{
		return lhs.impl <= rhs;
	}

	bool operator<=(std::nullptr_t lhs, const shared_ptr_void& rhs)
	{
		return rhs <= lhs;
	}

	bool operator>=(const shared_ptr_void& lhs, std::nullptr_t rhs)
	{
		return lhs.impl >= rhs;
	}

	bool operator>=(std::nullptr_t lhs, const shared_ptr_void& rhs)
	{
		return rhs >= lhs;
	}

	/*bool atomic_is_lock_free(const shared_ptr_void* p)
	{
		if (p == nullptr){
			return false;
		}

		return std::atomic_is_lock_free(&(p->impl));
	}

	shared_ptr_void atomic_load(const shared_ptr_void* p)
	{
		if (p == nullptr){
			return shared_ptr_void();
		}

		return std::atomic_load(&(p->impl));
	}

	shared_ptr_void atomic_load_explicit(const shared_ptr_void* p,
		std::memory_order mo)
	{
		if (p == nullptr){
			return shared_ptr_void();
		}

		return std::atomic_load_explicit(&(p->impl), mo);
	}

	void atomic_store(shared_ptr_void* p,
		shared_ptr_void r)
	{
		if (p != nullptr){
			std::atomic_store(&(p->impl), r.impl);
		}
	}

	void atomic_store_explicit(shared_ptr_void* p,
		shared_ptr_void r,
		std::memory_order mo)
	{
		if (p != nullptr){
			std::atomic_store_explicit(&(p->impl), r.impl, mo);
		}
	}

	shared_ptr_void atomic_exchange(shared_ptr_void* p,
		shared_ptr_void r)
	{
		if (p == nullptr){
			return shared_ptr_void();
		}

		return std::atomic_exchange(&(p->impl), r.impl);
	}

	shared_ptr_void atomic_exchange_explicit(shared_ptr_void* p,
		shared_ptr_void r,
		std::memory_order mo)
	{
		if (p == nullptr){
			return shared_ptr_void();
		}

		return std::atomic_exchange_explicit(&(p->impl), r.impl, mo);
	}

	bool atomic_compare_exchange_weak(shared_ptr_void* p,
		shared_ptr_void* expected,
		shared_ptr_void desired)
	{
		if (p == nullptr || expected == nullptr){
			return false;
		}

		return std::atomic_compare_exchange_weak(&(p->impl), &(expected->impl), desired.impl);
	}

	bool atomic_compare_exchange_strong(shared_ptr_void* p,
		shared_ptr_void* expected,
		shared_ptr_void desired)
	{
		if (p == nullptr || expected == nullptr){
			return false;
		}

		return std::atomic_compare_exchange_strong(&(p->impl), &(expected->impl), desired.impl);
	}

	bool atomic_compare_exchange_strong_explicit(shared_ptr_void* p,
		shared_ptr_void* expected,
		shared_ptr_void desired,
		std::memory_order success,
		std::memory_order failure)
	{
		if (p == nullptr || expected == nullptr){
			return false;
		}

		return std::atomic_compare_exchange_strong_explicit(&(p->impl), &(expected->impl), desired.impl, success, failure);
	}

	bool atomic_compare_exchange_weak_explicit(shared_ptr_void* p,
		shared_ptr_void* expected,
		shared_ptr_void desired,
		std::memory_order success,
		std::memory_order failure)
	{
		if (p == nullptr || expected == nullptr){
			return false;
		}

		return std::atomic_compare_exchange_weak_explicit(&(p->impl), &(expected->impl), desired.impl, success, failure);
	}*/

	void swap(shared_ptr_void& lhs, shared_ptr_void& rhs)
	{
		lhs.swap(rhs);
	}
}
