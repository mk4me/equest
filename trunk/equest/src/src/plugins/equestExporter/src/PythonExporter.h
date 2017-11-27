/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:03
	filename: 	PythonExporter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST_EXPORTER__PYTHONEXPORTER_H__
#define HEADER_GUARD_EQUEST_EXPORTER__PYTHONEXPORTER_H__

#include "IExporter.h"

namespace equestExporter {
	class PythonExporter : public IExporter
	{
	public:
		virtual ~PythonExporter() {}

		virtual void exportAnnotations(const utils::Filesystem::Path& path, const AnnotationData& data) const;

	};
	DEFINE_SMART_POINTERS(PythonExporter);
}

#endif
