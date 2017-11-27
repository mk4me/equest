/********************************************************************
	created:  2014/04/01
	created:  1:4:2014   8:05
	filename: IEquestService.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_EQUEST__IEquestService_H__
#define HEADER_GUARD_EQUEST__IEquestService_H__

#include <string>
#include <corelib/IService.h>

namespace equest
{
	class IEquestService : public plugin::IService
	{
	public:
		virtual ~IEquestService() {}

		virtual void updateItemIcon(const std::string& filename, const QIcon& icon) = 0;
	};
}

#endif	//	HEADER_GUARD_EQUEST__IEquestService_H__
