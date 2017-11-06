/********************************************************************
	created:  2014/12/21	19:18:18
	filename: IMUCostumeConfigurationWizard.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__
#define __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__

#include <imucostumelib/CostumeRawIO.h>
#include <QtWidgets/QWizard>
#include <osg/Vec3d>
#include <osg/Quat>
#include <map>
#include <vector>
#include <plugins/imuCostume/IIMUDataSource.h>

class IMUCostumeProfileEditionWizard : public QWizard
{
	Q_OBJECT;

public:

	IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
		const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
		const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
		const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
		const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
		const QString & profileName = tr("New Profile"),
		QWidget * parent = nullptr);

	IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
		const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
		const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
		const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
		const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
		IMU::CostumeProfileConstPtr profile,
		QWidget * parent = nullptr);

	virtual ~IMUCostumeProfileEditionWizard();	

	IMU::CostumeProfile costumeProfile() const;
};

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__