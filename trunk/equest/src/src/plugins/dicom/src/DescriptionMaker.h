/********************************************************************
	created:	2015/08/25
	created:	13:13
	filename: 	DescriptionMaker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DescriptionMaker_H__
#define HEADER_GUARD_DICOM__DescriptionMaker_H__

#include <plugins/dicom/IDicomInternalStruct.h>

namespace  dicom {
	class DescriptionMaker {
		public:
			virtual ~DescriptionMaker() {}

		public:
			static QString createDesc(const internalData::Patient& patient);
			static QString createDesc(const internalData::Study& study);
			static QString createDesc(const internalData::Serie& serie);
			static QString createDesc(const internalData::Image& image);
	};
	DEFINE_SMART_POINTERS(DescriptionMaker);
}

#endif // HEADER_GUARD 
