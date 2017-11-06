/********************************************************************
	created:  2015/09/17	15:14:21
	filename: ModelConfigurationWizardPage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__MODELCONFIGURATIONWIZARDPAGE_H__
#define __HEADER_GUARD_IMU__MODELCONFIGURATIONWIZARDPAGE_H__

#include <set>
#include <string>
#include <QtWidgets/QWizardPage>
#include <plugins/imuCostume/IIMUDataSource.h>

namespace Ui
{
	class ModelConfigurationWizardPage;
}

class ModelConfigurationWizardPage : public QWizardPage
{
	Q_OBJECT

public:
	ModelConfigurationWizardPage(const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
		QWidget *parent = 0);
	ModelConfigurationWizardPage(const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
		const core::UniqueID & currentID, QWidget *parent = 0);

	virtual ~ModelConfigurationWizardPage();

	virtual bool isComplete() const override;

	IMU::SkeletonConstPtr skeleton() const;

private slots:

	void onLoadModel();

private:

	utils::unique_ptr<Ui::ModelConfigurationWizardPage> ui;
};

#endif	// __HEADER_GUARD_IMU__MODELCONFIGURATIONWIZARDPAGE_H__
