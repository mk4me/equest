/********************************************************************
	created:  2015/09/17	17:06:27
	filename: SensorsConfigurationWizardPage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__SENSORSCONFIGURATIONWIZARDPAGE_H__
#define __HEADER_GUARD_IMU__SENSORSCONFIGURATIONWIZARDPAGE_H__

#include <utils/SmartPtr.h>
#include <QtWidgets/QWizardPage>
#include <imucostumelib/ImuCostume.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <plugins/imuCostume/CostumeProfile.h>

class SensorIDDelegate;
class SkeletonJointsDelegate;
class Vector3DDelegate;
class OrientationDelegate;
class OrientationEstimationAlgorithmDelegate;
class ModelConfigurationWizardPage;

namespace Ui
{
	class SensorsConfigurationWizardPage;
}

class SensorsConfigurationWizardPage : public QWizardPage
{
	Q_OBJECT

public:
	SensorsConfigurationWizardPage(const imuCostume::Costume::SensorIDsSet & costumeSensors,
		const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
		ModelConfigurationWizardPage * modelPage, QWidget *parent = 0);

	SensorsConfigurationWizardPage(const imuCostume::Costume::SensorIDsSet & costumeSensors,
		const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
		const IMU::CostumeProfile::SensorsDescriptions & sensorsDescriptions,
		ModelConfigurationWizardPage * modelPage, QWidget *parent = 0);

	virtual ~SensorsConfigurationWizardPage();

	virtual void initializePage() override;

	virtual bool isComplete() const override;

	IMU::CostumeProfile::SensorsDescriptions sensorsDescriptions() const;

private:

	bool rowIsValid(int row) const;

private slots:

	void onSensorsConfigurationTableContextMenu(const QPoint & position);
	
	void onSensorConfigurationCellChange(int row, int column);

	void onSelectAllSensors();

	void onDeselectAllSensors();

	void onInversSensorsSelection();

	void onSelectCommonEstimationAlgorithm();
	
	void onOneToOneMappingChanged(int state);

private:
	ModelConfigurationWizardPage * modelPage;
	std::vector<Qt::CheckState> checkStates;
	SensorIDDelegate * sensorDelegate;
	SkeletonJointsDelegate * skeletonJointDelegate;
	Vector3DDelegate * vector3DDelegate;
	OrientationDelegate * orientationDelegate;
	OrientationEstimationAlgorithmDelegate * orientationAlgorithmDelegate;
	utils::unique_ptr<Ui::SensorsConfigurationWizardPage> ui;
	kinematic::LinearizedSkeleton::Order order;
};

#endif	// __HEADER_GUARD_IMU__SENSORSCONFIGURATIONWIZARDPAGE_H__