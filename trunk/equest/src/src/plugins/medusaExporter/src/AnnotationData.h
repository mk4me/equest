/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:02
	filename: 	AnnotationData.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__ANNOTATIONDATA_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__ANNOTATIONDATA_H__

#include <string>
#include <vector>
#include <boost/range.hpp>
#include <plugins/dicom/ILayer.h>

namespace medusaExporter {

	class AnnotationData
	{
	public:
		struct ImageInfo
		{
			std::string imageName;
			int trial;
			int session;
			bool isPowerDoppler;

			bool operator< (const ImageInfo& ii) const {
				return imageName < ii.imageName;
			}
		};

		typedef std::map<ImageInfo, std::vector<dicom::ILayerItemConstPtr>> Layers;

	public:
		virtual ~AnnotationData() {}

	public:
		void addAnnotation(const ImageInfo& info, dicom::ILayerItemConstPtr item);
		const Layers& getAnnotations() const;

	private:
		Layers layers;
	};
	DEFINE_SMART_POINTERS(AnnotationData);

}
#endif
