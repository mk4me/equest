#include "CorePCH.h"
#include "ThreadPool.h"
#include "ApplicationCommon.h"
#include <loglib/ILog.h>
#include <loglib/Exceptions.h>

using namespace core;
		
ThreadPool::ThreadPool(InnerThreadPool * tp) : tp(tp),
logger(getLogInterface()->subLog("threads"))
{
	if (tp == nullptr){
		throw std::runtime_error("Uninitialized inner thread");
	}
}

ThreadPool::~ThreadPool()
{

}

const ThreadPool::size_type ThreadPool::maxThreads() const
{
	return tp->maxThreads();
}

const ThreadPool::size_type ThreadPool::minThreads() const
{
	return tp->minThreads();
}

const ThreadPool::size_type ThreadPool::threadsCount() const
{
	return tp->threadsCount();
}

Thread ThreadPool::create(InnerThreadPool::Thread && innerThread,
	const std::string & who, const std::string & destination)
{
	return Thread([this](const std::string & message)
	{
		LOG_ERROR(logger, "Thread ID " << std::this_thread::get_id() << " owned by " << Thread::currentOwner() << " for "
			<< Thread::currentDestination() << " failed with message: "	<< message);
	}, std::move(innerThread), who, destination);
}

Thread ThreadPool::get(const std::string & who, const std::string & destination)
{		
	return create(tp->get(), who, destination);
}

const ThreadPool::size_type ThreadPool::get(const size_type groupSize,
	Threads & threads, const bool exact, const std::string & who,
	const std::string & destination)
{	
	InnerThreadPool::Threads innerThreads;
	auto ret = tp->get(groupSize, innerThreads, exact);

	auto dest(destination);

	if (dest.empty() == true){
		dest = std::string("No explicit destination");
	}

	for (auto & t : innerThreads)
	{
		threads.push_back(create(std::move(t), who, destination));
	}

	return ret;
}

InnerThreadPool::CustomThreadProxy ThreadPool::getCustom(const size_type groupSize)
{
	return tp->getCustom(groupSize);
}