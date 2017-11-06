/********************************************************************
	created:  2015/02/12	12:38:39
	filename: CostumeProfile.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__COSTUMEPROFILE_H__
#define __HEADER_GUARD_IMU__COSTUMEPROFILE_H__

#include <string>
#include <map>
#include <plugins/imuCostume/Types.h>
#include <kinematiclib/Skeleton.h>
#include <imucostumelib/ImuCostume.h>
#include <corelib/IIdentifiable.h>
#include <plugins/imuCostume/IIMUOrientationEstimationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeCalibrationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeMotionEstimationAlgorithm.h>
#include <plugins/imuCostume/Export.h>

namespace IMU
{
	class IIMUDataSource;

	//! Klasa szkieletu który jest identyfikowalny
	struct IMU_EXPORT Skeleton : public kinematic::Skeleton, public plugin::IIdentifiable
	{
		//! Identyfikator szkieletu
		const core::UniqueID id;
		const std::string name;

		//! \param skeleton Kopiowany szkielet
		Skeleton(const Skeleton & skeleton);
		//! \param skeleton Przejmowany szkielet
		Skeleton(Skeleton && skeleton);
		//! \param id Identyfikator
		//! \param skeleton Hierarchia wraz z orientacjami
		Skeleton(const core::UniqueID id, const std::string & name, const kinematic::Skeleton & skeleton);
		//! \param id Identyfikator
		//! \param skeleton Hierarchia wraz z orientacjami
		Skeleton(const core::UniqueID id, const std::string & name, kinematic::Skeleton && skeleton);
		//! Destruktor wirtualny
		virtual ~Skeleton();
		//! \return Identyfokator szkieletu
		virtual const core::UniqueID ID() const override;
	};

	DEFINE_SMART_POINTERS(Skeleton);

	//! Profil kostiumu
	struct CostumeProfile
	{
		//! Struktura opisuj¹ca sensory - mapowanie, algorytmy, u³o¿enie
		struct SensorDescription : public IMUCostumeCalibrationAlgorithm::SensorDescription
		{
			//! Algorytm estymacji orientacji
			IIMUOrientationEstimationAlgorithmPtr orientationEstimationAlgorithm;
		};

		//! Mapa opisów sensorów
		typedef std::map<imuCostume::Costume::SensorID, SensorDescription> SensorsDescriptions;

		//! Nazwa profilu
		std::string name;
		//! Szkielet
		SkeletonPtr skeleton;
		//! Opis sensorów
		SensorsDescriptions sensorsDescriptions;
		//! Algorytm kalibracji
		IMUCostumeCalibrationAlgorithmPtr calibrationAlgorithm;				
		//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchiê, ograniczneia stawów, ...)
		IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm;
		//! Aktywne jointy szkieletu
		std::set<kinematic::LinearizedSkeleton::NodeIDX> activeJoints;
		//! \return G³êboka kopia profilu
		CostumeProfile * clone() const;
	};

	DEFINE_SMART_POINTERS(CostumeProfile);

	struct IMU_EXPORT SerializableCostumeProfile
	{
		//! Struktura opisuj¹ca sensory - mapowanie, algorytmy, u³o¿enie
		struct SensorDescription
		{
			//! Nazwa skojarzonego jointa
			std::string jointName;
			//! Przesuniêcie lokalne wzglêdem pozycji jointa
			osg::Vec3 offset;
			//! Rotacja o jak¹ nalezy obróciæ sensor by uzyskaæ orientacje jointa
			osg::Quat preMulRotation;
			//! Rotacja o jak¹ nalezy obróciæ sensor by uzyskaæ orientacje jointa
			osg::Quat postMulRotation;
			//! Algorytm estymacji orientacji
			core::UniqueID orientationEstimationAlgorithmID;
		};

		//! Mapa opisów sensorów
		typedef std::map<imuCostume::Costume::SensorID, SensorDescription> SensorsDescriptions;

		//! Wyliczenie opisuj¹ce status weryfikacji profilu
		enum VerificationStatus
		{
			Ok,			//! OK
			Incomplete,	//! Niekompletny
			Failed		//! Niepoprawny
		};

		//! Nazwa profilu
		std::string name;
		//! Szkielet
		core::UniqueID skeletonID;
		//! Opis sensorów
		SensorsDescriptions sensorsDescriptions;
		//! Algorytm kalibracji
		core::UniqueID calibrationAlgorithmID;
		//! Algorytm poprawki estymacji ruchu (bierze pod uwage np. hierarchiê, ograniczneia stawów, ...)
		core::UniqueID motionEstimationAlgorithmID;

		//! \param profile Profil kostiumu który przygotowujemy do serializacji
		//! \return Gotowy do serializacji profil kostiumu
		static SerializableCostumeProfile pack(const CostumeProfile & profile);
		//! \param profile Serializowany profil kostiumu do wypakowania
		//! \param ds DataSource z prototypami algorytmów i szkieletów
		//! \return Wypakowany prototyp profilu
		static CostumeProfile unpack(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds);
		//! \param profile Serializowany profil kostiumu do wypakowania
		//! \param ds DataSource z prototypami algorytmów i szkieletów
		//! \return Status weryfikacji
		static VerificationStatus verify(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds);
		//! \param stream Strumieñ w ramach którego zapisujemy profil
		//! \param sProfile Serializowany profil
		static void serialize(std::ostream & stream, const SerializableCostumeProfile & sProfile);
		//! \param stream Strumieñ w ramach którego wypakowujemy profil
		//! \return Wypakowany profil
		static SerializableCostumeProfile deserialize(std::istream & stream);
	};
}

#endif	// __HEADER_GUARD_IMU__COSTUMEPROFILE_H__