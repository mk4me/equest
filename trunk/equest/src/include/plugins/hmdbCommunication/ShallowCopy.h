/********************************************************************
	created:  2012/03/08
	created:  8:3:2012   11:01
	filename: ShallowCopy.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPY_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPY_H__

#include <utils/SmartPtr.h>
#include <hmdbserviceslib/ShallowCopy.h>
#include <hmdbserviceslib/IncrementalBranchShallowCopy.h>

namespace hmdbCommunication
{
	//! Struktura zawierająca sparsowane płytkie kopie bazy danych medycznej i ruchu oraz ich metadane
	struct ShallowCopy
	{
		//! Płytka kopia bazy danych medycznych
		hmdbServices::MedicalShallowCopy::ShallowCopy medicalShallowCopy;
		//! Metadane medyczne
		hmdbServices::MedicalMetaData::MetaData medicalMetaData;
		//! Płytka kopia bazy danych ruchu
		hmdbServices::MotionShallowCopy::ShallowCopy motionShallowCopy;
		//! Metadane ruchu
		hmdbServices::MotionMetaData::MetaData motionMetaData;

		ShallowCopy() = default;
		ShallowCopy(const ShallowCopy & other) = delete;
		ShallowCopy(ShallowCopy && other) : medicalShallowCopy(std::move(other.medicalShallowCopy)),
			medicalMetaData(std::move(other.medicalMetaData)), motionShallowCopy(std::move(other.motionShallowCopy)),
			motionMetaData(std::move(other.motionMetaData)) {}

		ShallowCopy& operator=(const ShallowCopy & other) = delete;
		ShallowCopy& operator=(ShallowCopy && other)
		{
			medicalShallowCopy = std::move(other.medicalShallowCopy);
			medicalMetaData = std::move(other.medicalMetaData);
			motionShallowCopy = std::move(other.motionShallowCopy);
			motionMetaData = std::move(other.motionMetaData);
			return *this;
		}
	};

	DEFINE_SMART_POINTERS(ShallowCopy);

	DEFINE_SMART_POINTERS_EXT(hmdbServices::IncrementalBranchShallowCopy, IncrementalBranchShallowCopy);
}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPY_H__
