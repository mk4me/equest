/********************************************************************
	created:  2014/12/21	12:00:30
	filename: IMUCostumeMotionEstimationAlgorithm.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__
#define __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__

#include <string>
#include <map>
#include <osg/Quat>
#include <plugins/imuCostume/Types.h>
#include <plugins/imuCostume/IMUCostumeCalibrationAlgorithm.h>
#include <kinematiclib/Skeleton.h>
#include <kinematiclib/SkeletonState.h>
#include <corelib/IIdentifiable.h>

class QWidget;

namespace IMU
{
	//! Interfejs algorytmów poprawiaj¹cych ogóln¹ estymacjê pozy i pozycji szkieletu (uwzlêgniaj¹c ograniczenia, hierarchiê, ...)
	class IMUCostumeMotionEstimationAlgorithm : public plugin::IIdentifiable
	{
	public:
		//! Destruktor wirtualny
		virtual ~IMUCostumeMotionEstimationAlgorithm() {}

		//! \return Nowy algorytm kalibracji
		virtual IMUCostumeMotionEstimationAlgorithm * create() const = 0;
		
		//! Returns internal filter name
		virtual std::string name() const = 0;

		//! Resets algo internal state
		virtual void reset() = 0;

		//! \param skeleton	Kalibrowany szkielet
		//! \param sensorsDescription Opis sensorów - mapowanie + poprawki po kalibracji
		virtual void initialize(kinematic::SkeletonConstPtr skeleton,
			const IMUCostumeCalibrationAlgorithm::SensorsDescriptions & sensorsDescription) = 0;
		
		//! \param motionState Aktualny stan ruchu szkieletu
		//! \param data Kompletne rozpakowane dane z IMU
		//! \param inDeltaT Czas od poprzedniej ramki danych
		//! \return Returns Lokalne orientacje wszystkich jointów, bez end-sitów (jointów bez dzieci)
		virtual kinematic::SkeletonState::RigidCompleteStateLocal estimate(const kinematic::SkeletonState::RigidCompleteStateLocal & motionState,
			const SensorsData & data, const double inDeltaT) = 0;

		//! \return Widget konfiguruj¹cy algorytm
		virtual QWidget* configurationWidget() { return nullptr; }

		//! \return Aktywne jointy kostiumu którymi faktycznie bêdziemy ruszaæ
		virtual std::set<kinematic::LinearizedSkeleton::NodeIDX> activeJoints(kinematic::SkeletonConstPtr skeleton,
			const IMUCostumeCalibrationAlgorithm::SensorsDescriptions & sensorsDescription) const
		{
			std::set<kinematic::LinearizedSkeleton::NodeIDX> ret;

			for (const auto & sd : sensorsDescription)
			{
				ret.insert(sd.second.jointIdx);
			}

			return ret;
		}
	};

	DEFINE_SMART_POINTERS(IMUCostumeMotionEstimationAlgorithm);
}

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMEMOTIONESTIMATIONALGORITHM_H__