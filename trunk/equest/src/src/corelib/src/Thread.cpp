#include "CoreLibPCH.h"
#include <corelib/Thread.h>

using namespace core;

Thread::DescriptionLocalGuard::DescriptionLocalGuard(utils::shared_ptr<Thread::Description> td)
{
	if (Thread::tlsTD.get() == nullptr){
		Thread::tlsTD.reset(new utils::shared_ptr<Thread::Description>(td));
	}
	else{
		(*Thread::tlsTD) = td;
	}
}

Thread::DescriptionLocalGuard::~DescriptionLocalGuard()
{
	auto td = Thread::tlsTD.get();
	if (td != nullptr){
		auto unpackTD = *td;
		if (unpackTD != nullptr){
			unpackTD->owner = "Unknown owner";
			unpackTD->destination = "Unknown destination";
		}
	}
}

Thread::Thread()
{

}

Thread::Thread(Thread && Other) : innerThread(std::move(Other.innerThread)),
description(std::move(Other.description)), logger(std::move(logger))
{

}

Thread::~Thread()
{

}

Thread& Thread::operator=(Thread&& Other) NOEXCEPT
{
	innerThread = std::move(Other.innerThread);
	description = std::move(Other.description);
	logger = std::move(Other.logger);
	return *this;
}

void Thread::swap(Thread& Other) NOEXCEPT
{
	std::swap(innerThread, Other.innerThread);
	std::swap(description, Other.description);
	std::swap(logger, Other.logger);
}

const bool Thread::joinable() const NOEXCEPT
{
	return innerThread.joinable();
}

void Thread::join()
{
	innerThread.join();
}

void Thread::detach()
{
	innerThread.detach();
}

std::thread::id Thread::get_id() const NOEXCEPT
{
	return innerThread.get_id();
}

std::thread::native_handle_type Thread::native_handle()
{
	return innerThread.native_handle();
}

void Thread::interrupt()
{
	innerThread.interrupt();
}

const bool Thread::interruptible() const
{
	return innerThread.interruptible();
}

void Thread::interruptionPoint()
{
	InnerThreadPool::Thread::interruptionPoint();
}

void Thread::resetInterruption()
{
	InnerThreadPool::Thread::resetInterruption();
}

std::string Thread::owner() const
{
	return description->owner;
}

std::string Thread::destination() const
{
	return description->destination;
}

std::string Thread::currentOwner()
{
	return tlsTD.get() != nullptr ? (*tlsTD)->owner : "Unknown owner";	
}

std::string Thread::currentDestination()
{
	return tlsTD.get() != nullptr ? (*tlsTD)->destination : "Unknown destination";	
}

Thread::TLS Thread::tlsTD;