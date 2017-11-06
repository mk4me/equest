/********************************************************************
	created:  2014/10/02	10:12:09
	filename: ThreadGuard.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__THREADGUARD_H__
#define __HEADER_GUARD_THREADINGUTILS__THREADGUARD_H__

namespace threadingUtils
{
	//! Klasa narzêdziowa pozwalaj¹ca oczekiwaæ na zakoñczenie dzia³ania w¹tku
	//! (RAII)
	//! \tparam ThreadType Typ w¹tku
	template<class ThreadType>
	class ThreadGuard
	{
	public:
		//! \param t_ W¹tek na który iczekujemy
		explicit ThreadGuard(ThreadType& t_) t(t_)
		{

		}

		//! Destruktor
		~ThreadGuard()
		{
			if (t.joinable())
			{
				t.join();
			}
		}

		ThreadGuard(ThreadGuard const&) = delete;
		ThreadGuard& operator=(ThreadGuard const&) = delete;

	private:
		//! Referencja w¹tku na który oczekujemy
		ThreadType& t;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__THREADGUARD_H__