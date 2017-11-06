#include <threadingUtils/InterruptFlag.h>

using namespace threadingUtils;

InterruptFlag::InterruptFlag() : threadCond(nullptr), threadCondAny(nullptr)
{

}

InterruptFlag::~InterruptFlag()
{

}

void InterruptFlag::set()
{
	flag.store(true, std::memory_order_relaxed);
	std::lock_guard<std::mutex> lk(setClearMutex);
	if (threadCond)
	{
		threadCond->notify_all();
	}
	else if (threadCondAny)
	{
		threadCondAny->notify_all();
	}
}

void InterruptFlag::reset()
{
	flag.store(false, std::memory_order_relaxed);
}

bool InterruptFlag::isSet() const
{
	return flag.load(std::memory_order_relaxed);
}

bool InterruptFlag::threadIsSet()
{
	return threadInterruptFlag()->isSet();
}

void InterruptFlag::threadReset()
{
	return threadInterruptFlag()->reset();
}

InterruptFlag *& InterruptFlag::threadInterruptFlag()
{
	//! Zmienna lokalna w¹tku z flag¹
	//HACK - to powinno iœæ jako niezale¿na zmienna
	static thread_local InterruptFlag * threadInterruptFlag_ = nullptr;
	return threadInterruptFlag_;
}

void InterruptFlag::setConditionVariable(std::condition_variable & cv)
{
	std::lock_guard<std::mutex> lk(setClearMutex);
	threadCond = &cv;
}

void InterruptFlag::clearConditionVariable()
{
	std::lock_guard<std::mutex> lk(setClearMutex);
	threadCond = nullptr;
}