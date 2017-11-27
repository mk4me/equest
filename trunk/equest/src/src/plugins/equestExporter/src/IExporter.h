/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:04
	filename: 	IExporter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST_EXPORTER__IEXPORTER_H__
#define HEADER_GUARD_EQUEST_EXPORTER__IEXPORTER_H__

namespace equestExporter {

	class AnnotationData;


	struct ExportConfig
	{
	};

	class IExporter
	{
	public:
		virtual ~IExporter() {}
		virtual void exportAnnotations(const utils::Filesystem::Path& path, const AnnotationData& data, const ExportConfig& config) const = 0;
	};
	DEFINE_SMART_POINTERS(IExporter);
}

#endif
