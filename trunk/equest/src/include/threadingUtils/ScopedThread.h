/********************************************************************
	created:  2014/10/02	10:08:53
	filename: ScopedThread.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__SCOPEDTHREAD_H__
#define __HEADER_GUARD_THREADINGUTILS__SCOPEDTHREAD_H__

#include <stdexcept>

namespace threadingUtils
{
	//! Klasa pomocnicza (RAII) przy czekaniu na w¹tki, przejmuje w¹tek
	//! \tparam Thread Typ w¹tku
	template<class Thread>
	class ScopedThread
	{
	public:
		//! \param t W¹tek którego pilnujemy
		explicit ScopedThread(Thread && t) : t(std::move(t))
		{
			if (t.joinable() == false){
				throw std::logic_error("Empty thread");
			}
		}

		//! Destruktor
		~ScopedThread()
		{
			t.join();
		}

		ScopedThread(const ScopedThread &) = delete;
		ScopedThread& operator=(const ScopedThread &) = delete;

	private:
		//! Obs³ugiwany w¹tek
		Thread t;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__SCOPEDTHREAD_H__