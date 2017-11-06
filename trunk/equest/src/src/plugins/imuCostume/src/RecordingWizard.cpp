#include "RecordingWizard.h"
#include "ui_RecordingWizard.h"
#include <QtWidgets/QFileDialog>
#include <corelib/PluginCommon.h>

Q_DECLARE_METATYPE(IMU::IIMUDataSource::CostumeID);

RecordingWizard::RecordingWizard(const CostumesToRecord & costumes,
	QWidget *parent, Qt::WindowFlags flags)
	: QWizard(parent, flags), ui(new Ui::RecordingWizard),
	baseCostumesToRecord_(costumes)
{
	if (costumes.empty() == true){
		throw std::runtime_error("No costumes to choose for recording");
	}

	ui->setupUi(this);

	for (const auto & costume : costumes)
	{
		QListWidgetItem* item = new QListWidgetItem(QString("%1:%2").arg(costume.first.ip.c_str()).arg(costume.first.port), ui->costumesToRecordListWidget);
		item->setData(Qt::UserRole, QVariant::fromValue(costume.first));
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
		item->setCheckState(Qt::Unchecked); // AND initialize check state
		ui->costumesToRecordListWidget->addItem(item);
	}	

	ui->costumesToRecordListWidget->item(0)->setCheckState(Qt::Checked);
}

RecordingWizard::~RecordingWizard()
{

}

QString RecordingWizard::outputPath() const
{
	return ui->outputLineEdit->text();
}

RecordingWizard::CostumesToRecord RecordingWizard::costumes() const
{
	return costumesToRecord_;
}

void RecordingWizard::onCostumeChange(QListWidgetItem * current, QListWidgetItem * previous)
{
	currentCostume = current->data(Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	ui->sensorsToRecordListWidget->setEnabled(current->checkState() == Qt::Checked);

	auto it = costumesToRecord_.find(currentCostume);
	if (it != costumesToRecord_.end()){

	}
	else{
		costumesToRecord_[currentCostume] = baseCostumesToRecord_[currentCostume];
		it = costumesToRecord_.find(currentCostume);
	}

	ui->sensorsToRecordListWidget->clear();

	for (const auto & id : baseCostumesToRecord_[currentCostume])
	{
		auto item = new QListWidgetItem(QString::number(id));
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(it->second.find(id) == it->second.end() ? Qt::Unchecked : Qt::Checked);
		ui->sensorsToRecordListWidget->addItem(item);
	}
}

void RecordingWizard::onSensorSelectionChanged(QListWidgetItem* item)
{
	auto currentCostume = ui->costumesToRecordListWidget->currentItem()->data(Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	auto sensorID = item->data(Qt::DisplayRole).toInt();
	if (item->checkState() == Qt::Checked)
	{
		costumesToRecord_[currentCostume].insert(sensorID);
	}
	else{
		costumesToRecord_[currentCostume].erase(sensorID);
	}	
}

void RecordingWizard::onCostumeSelectionChange(QListWidgetItem* item)
{
	if (item != ui->costumesToRecordListWidget->currentItem()){
		return;
	}

	ui->sensorsToRecordListWidget->setEnabled(item->checkState() == Qt::Checked);	

	if (item->checkState() == Qt::Checked){
		button(QWizard::NextButton)->setEnabled(true);
	}
	else{
		
		bool found = false;
		for (unsigned int i = 0; i < ui->costumesToRecordListWidget->count(); ++i)
		{
			auto item = ui->costumesToRecordListWidget->item(i);

			if (item->checkState() == Qt::Checked){
				found = true;
				break;
			}
		}

		button(QWizard::NextButton)->setEnabled(found);
	}
}

void RecordingWizard::onChoose()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Choose output file"),
		plugin::getPaths()->getUserDataPath().string().c_str(),
		tr("Costume data (*.cdata)"));

	if (fileName.isEmpty() == false){
		
		ui->outputLineEdit->setText(fileName);		

	}
}

bool verifyFilePath(const QString & path)
{
	bool ret = false;

	QFileInfo fi(path);

	if (fi.isFile() == true || QFileInfo::exists(fi.absolutePath()) == true)
	{
		ret = true;
	}

	return ret;
}

void RecordingWizard::onOutputFilePathEdit(const QString & path)
{
	button(QWizard::FinishButton)->setEnabled(verifyFilePath(path));
}

void RecordingWizard::pageChanged(int idx)
{
	if (idx == 1){
		button(QWizard::FinishButton)->setEnabled(verifyFilePath(ui->outputLineEdit->text()));
	}
}