/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:04
	filename: 	IExporter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__IEXPORTER_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__IEXPORTER_H__

namespace medusaExporter {

	class AnnotationData;

	struct RecreateAnnotationsConfig
	{
		bool recreate = false;
		bool merge = false;
		bool drawJoints = true;
		bool drawBones = true;
		bool drawSkins = true;
		bool drawInflammations = true;
	};

	struct ExportConfig
	{
		bool skipIdentical;
		bool pixelByPixel;
		int pointsDensity;
        bool normalizePointVectorsLenght;
		RecreateAnnotationsConfig recreateAnnotations;
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
