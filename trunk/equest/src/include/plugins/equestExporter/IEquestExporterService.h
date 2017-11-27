/********************************************************************
	created:	2017/11/07
	filename: 	IEquestExporterService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_equestExporter__IEQUESTEXPORTERSERVICE_H__
#define HEADER_GUARD_equestExporter__IEQUESTEXPORTERSERVICE_H__

#include <corelib/IService.h>

namespace equestExporter {


	class IEquestExporterService : public plugin::IService
	{
	public:
		virtual ~IEquestExporterService() {}
		virtual QWidget* getExporterDialog() = 0;
	};
	DEFINE_SMART_POINTERS(IEquestExporterService);
}


#endif
