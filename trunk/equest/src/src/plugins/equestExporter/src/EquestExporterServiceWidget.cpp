#include "EquestExporterPCH.h"
#include "EquestExporterServiceWidget.h"
#include "ui_equestExporter.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtGui/QDesktopServices>
#include <corelib/PluginCommon.h>
#include <corelib/IFileDataManager.h>
#include <utils/Filesystem.h>
#include "EquestExporterService.h"
#include <corelib/ISourceManager.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <quazip/JlCompress.h>
#include <QtCore/QUrl>
#include <tuple>

using namespace equestExporter;
typedef utils::Filesystem fs;




EquestExporterServiceWidget::EquestExporterServiceWidget(EquestExporterService* service) :
    equestService(service),
    ui(new Ui::EquestExporter())
{
    ui->setupUi(this);
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(onExport()));
	connect(ui->extractButton, SIGNAL(clicked()), this, SLOT(onExtract()));
	connect(ui->extractToButton, SIGNAL(clicked()), this, SLOT(onExtractDirDialog()));
	connect(ui->exportFromButton, SIGNAL(clicked()), this, SLOT(onExportFrom()));
    connect(ui->imagesCheck, SIGNAL(stateChanged(int)), this, SLOT(onPackChecks()));
    connect(ui->metaCheck, SIGNAL(stateChanged(int)), this, SLOT(onPackChecks()));
    connect(ui->clearEquestFolderButton, SIGNAL(clicked()), this, SLOT(onClearEquestExport()));
    connect(ui->openEquestFolderButton, SIGNAL(clicked()), this, SLOT(onOpenEquestExport()));

    ui->chooseFormatComboBox->addItem("CSV");

	ui->progressBar->setVisible(false);
	ui->progressBarLabel->setVisible(false);

	callbackFunction = std::bind(&EquestExporterServiceWidget::callback, this, std::placeholders::_1, std::placeholders::_2);
}

void equestExporter::EquestExporterServiceWidget::onExport()
{    
    
    QString user = ui->chooseUserComboBox->currentText();
    QDir exportFrom(ui->exportFromLineEdit->text() + "/Data");
    QDir outDir(ui->exportFromLineEdit->text());

    if (!exportFrom.exists()) {
        QMessageBox::warning(this, tr("Error"), tr("Data export doesn't exist. Please Export Data before Export Attributes"));
        return;
    }

    if (user.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Choose user before exporting"));
        return;
    }


	ExportConfig config;
	//config.skipIdentical = ui->skipIdenticalComboBox->isChecked();
	//config.pixelByPixel = !isKeyPoints();
	//config.pointsDensity = ui->curveDensitySpinBox->value();
 //   config.normalizePointVectorsLenght = ui->normalizePointsCheck->isChecked();
	//RecreateAnnotationsConfig recreate;
	//recreate.recreate = ui->recreateGroup->isChecked();
	//recreate.merge = ui->mergeButton->isChecked();
	//recreate.drawJoints = ui->jointsButton->isChecked();
	//recreate.drawBones = ui->bonesButton->isChecked();
	//recreate.drawInflammations = ui->inflammationButton->isChecked();
	//recreate.drawSkins = ui->skinButton->isChecked();
	//config.recreateAnnotations = recreate;
	try {
        ExporterThread* t = new ExporterThread();
        connect(t, SIGNAL(progressChanged(double, const QString&)), this, SLOT(callback(double, const QString&)), Qt::QueuedConnection);
        connect(t, SIGNAL(finished()), this, SLOT(afterExport()));
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
		connect(ui->curveDescriptionComboBox, SIGNAL(editTextChanged(const QString &)), this, SLOT(curveDescChanged(TODO)));
        
		//exporterModel->exportData(outDir.absolutePath(), user, exportFrom.absolutePath(), config, callbackFunction);
		CallbackCollector::Operation op = std::bind(static_cast<void(ExporterModel::*)(const QString&, const QString&, const QString&, const ExportConfig&, ExporterModel::CallbackFunction)>(&ExporterModel::exportData), exporterModel.get(),
			outDir.absolutePath(), user, exportFrom.absolutePath(), config, std::placeholders::_1);
        t->addOperation(op, 1.0, tr("Exporting"));

        disabler = utils::make_shared<WidgetDisabler>(ui->miscBox, ui->dataBox);
        disabler->addWidget(ui->exportFromButton);
        disabler->addWidget(ui->normalizePointsCheck);
        disabler->addWidget(ui->skipIdenticalComboBox);
        //disabler->addWidget(ui->chooseFormatComboBox);
        disabler->addWidget(ui->curveDensitySpinBox);
		disabler->addWidget(ui->curveDescriptionComboBox);
        disabler->addWidget(ui->chooseUserComboBox);
        disabler->addWidget(ui->exportButton);
        disabler->checkButton(ui->exportButton);
        t->start();
	} catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Error"), tr(e.what()));
	} catch (...) {
		QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
	}
}


void equestExporter::EquestExporterServiceWidget::setExporter(ExporterModelPtr exporterModel)
{
	this->exporterModel = exporterModel;
}

void equestExporter::EquestExporterServiceWidget::onExtract()
{    
    
	QString dirPath = ui->extractToLineEdit->text();

	auto icomm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (!icomm) {
		QMessageBox::critical(this, tr("Error"), tr("Communication plugin was not found"));
		return;
	}
	int count = icomm->shallowContextsCount();
	if (count == 0) {
		QMessageBox::critical(this, tr("Error"), tr("User is not logged"));
		return;
	}
	else if (count > 1) {
		QMessageBox::critical(this, tr("Error"), tr("More than one user logged"));
		return;
	}

    if (dirPath.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Choose directory where files will be exported"));
        return;
    }

    QDir dir(dirPath);
    if (!dir.exists()) {
        auto create = QMessageBox::question(this, tr("Warning"), tr("Folder does not exist. Create?"), QMessageBox::Yes | QMessageBox::No);
        if (create == QMessageBox::Yes) {
            if (!dir.mkpath(".")) {
                QMessageBox::critical(this, tr("Error"), tr("Unable to create directory"));
                return;
            }
        } else {
            return;
        }
    }

    QString innerDirPath = dirPath + "/" + "Export_" + QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss");
    QDir innerDir(innerDirPath);
    QDir innerDataDir(innerDirPath + "/" + "Data");
    innerDataDir.mkpath(".");
	try {
        //CallbackCollector cc(callbackFunction);
        ExporterThread* t = new ExporterThread();
        connect(t, SIGNAL(progressChanged(double, const QString&)), this, SLOT(callback(double, const QString&)), Qt::QueuedConnection);
        connect(t, SIGNAL(finished()), this, SLOT(afterExtract()));
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
        //connect(t, SIGNAL(terminated()), this, SLOT(afterExtract()));
        //connect(t, SIGNAL(terminated()), t, SLOT(deleteLater()));
               
        
        //exporterModel->extractData(innerDataDir.absolutePath(), callbackFunction);
		auto extractOp = std::bind(static_cast<void(ExporterModel::*)(const QString&, bool, ExporterModel::CallbackFunction)>(&ExporterModel::extractData), exporterModel.get(), innerDataDir.absolutePath(), ui->copyLocalBox->isChecked(), std::placeholders::_1);
        t->addOperation(extractOp, 0.2, tr("Exporting"));
        QString fileName = dirPath + "/" + innerDir.dirName() + "/";
        if (ui->metaCheck->isChecked() && ui->imagesCheck->isChecked() && ui->togetherRadio->isChecked()) {
            fileName += "all.zip";
            //exporterModel->packBoth(innerDirPath, fileName, callbackFunction);
			auto compressOp = std::bind(static_cast<void(ExporterModel::*)(const QString&, const QString&, ExporterModel::CallbackFunction)>(&ExporterModel::packBoth), exporterModel.get(), innerDirPath, fileName, std::placeholders::_1);
            t->addOperation(compressOp, 0.8, tr("Compressing"));
        } else {
            if (ui->metaCheck->isChecked()) {
                QString fn = fileName + "metadata.zip";
                //exporterModel->packMeta(innerDirPath, fn, callbackFunction);
				auto compressOp = std::bind(static_cast<void(ExporterModel::*)(const QString&, const QString&, ExporterModel::CallbackFunction)>(&ExporterModel::packMeta), exporterModel.get(), innerDirPath, fn, std::placeholders::_1);
                t->addOperation(compressOp, 0.2, tr("Compressing metadata"));
            }
            if (ui->imagesCheck->isChecked()) {
                QString fn = fileName + "images.zip";
                //exporterModel->packImages(innerDirPath, fn, callbackFunction);
				CallbackCollector::Operation compressOp = std::bind(static_cast<void(ExporterModel::*)(const QString&, const QString&, ExporterModel::CallbackFunction)>(&ExporterModel::packImages), exporterModel.get(), innerDirPath, fn, std::placeholders::_1);
                t->addOperation(compressOp, 0.6, tr("Compressing images"));
            }
        }
        innerExportDirPath = innerDirPath;
        auto collectUsers = [&](ExporterModel::CallbackFunction f) {
            this->tempUsers = this->exporterModel->getUsers(this->innerExportDirPath, f);
        };
        //auto usersOp = boost::bind(&EquestExporterServiceWidget::setExportFrom, this, innerDirPath, ::_1);
        t->addOperation(collectUsers, 0.05, tr("Collecting users"));
        disabler = utils::make_shared<WidgetDisabler>(ui->miscBox, ui->atributesBox);
        disabler->addWidget(ui->compressOptionBox);
        disabler->addWidget(ui->compressBox);
        disabler->addWidget(ui->extractButton);
        disabler->addWidget(ui->extractToButton);
        disabler->checkButton(ui->extractButton);
        t->start();

        ui->extractButton->setChecked(false);

	} catch (const std::exception& e) {
		QMessageBox::critical(this, tr("Error"), tr(e.what()));
	} catch (...) {
		QMessageBox::critical(this, tr("Error"), tr("Unknown error"));
	}
}

void equestExporter::EquestExporterServiceWidget::onExtractDirDialog()
{
	QString dir = QFileDialog::getExistingDirectory(this, QString());
	ui->extractToLineEdit->setText(dir);
}

void equestExporter::EquestExporterServiceWidget::setExportFrom(const QString& dir, const QStringList& users)
{
	ui->exportFromLineEdit->setText(QDir::toNativeSeparators(dir));
    QDir d(dir);
	ui->curveDescriptionComboBox->setEnabled(true);
    ui->curveDensitySpinBox->setEnabled(true);
	ui->skipIdenticalComboBox->setEnabled(true);
	ui->exportButton->setEnabled(true);
	ui->chooseUserComboBox->setEnabled(true);
    ui->chooseUserComboBox->clear();
	ui->chooseUserComboBox->addItems(users);
}

void equestExporter::EquestExporterServiceWidget::onExportFrom()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
	ui->extractToLineEdit->setText(dir);
	if (!dir.isEmpty()) {
        auto users = exporterModel->getUsers(dir, callbackFunction);
		setExportFrom(dir, users);
	}
}

void equestExporter::EquestExporterServiceWidget::callback(double ratio, const QString& description)
{
	ui->progressBar->setVisible(true);
	ui->progressBar->setValue(ratio * 100);
	ui->progressBarLabel->setVisible(true);
	ui->progressBarLabel->setText(description);
	//QApplication::processEvents();
}

void equestExporter::EquestExporterServiceWidget::onPackChecks()
{
    if (ui->imagesCheck->isChecked() && ui->metaCheck->isChecked()) {
        ui->compressOptionBox->setEnabled(true);
    } else {
        ui->compressOptionBox->setEnabled(false);
    }
}

void equestExporter::EquestExporterServiceWidget::setExportDir(const QString& dir)
{
    ui->extractToLineEdit->setText(dir);
}

QString equestExporter::EquestExporterServiceWidget::getExportDir() const
{
    return ui->extractToLineEdit->text();
}

void equestExporter::EquestExporterServiceWidget::onClearEquestExport()
{
    auto create = QMessageBox::question(this, tr("Warning"), tr("Are you sure you want to delete whole export folder?"), QMessageBox::Yes | QMessageBox::No);
    if (create == QMessageBox::Yes) {
        ExporterThread* t = new ExporterThread();
        connect(t, SIGNAL(progressChanged(double, const QString&)), this, SLOT(callback(double, const QString&)), Qt::QueuedConnection);
        connect(t, SIGNAL(finished()), this, SLOT(afterClear()));
        connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
        //connect(t, SIGNAL(terminated()), this, SLOT(afterClear()));
        auto op = [&](ExporterModel::CallbackFunction f) {
            f(0.0, tr("Started"));
            this->exporterModel->clearEquestExportDir();
            f(1.0, tr("Done"));
        }; 
        t->addOperation(op, 1.0, tr("Clearing folder"));
        disabler = utils::make_shared<WidgetDisabler>(ui->dataBox, ui->atributesBox);
        disabler->addWidget(ui->openEquestFolderButton);
        disabler->addWidget(ui->clearEquestFolderButton);
        disabler->checkButton(ui->clearEquestFolderButton);
        t->start();

        ui->exportFromLineEdit->setText("");
        ui->chooseUserComboBox->clear();
    }
}

void equestExporter::EquestExporterServiceWidget::onOpenEquestExport()
{
    QString path = QString::fromStdString(plugin::getUserDataPath("Export").string());
    QDir dir(QDir::toNativeSeparators(path));
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QDesktopServices::openUrl(QUrl("file:///" + dir.path()));
}

void equestExporter::EquestExporterServiceWidget::afterExtract()
{
    UTILS_ASSERT(this->innerExportDirPath.isEmpty() == false);
    //UTILS_ASSERT(this->tempUsers.isEmpty() == false);
    setExportFrom(this->innerExportDirPath, tempUsers);
    disabler = WidgetDisablerPtr();
}

void equestExporter::EquestExporterServiceWidget::afterExport()
{
    //setExportFrom(this->innerExportDirPath, tempUsers);
    disabler = WidgetDisablerPtr();
}

void equestExporter::EquestExporterServiceWidget::afterClear()
{
    disabler = WidgetDisablerPtr();
}

bool equestExporter::EquestExporterServiceWidget::isKeyPoints()
{
	QString txt = ui->curveDescriptionComboBox->currentText();
	if (txt == QObject::tr("Key points")) {
		return true;
	} else if (txt == QObject::tr("Pixel by pixel")) {
		return false;
	}
	UTILS_ASSERT(false);
	return false;
}

void equestExporter::EquestExporterServiceWidget::curveDescChanged(const QString &)
{
	if (isKeyPoints()) {
		ui->curveDensitySpinBox->setEnabled(true);
	} else {
		ui->curveDensitySpinBox->setEnabled(false);
	}
}
