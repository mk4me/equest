#include <threadingUtils/ThreadAfinityVerifier.h>
#include <thread>

using namespace threadingUtils;

class ThreadAfinityVerifier::ThreadAfinityVerifierImpl
{
public:
	ThreadAfinityVerifierImpl() : id(std::this_thread::get_id())
	{

	}

	~ThreadAfinityVerifierImpl()
	{

	}

	bool verifyCurrentThreadAfinity() const
	{
		return id == std::this_thread::get_id();
	}

private:
	//! Identyfikator w¹tku tworz¹cego obiekt
	const std::thread::id id;
};

ThreadAfinityVerifier::ThreadAfinityVerifier() : impl(new ThreadAfinityVerifierImpl)
{

}
	
bool ThreadAfinityVerifier::verifyCurrentThreadAfinity() const
{
	return impl->verifyCurrentThreadAfinity();
}

ThreadAfinityVerifier::~ThreadAfinityVerifier()
{

}