#include "IMUCostumeProfileEditionWizard.h"
#include <QtWidgets/QWizardPage>
#include "ui_ModelConfigurationWizardPage.h"
#include "ui_ModelPhysicalDimensionsWizardPage.h"
#include "ui_SensorsConfigurationWizardPage.h"
#include "ui_MotionEstimationConfigurationWizardPage.h"
#include <QtGui/QMouseEvent>
#include <coreui/CoreVectorInputDelegate.h>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QDoubleSpinBox>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMenu>
#include <coreui/CoreRotationWidgetEditor.h>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include "OrientationDelegate.h"
#include <boost/lexical_cast.hpp>
#include "OrientationDelegate.h"
#include "Vector3DDelegate.h"
#include "SensorIDDelegate.h"
#include "SkeletonJointsDelegate.h"
#include "OrientationEstimationAlgorithmDelegate.h"
#include <fstream>
#include "ModelConfigurationWizardPage.h"
#include "ModelPhysicalDimensionsWizardPage.h"
#include "SensorsConfigurationWizardPage.h"
#include "MotionEstimationConfigurationWizardPage.h"

using namespace IMU;

IMUCostumeProfileEditionWizard::IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	const QString & profileName,
	QWidget * parent) : QWizard(parent)
{
	setWindowTitle(tr("New profile"));
	auto modelPage = new ModelConfigurationWizardPage(skeletonModels);
	addPage(modelPage);
	addPage(new ModelPhysicalDimensionsWizardPage());
	addPage(new SensorsConfigurationWizardPage(costumeSensors, orientAlgorithms, modelPage));
	addPage(new MotionEstimationConfigurationWizardPage(calibAlgorithms, motionAlgorithms));
}

IMUCostumeProfileEditionWizard::IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	IMU::CostumeProfileConstPtr profile,
	QWidget * parent) : QWizard(parent)
{
	if (profile == nullptr){
		setWindowTitle(tr("New profile"));
		auto modelPage = new ModelConfigurationWizardPage(skeletonModels);
		addPage(modelPage);
		addPage(new ModelPhysicalDimensionsWizardPage());
		addPage(new SensorsConfigurationWizardPage(costumeSensors, orientAlgorithms, modelPage));
		addPage(new MotionEstimationConfigurationWizardPage(calibAlgorithms, motionAlgorithms));
	}
	else{
		setWindowTitle(QString::fromStdString(profile->name));
		ModelConfigurationWizardPage * modelPage = nullptr;
		if (profile->skeleton != nullptr){
			modelPage = new ModelConfigurationWizardPage(skeletonModels, profile->skeleton->ID());
			addPage(modelPage);
		}
		else{
			modelPage = new ModelConfigurationWizardPage(skeletonModels);
			addPage(modelPage);
		}

		addPage(new ModelPhysicalDimensionsWizardPage());

		addPage(new SensorsConfigurationWizardPage(costumeSensors, orientAlgorithms, profile->sensorsDescriptions, modelPage));

		core::UniqueID cID;
		core::UniqueID mID;
		if (profile->calibrationAlgorithm != nullptr){
			cID = profile->calibrationAlgorithm->ID();
		}

		if (profile->motionEstimationAlgorithm != nullptr){
			mID = profile->motionEstimationAlgorithm->ID();
		}

		addPage(new MotionEstimationConfigurationWizardPage(calibAlgorithms, motionAlgorithms, cID, mID));
	}
}

IMUCostumeProfileEditionWizard::~IMUCostumeProfileEditionWizard()
{
	
}

CostumeProfile IMUCostumeProfileEditionWizard::costumeProfile() const
{
	CostumeProfile ret;

	ret.name = "New profile";	

	ret.skeleton.reset(new IMU::Skeleton(*dynamic_cast<ModelConfigurationWizardPage*>(page(0))->skeleton()));
	ret.sensorsDescriptions = dynamic_cast<SensorsConfigurationWizardPage*>(page(2))->sensorsDescriptions();
	ret.calibrationAlgorithm.reset(dynamic_cast<MotionEstimationConfigurationWizardPage*>(page(3))->calibrationAlgorithm()->create());
	ret.motionEstimationAlgorithm.reset(dynamic_cast<MotionEstimationConfigurationWizardPage*>(page(3))->motionEstimationAlgorithm()->create());

	IMU::IMUCostumeCalibrationAlgorithm::SensorsDescriptions sd;

	for (const auto & s : ret.sensorsDescriptions)
	{
		IMU::IMUCostumeCalibrationAlgorithm::SensorDescription lsd;

		lsd.adjustment = s.second.adjustment;
		lsd.jointIdx = s.second.jointIdx;
		lsd.jointName = s.second.jointName;

		sd.insert(sd.end(),{ s.first, lsd });
	}

	ret.activeJoints = ret.motionEstimationAlgorithm->activeJoints(ret.skeleton, sd);

	return ret;
}


