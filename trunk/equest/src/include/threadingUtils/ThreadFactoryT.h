/********************************************************************
	created:  2015/08/14	09:56:11
	filename: ThreadFactoryT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__THREADFACTORYT_H__
#define __HEADER_GUARD_THREADINGUTILS__THREADFACTORYT_H__

namespace threadingUtils
{
	template<typename Thread>
	struct ThreadFactoryT
	{
		static inline Thread create() { return Thread(); }
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__THREADFACTORYT_H__