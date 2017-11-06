/********************************************************************
	created:  2014/04/01
	created:  1:4:2014   8:05
	filename: IDicomService.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_DICOM__IDICOMSERVICE_H__
#define HEADER_GUARD_DICOM__IDICOMSERVICE_H__

#include <string>
#include <corelib/IService.h>

namespace dicom
{
	class IDicomService : public plugin::IService
	{
	public:
		virtual ~IDicomService() {}

		virtual void updateItemIcon(const std::string& filename, const QIcon& icon) = 0;
	};
}

#endif	//	HEADER_GUARD_DICOM__IDICOMSERVICE_H__
