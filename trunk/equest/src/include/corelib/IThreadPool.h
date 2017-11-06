/********************************************************************
created:  2014/10/26	16:22:44
filename: IThreadPool.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__ITHREADPOOL_H__
#define __HEADER_GUARD_CORELIB__ITHREADPOOL_H__

#include <utils/SmartPtr.h>
#include <corelib/Thread.h>

namespace core
{
	class IThreadPool
	{
	public:
		//! 
		typedef InnerThreadPool::size_type size_type;		

		typedef std::list<Thread> Threads;		

	public:
		//! Desturktor
		virtual ~IThreadPool() {}

		//! \return Maksymalna ilość wątków jakie można utworzyć
		virtual const size_type maxThreads() const = 0;
		//! \return Minimalna ilość wątków utrzymywana przez manager
		virtual const size_type minThreads() const = 0;
		//! \return Ilość aktualnie zajętych wątków
		virtual const size_type threadsCount() const = 0;
		//! \return Nowy wątek		
		virtual Thread get(const std::string & who, const std::string & destination) = 0;

		//! \param groupSize Ilość wątków w grupie
		//! \param threads [out] Lista z nowymi wątkami, dopisujemy zawsze na końcu
		//! \return Ilość faktycznie dostarczonych wątków
		virtual const size_type get(const size_type groupSize, Threads & threads, const bool exact,
			const std::string & who, const std::string & destination = std::string()) = 0;

		//! \param groupSize Ilość wątków jakie tworzymy poza ThreadPoolem
		virtual InnerThreadPool::CustomThreadProxy getCustom(const size_type groupSize = 1) = 0;
	};	
}

#endif	// __HEADER_GUARD_CORELIB__THREADPOOL_H__
