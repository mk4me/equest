/********************************************************************
	created:	2014/05/17
	created:	17:5:2014   18:40
	filename: 	IMedusaExporterService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_medusaExporter__IMEDUSAEXPORTERSERVICE_H__
#define HEADER_GUARD_medusaExporter__IMEDUSAEXPORTERSERVICE_H__

#include <corelib/IService.h>

namespace medusaExporter {


	class IMedusaExporterService : public plugin::IService
	{
	public:
		virtual ~IMedusaExporterService() {}
		virtual QWidget* getExporterDialog() = 0;
	};
	DEFINE_SMART_POINTERS(IMedusaExporterService);
}


#endif
