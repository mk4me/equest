#include "MotionEstimationConfigurationWizardPage.h"
#include "ui_MotionEstimationConfigurationWizardPage.h"
#include <QtGui/QStandardItemModel>

Q_DECLARE_METATYPE(IMU::IMUCostumeCalibrationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeMotionEstimationAlgorithmConstPtr);

MotionEstimationConfigurationWizardPage::MotionEstimationConfigurationWizardPage(
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	QWidget *parent) : QWizardPage(parent), ui(new Ui::MotionEstimationConfigurationWizardPage)
{
	ui->setupUi(this);	
	connect(ui->calibrationAlgorithmComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));
	connect(ui->motionEstimationAglorithmComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));

	//CALIBRATION combobox
	{
		ui->calibrationAlgorithmComboBox->addItem(tr("Select algorithm..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->calibrationAlgorithmComboBox->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(ui->calibrationAlgorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));

		if (calibAlgorithms.empty() == false){
			ui->calibrationAlgorithmComboBox->insertSeparator(1);
		}

		unsigned int counter = 1;

		for (auto a : calibAlgorithms)
		{
			ui->calibrationAlgorithmComboBox->addItem(QString::fromStdString(a.second->name()), QVariant::fromValue(a.second));
			ui->calibrationAlgorithmComboBox->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first))), Qt::ToolTipRole);
		}
	}

	//MOTION ESTIMATION combobox
	{
		ui->motionEstimationAglorithmComboBox->addItem(tr("Select algorithm..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->motionEstimationAglorithmComboBox->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(ui->motionEstimationAglorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));

		if (motionAlgorithms.empty() == false){
			ui->motionEstimationAglorithmComboBox->insertSeparator(1);
		}

		unsigned int counter = 1;

		for (auto a : motionAlgorithms)
		{
			ui->motionEstimationAglorithmComboBox->addItem(QString::fromStdString(a.second->name()), QVariant::fromValue(a.second));
			ui->motionEstimationAglorithmComboBox->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first))), Qt::ToolTipRole);
		}
	}
}

MotionEstimationConfigurationWizardPage::MotionEstimationConfigurationWizardPage(const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const core::UniqueID & currentCalibID, const core::UniqueID & currentMotionID, QWidget *parent)
	: MotionEstimationConfigurationWizardPage(calibAlgorithms, motionAlgorithms, parent)
{
	if (currentCalibID != core::UniqueID())
	{
		const auto size = ui->calibrationAlgorithmComboBox->count();

		for (std::remove_const<decltype(size)>::type i = 1; i < size; ++i)
		{
			if (ui->calibrationAlgorithmComboBox->itemData(i).canConvert<IMU::IMUCostumeCalibrationAlgorithmConstPtr>() == true){
				auto s = ui->calibrationAlgorithmComboBox->itemData(i).value<IMU::IMUCostumeCalibrationAlgorithmConstPtr>();
				if (s->ID() == currentCalibID){
					ui->calibrationAlgorithmComboBox->setCurrentIndex(i);
					break;
				}
			}
		}
	}

	if (currentMotionID != core::UniqueID())
	{
		const auto size = ui->motionEstimationAglorithmComboBox->count();

		for (std::remove_const<decltype(size)>::type i = 1; i < size; ++i)
		{
			if (ui->motionEstimationAglorithmComboBox->itemData(i).canConvert<IMU::IMUCostumeMotionEstimationAlgorithmConstPtr>() == true){
				auto s = ui->motionEstimationAglorithmComboBox->itemData(i).value<IMU::IMUCostumeMotionEstimationAlgorithmConstPtr>();
				if (s->ID() == currentMotionID){
					ui->motionEstimationAglorithmComboBox->setCurrentIndex(i);
					break;
				}
			}
		}
	}
}

MotionEstimationConfigurationWizardPage::~MotionEstimationConfigurationWizardPage()
{

}

bool MotionEstimationConfigurationWizardPage::isComplete() const
{
	return (ui->calibrationAlgorithmComboBox->currentIndex() > 0) && (ui->motionEstimationAglorithmComboBox->currentIndex() > 0);
}

IMU::IMUCostumeCalibrationAlgorithmConstPtr MotionEstimationConfigurationWizardPage::calibrationAlgorithm() const
{
	if (ui->calibrationAlgorithmComboBox->currentData().canConvert<IMU::IMUCostumeCalibrationAlgorithmConstPtr>() == true){
		return ui->calibrationAlgorithmComboBox->currentData().value<IMU::IMUCostumeCalibrationAlgorithmConstPtr>();
	}

	return IMU::IMUCostumeCalibrationAlgorithmConstPtr();
}

IMU::IMUCostumeMotionEstimationAlgorithmConstPtr MotionEstimationConfigurationWizardPage::motionEstimationAlgorithm() const
{
	if (ui->motionEstimationAglorithmComboBox->currentData().canConvert<IMU::IMUCostumeMotionEstimationAlgorithmConstPtr>() == true){
		return ui->motionEstimationAglorithmComboBox->currentData().value<IMU::IMUCostumeMotionEstimationAlgorithmConstPtr>();
	}

	return IMU::IMUCostumeMotionEstimationAlgorithmConstPtr();
}