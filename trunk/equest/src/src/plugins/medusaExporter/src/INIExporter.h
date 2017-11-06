/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:03
	filename: 	INIExporter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__INIEXPORTER_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__INIEXPORTER_H__

namespace medusaExporter {

	class INIExporter
	{
	public:
		virtual ~INIExporter() {}
	};
	DEFINE_SMART_POINTERS(INIExporter);
}

#endif
