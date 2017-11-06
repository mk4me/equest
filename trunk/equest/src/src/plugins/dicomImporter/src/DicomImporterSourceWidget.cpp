//#include "DicomImporterPCH.h"
#include "DicomImporterSourceWidget.h"
#include "ui_DicomSource.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLayout>
#include <utils/Debug.h> 
#include <corelib/PluginCommon.h>
#include <loglib/ILog.h>
#include <corelib/IFileDataManager.h>
#include <utils/Filesystem.h>
#include <QtWidgets/QFileDialog>
#include "DicomImporterSource.h"
#include <QtCore/QDir>
#include <QtWidgets/QProgressDialog>
#include "dicomlib/DicomImporter.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "dicomlib/DicomSplitterModel.h"

using namespace dicomImporter;

DicomImporterSourceWidget::DicomImporterSourceWidget( DicomImporterSource* source ) :
    dicomSource(source),
    ui(new Ui::DicomSource)
{
    ui->setupUi(this);
    connect(ui->importFromButton, SIGNAL(clicked()), this, SLOT(onSelectImportDir()));
    connect(ui->exportToButton, SIGNAL(clicked()), this, SLOT(onSelectSaveDir()));
    connect(ui->importButton, SIGNAL(clicked()), this, SLOT(onImport()));
	connect(ui->importSingleButton, SIGNAL(clicked()), this, SLOT(onImportSingle()));
	connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(onSave()));
}


class Refresher
{
public:
    Refresher(QProgressDialog* dlg, DicomImporter* imp) : 
      dialog(dlg),
      importer(imp),
      counter(0)
      {
          importer->setCallBack( boost::bind( &Refresher::refresh, this, ::_1 ));
          QApplication::processEvents();
      }

private:
    void refresh(const std::string& str) {
        dialog->setValue(++counter);
    }
private:
    QProgressDialog* dialog;
    DicomImporter* importer;
    int counter;
};

void dicomImporter::DicomImporterSourceWidget::onImport()
{
    
    
    QString importFrom = ui->importFromEdit->text();
    QString exportTo = ui->exportToEdit->text();

    QDir dir;
    if ( dir.exists(importFrom) && dir.exists(exportTo) ) {
        utils::Filesystem::Path from = importFrom.toStdString();
        utils::Filesystem::Path to = exportTo.toStdString();
        //dicomSource->import(from, to);
        
        DicomImporter importer(ui->startIndex->value());
        auto inter = importer.import(from);
        QProgressDialog progress("Importing files...", "Abort", 0, inter->getNumImages(), this);
        progress.setWindowModality(Qt::WindowModal);
        Refresher r(&progress, &importer);
        std::vector<DicomInternalStructPtr> splits = importer.split(inter);

		splitterModel = utils::make_shared<DicomSplitterModel>(this, std::move(splits));
		ui->tableView->setModel(splitterModel.get());
		return;

        

    } else {
        QMessageBox::critical(this, tr("Error"), tr("Unable to import DICOM structure, check directories"));
    }

}

void dicomImporter::DicomImporterSourceWidget::onSelectImportDir()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Directory"));
    if ( dirPath.isNull() == false ) {
        ui->importFromEdit->setText(dirPath);
    }
}

void dicomImporter::DicomImporterSourceWidget::onSelectSaveDir()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Directory"));
    if ( dirPath.isNull() == false ) {
        ui->exportToEdit->setText(dirPath);
    }
}

void dicomImporter::DicomImporterSourceWidget::onImportSingle()
{
	
//	std::string filename = "E:/programming/WORK/PET_CT_dane/De_Lorme_Urbanczyk^Iwona_673675/CT_10190/2/1.2.840.113704.1.111.5596.1264074742.2380.dcm";
//	DicomImagePtr image = object->get();
//	QPixmap pixmap = convertToPixmap(image);
	QString importFrom = ui->importFromEdit->text();
	QString exportTo = ui->exportToEdit->text();
	/*importFrom = "E:/programming/WORK/PET_CT_dane/De_Lorme_Urbanczyk^Iwona_673675/PT_10190/767120";
	exportTo = "E:/programming/WORK/PET_CT_dane";*/

	QDir dir;
	if (dir.exists(importFrom) && dir.exists(exportTo)) {
		utils::Filesystem::Path from = importFrom.toStdString();
		utils::Filesystem::Path to = exportTo.toStdString();
		//dicomSource->import(from, to);

		DicomImporter importer(ui->startIndex->value());
		auto inter = importer.importRaw(from);
		QProgressDialog progress("Importing files...", "Abort", 0, inter->getNumImages(), this);
		progress.setWindowModality(Qt::WindowModal);
		Refresher r(&progress, &importer);
		std::vector<DicomInternalStructPtr> splits = importer.split(inter);

		DicomSaver saver;
		for (auto it = splits.begin(); it != splits.end(); ++it) {
			importer.convertImages(*it, from, to);
			saver.save(to, *it);
		}

	}
	else {
		QMessageBox::critical(this, tr("Error"), tr("Unable to import DICOM structure, check directories"));
	}
}

void dicomImporter::DicomImporterSourceWidget::onSave()
{
	DicomSaver saver;
	DicomImporter importer(ui->startIndex->value());
	QString importFrom = ui->importFromEdit->text();
	QString exportTo = ui->exportToEdit->text();
	QDir dir;
	if (dir.exists(importFrom) && dir.exists(exportTo)) {
		utils::Filesystem::Path from = importFrom.toStdString();
		utils::Filesystem::Path to = exportTo.toStdString();
		const auto& splits = splitterModel->getSplits();
		for (auto it = splits.begin(); it != splits.end(); ++it) {
			importer.convertImages(*it, from, to);
			saver.save(to, *it);
		}
	}
}


