
/********************************************************************
	created:	2015/08/25
	created:	12:26
	filename: 	DicomLoader.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DicomLoader_H__
#define HEADER_GUARD_DICOM__DicomLoader_H__

#include <plugins/dicom/ILayeredImage.h>
#include "DicomInternalStruct.h"

namespace  dicom {
	//! Kalsa realizuj¹ca odczyt danych USG
	class DicomLoader
	{
	public:
		//! \param from Œcie¿ka z której próbujemy czytaæ wewnêtrzn¹ reprezentacjê danych USG
		//! \return Drzewo danych USG
		static DicomInternalStructPtr load(const utils::Filesystem::Path& from);
		//! \param p Œcie¿ka z kórej próbujemy czytaæ warstwy adnotacji opusuj¹ce obraz USG
		//! \return Warstwy adnotacji opusuj¹ce obraz USG
		static LayersVectorPtr loadLayers(const utils::Filesystem::Path &p);
	};
	DEFINE_SMART_POINTERS(DicomLoader);
}

#endif // HEADER_GUARD 
