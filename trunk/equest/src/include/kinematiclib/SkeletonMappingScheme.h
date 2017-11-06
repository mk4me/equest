/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:26
	filename: 	SkeletonMappingScheme.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SKELETONMAPPINGSCHEME_H__
#define HEADER_GUARD_KINEMATIC__SKELETONMAPPINGSCHEME_H__

#include <string>
#include <map>
#include <list>
#include <utils/SmartPtr.h>

namespace kinematic
{
	//! Schemat mapowania kości
	//! \version 0.9.1
	struct SkeletonMappingScheme
	{
		//! Typ pojedynczego schematu mapowania jointow
		using MappingDictionary = std::map<std::string, std::string>;
		//! statyczna metoda wczytuje schematy z pliku XML
		//! \param filename nazwa pliku do wczytania
		//! \return kolekcja schematów mapowania
		static void loadFromXML(std::istream& in, std::list<MappingDictionary>& outSchemes);
		//! \return Domyślne mapowanie
		static const MappingDictionary& defaultAcclaimToHAnimMappingScheme();
	};
}

#endif
