#include "MedusaExporterPCH.h"
#include "AnnotationData.h"

void medusaExporter::AnnotationData::addAnnotation(const ImageInfo& info, dicom::ILayerItemConstPtr item)
{
	layers[info].push_back(item);
}

const medusaExporter::AnnotationData::Layers& medusaExporter::AnnotationData::getAnnotations() const
{
	return layers;
}
