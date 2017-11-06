/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:26
	filename: MetadataParser.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_METADATAPARSER_H__
#define HEADER_GUARD__HMDBSERVICES_METADATAPARSER_H__

#include <hmdbserviceslib/Export.h>
#include <hmdbserviceslib/ShallowCopy.h>

namespace hmdbServices
{
	//! Parser metadanych ruchu
	class HMDBSERVICES_EXPORT MotionMetadataParser
	{
	public:
		//! \param document Strumień z dokumentem XML
		//! \param metadata [out] Struktura metadanych do wypełnienia
		//! \return Czy dokument poprawnie sparsowany
		static bool parseFile(std::istream & document, MotionMetaData::MetaData & metadata);
	};

	//! Parser metadanych medycznych
	class HMDBSERVICES_EXPORT MedicalMetadataParser
	{
	public:
		//! \param document Strumień z dokumentem XML
		//! \param metadata [out] Struktura metadanych do wypełnienia
		//! \return Czy dokument poprawnie sparsowany
		static bool parseFile(std::istream & document, MedicalMetaData::MetaData & metadata);
	};
}

#endif	//	HEADER_GUARD__HMDBSERVICES_METADATAPARSER_H__
