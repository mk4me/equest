/********************************************************************
	created:  2016/07/18
	filename: SkiMetadata.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__SKIMETADATA_H__
#define __HEADER_GUARD_SKIDATA__SKIMETADATA_H__

#include <string>
#include <map>

#include <objectwrapperlib/ObjectWrapper.h>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>

namespace skidata
{
	//! Opis opóŸnieñ filmów wzglêdem danych
	using MoviesDelays = std::map<std::string, float>;

	//! Metadane zjazdu
	struct SkiMetadata
	{
		//! Nazwa
		std::string name;
		//! Opis
		std::string description;
		//! Czêstotliwoœæ danych
		float dataFrequency;
		//! Iloœæ próbek
		std::size_t samplesCount;
		//! OpóŸnienia filmów
		MoviesDelays moviesDelays;
	};
}

DEFINE_WRAPPER(skidata::SkiMetadata, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);

#endif  // __HEADER_GUARD_SKIDATA__SKIMETADATA_H__