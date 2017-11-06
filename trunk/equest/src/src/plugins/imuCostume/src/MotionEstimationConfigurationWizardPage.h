/********************************************************************
	created:  2015/09/17	16:56:34
	filename: MotionEstimationConfigurationWizardPage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__MOTIONESTIMATIONCONFIGURATIONWIZARDPAGE_H__
#define __HEADER_GUARD_IMU__MOTIONESTIMATIONCONFIGURATIONWIZARDPAGE_H__

#include <utils/SmartPtr.h>
#include <QtWidgets/QWizardPage>
#include <plugins/imuCostume/IIMUDataSource.h>

namespace Ui
{
	class MotionEstimationConfigurationWizardPage;
}

class MotionEstimationConfigurationWizardPage : public QWizardPage
{
	Q_OBJECT

public:
	MotionEstimationConfigurationWizardPage(const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
		const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms, QWidget *parent = 0);
	MotionEstimationConfigurationWizardPage(const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
		const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
		const core::UniqueID & currentCalibID, const core::UniqueID & currentMotionID, QWidget *parent = 0);

	virtual ~MotionEstimationConfigurationWizardPage();

	virtual bool isComplete() const override;

	IMU::IMUCostumeCalibrationAlgorithmConstPtr calibrationAlgorithm() const;
	IMU::IMUCostumeMotionEstimationAlgorithmConstPtr motionEstimationAlgorithm() const;

private:

	utils::unique_ptr<Ui::MotionEstimationConfigurationWizardPage> ui;
};

#endif	// __HEADER_GUARD_IMU__MOTIONESTIMATIONCONFIGURATIONWIZARDPAGE_H__