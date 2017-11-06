#include "IMUCostumeProfileConfigurationWizard.h"
#include <QtWidgets/QWizard>
#include <QtWidgets/QVBoxLayout>

using namespace IMU;

struct WizzardPageDetails
{
	QString title;
	QString subTitle;
	QWidget * widget;
};

QWizard * IMUCostumeProfileConfigurationWizard::create(const CostumeProfile & profile,
	QWidget * parent)
{
	QWizard * ret = nullptr;

	std::list<WizzardPageDetails> pages;

	for (const auto & a : profile.sensorsDescriptions)
	{
		auto w = a.second.orientationEstimationAlgorithm->configurationWidget();

		if (w != nullptr){
			WizzardPageDetails pd;
			pd.title = QObject::tr("Orientation estimation algorithms configuration");
			pd.subTitle = QObject::tr("Sensor %1 -> Joint %2 : %3 algorithm").arg(a.first).arg(QString::fromStdString(a.second.jointName)).arg(QString::fromStdString(a.second.orientationEstimationAlgorithm->name()));
			pd.widget = w;
			pages.push_back(pd);
		}
	}	

	auto w = profile.calibrationAlgorithm->configurationWidget();
	if (w != nullptr){
		WizzardPageDetails pd;
		pd.title = QObject::tr("Calibration configuration");
		pd.subTitle = QObject::tr("Algorithm %1").arg(QString::fromStdString(profile.calibrationAlgorithm->name()));
		pd.widget = w;
		pages.push_back(pd);
	}

	w = profile.motionEstimationAlgorithm->configurationWidget();
	if (w != nullptr){
		WizzardPageDetails pd;
		pd.title = QObject::tr("Motion estimation configuration");
		pd.subTitle = QObject::tr("Algorithm %1").arg(QString::fromStdString(profile.motionEstimationAlgorithm->name()));
		pd.widget = w;
		pages.push_back(pd);
	}

	if (pages.empty() == false){

		ret = new QWizard(parent);

		QWizardPage * wp = nullptr;

		for (const auto & p : pages)
		{
			wp = new QWizardPage(ret);
			auto layout = new QVBoxLayout;
			layout->addWidget(p.widget);
			wp->setLayout(layout);
			wp->setTitle(p.title);
			wp->setSubTitle(p.subTitle);
			ret->addPage(wp);
		}

		wp->setFinalPage(true);		
	}

	return ret;
}