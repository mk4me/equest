/********************************************************************
	created:  2014/08/14	15:24:35
	filename: ThreadAfinityVerifier.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__THREADAFINITYVERIFIER_H__
#define __HEADER_GUARD_THREADINGUTILS__THREADAFINITYVERIFIER_H__

#include <threadingUtils/Export.h>
#include <utils/SmartPtr.h>

namespace threadingUtils
{
	//! Klasa pomocnicza pozwalaj¹ca stwierdziæ czy dany obiekt stworzono w aktualnym w¹tku
	class THREADINGUTILS_EXPORT ThreadAfinityVerifier
	{
		class ThreadAfinityVerifierImpl;

	protected:
		//! Domyœlny konstruktor
		ThreadAfinityVerifier();

	public:
		//! Destruktor wirtualny
		virtual ~ThreadAfinityVerifier();
		//! \return Zwraca prawdê jeœli aktualny w¹tek to ten sam dla któego stworzono dany obiekt
		bool verifyCurrentThreadAfinity() const;

	private:
		//! Implementacja
		utils::shared_ptr<ThreadAfinityVerifierImpl> impl;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__THREADAFINITYVERIFIER_H__