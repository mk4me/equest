/********************************************************************
	created:  2014/10/26	16:22:44
	filename: ThreadPool.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORELIB__THREADPOOL_H__
#define __HEADER_GUARD_CORELIB__THREADPOOL_H__

#include <corelib/IThreadPool.h>
#include <loglib/ILog.h>

namespace core
{
	class ThreadPool : public IThreadPool
	{
	public:
		//! Domyślny konstruktor
		ThreadPool(InnerThreadPool * tp);
		//! Desturktor
		~ThreadPool();

		//! \return Maksymalna ilość wątków jakie można utworzyć
		const size_type maxThreads() const override;
		//! \return Minimalna ilość wątków utrzymywana przez manager
		const size_type minThreads() const override;
		//! \return Ilość aktualnie zajętych wątków
		const size_type threadsCount() const override;
		//! \return Nowy wątek		
		Thread get(const std::string & who, const std::string & destination) override;

		//! \param groupSize Ilość wątków w grupie
		//! \param threads [out] Lista z nowymi wątkami, dopisujemy zawsze na końcu
		//! \return Ilość faktycznie dostarczonych wątków
		const size_type get(const size_type groupSize, Threads & threads, const bool exact,
			const std::string & who, const std::string & destination = std::string()) override;

		//! \param groupSize Ilość wątków jakie tworzymy poza ThreadPoolem
		virtual InnerThreadPool::CustomThreadProxy getCustom(const size_type groupSize = 1) override;

	private:
		
		Thread create(InnerThreadPool::Thread && innerThread,
			const std::string & who, const std::string & destination);

	private:
		//! Wewnętrzny tp
		InnerThreadPool * tp;	
		loglib::ILogPtr logger;
	};	
}

#endif	// __HEADER_GUARD_CORELIB__THREADPOOL_H__
