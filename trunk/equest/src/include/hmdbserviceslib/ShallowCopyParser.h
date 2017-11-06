/********************************************************************
	created:  2012/02/29
	created:  29:2:2012   8:54
	filename: ShallowCopyParser.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_SHALLOWCOPYPARSER_H__
#define HEADER_GUARD__HMDBSERVICES_SHALLOWCOPYPARSER_H__

#include <hmdbserviceslib/Export.h>
#include <hmdbserviceslib/ShallowCopy.h>

namespace hmdbServices
{
	//! Parser płytkiej kopi danych ruchu
	class HMDBSERVICES_EXPORT MotionShallowCopyParser
	{
	public:
		//! \param document Strumień z dokumentem do parsowania
		//! \param shallowCopy [out] Struktura płytkiej kopii danych ruchu do wypełnienia
		//! \return Czy dokument poprawnie sparsowany
		static bool parseFile(std::istream & document, MotionShallowCopy::ShallowCopy & shallowCopy);
	};

	//! Parser płytkiej kopi danych medycznych
	class HMDBSERVICES_EXPORT MedicalShallowCopyParser
	{
	public:
		//! \param document Strumień z dokumentem do parsowania
		//! \param shallowCopy [out] Struktura płytkiej kopii danych medycznych do wypełnienia
		//! \return Czy dokument poprawnie sparsowany
		static bool parseFile(std::istream & document, MedicalShallowCopy::ShallowCopy & shallowCopy);
	};
}

#endif	//	HEADER_GUARD__HMDBSERVICES_SHALLOWCOPYPARSER_H__
