#include "ModelConfigurationWizardPage.h"
#include "ui_ModelConfigurationWizardPage.h"
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <utils/Filesystem.h>
#include <kinematiclib/LinearizedSkeleton.h>
#include <acclaimformatslib/AsfParser.h>
#include <algorithm>
#include <vector>
#include <fstream>

Q_DECLARE_METATYPE(IMU::SkeletonConstPtr);

ModelConfigurationWizardPage::ModelConfigurationWizardPage(const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	QWidget *parent)
	: QWizardPage(parent), ui(new Ui::ModelConfigurationWizardPage)
{
	ui->setupUi(this);

	connect(ui->modelComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));

	ui->modelComboBox->addItem(tr("Select model..."));

	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->modelComboBox->model());
	QStandardItem* item = model->item(0);

	item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
	// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
	item->setData(ui->modelComboBox->palette().color(QPalette::Disabled, QPalette::Text));

	if (skeletonModels.empty() == false){
		ui->modelComboBox->insertSeparator(1);
	}

	for (auto m : skeletonModels)
	{
		unsigned int jointsCount = m.second->root()->size();
		unsigned int activeJointsCount = kinematic::LinearizedSkeleton::createNonLeafOrder(*m.second).size();
		ui->modelComboBox->addItem(tr("Model %1: %2 joints (%3 active)").arg(QString::fromStdString(m.second->name)).arg(jointsCount).arg(activeJointsCount), QVariant::fromValue(m.second));
	}	
}

ModelConfigurationWizardPage::ModelConfigurationWizardPage(const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	const core::UniqueID & currentID, QWidget *parent) : ModelConfigurationWizardPage(skeletonModels, parent)
{
	const auto size = ui->modelComboBox->count();

	for (std::remove_const<decltype(size)>::type i = 1; i < size; ++i)
	{
		if (ui->modelComboBox->itemData(i).canConvert<IMU::SkeletonConstPtr>() == true){
			const auto s = ui->modelComboBox->itemData(i).value<IMU::SkeletonConstPtr>();
			if (s->ID() == currentID){
				ui->modelComboBox->setCurrentIndex(i);
				break;
			}
		}
	}
}

ModelConfigurationWizardPage::~ModelConfigurationWizardPage()
{

}

bool ModelConfigurationWizardPage::isComplete() const
{
	return (ui->modelComboBox->currentIndex() > 0);
}

void ModelConfigurationWizardPage::onLoadModel()
{
	auto file = QFileDialog::getOpenFileName(this, tr("Open model file"), QString(), tr("Acclaim skeleton file file (*.asf)"));

	utils::Filesystem::Path p(file.toStdString());
	if (utils::Filesystem::isRegularFile(p)) {
		try{
			std::ifstream file(p.string());
			auto as = acclaim::AsfParser::parse(file, true);
			kinematic::Skeleton skeleton;
			kinematic::Skeleton::convert(as, skeleton);

			boost::uuids::basic_random_generator<boost::mt19937> gen;
			boost::uuids::uuid u = gen();

			auto registeredSkeleton = utils::make_shared<const IMU::Skeleton>(u, as.name, skeleton);

			unsigned int jointsCount = registeredSkeleton->root()->size();
			unsigned int activeJointsCount = kinematic::LinearizedSkeleton::createNonLeafOrder(*registeredSkeleton).size();
			ui->modelComboBox->addItem(tr("Model %1: %2 joints (%3 active").arg(QString::fromStdString(registeredSkeleton->name)).arg(jointsCount).arg(activeJointsCount), QVariant::fromValue(registeredSkeleton));
		}
		catch (std::exception & e){
			QMessageBox::critical(this, tr("Loading model failure"), tr("Failed to load model file: %1 with following error: ").arg(file) + QString::fromStdString(e.what()));
		}
		catch (...){
			QMessageBox::critical(this, tr("Loading model failure"), tr("Failed to load model file: %1 with unknown error"));
		}
	}
}

IMU::SkeletonConstPtr ModelConfigurationWizardPage::skeleton() const
{
	if (ui->modelComboBox->currentData().canConvert<IMU::SkeletonConstPtr>() == true){
		return ui->modelComboBox->currentData().value<IMU::SkeletonConstPtr>();
	}

	return IMU::SkeletonConstPtr();
}