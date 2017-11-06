#include "DataSourceWidget.h"
#include "ui_DataSourceWidget.h"
#include <QtWidgets/QFileDialog>
#include <utils/Filesystem.h>
#include "DataSource.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenu>

using namespace skidata;

DataSourceWidget::DataSourceWidget(SkiDataSource * ds, QWidget * parent, const Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::SkiDataSourceWidget), ds(ds)
{
	ui->setupUi(this);	
}

DataSourceWidget::~DataSourceWidget()
{
	delete ui;
}

void DataSourceWidget::onOpen()
{	
	static QString lastPath = QString();

	auto pdir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), lastPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	auto dir = pdir.toStdString();

	utils::Filesystem::Path p(dir);
	if (p.empty() == false && utils::Filesystem::pathExists(p) == true) {
		lastPath = pdir;
		auto managed = ds->currentRuns();

		if (std::find(managed.begin(), managed.end(), p) == managed.end()) {
			//nie ma - mozemy ladowa
			try {
				auto idx = ds->addRun(p);
				auto sItem = new QTreeWidgetItem;
				sItem->setText(0, QObject::tr("Run %1").arg(idx));								
				sItem->setText(1, QString());
				sItem->setText(2, QString::fromStdString(dir));
				ui->dataTreeWidget->addTopLevelItem(sItem);
			}
			catch (const std::exception & e) {
				QMessageBox::critical(this, tr("Ski run loading"), tr("Failed to load particular ski run: %1").arg(e.what()));
			}
			catch (...) {
				QMessageBox::critical(this, tr("Ski run loading"), tr("Failed to load particular ski run with unknown reasons."));
			}
		}
		else {
			//juz mamy go na liscie
			QMessageBox::information(this, tr("Ski run loading"), tr("Provided run already loaded, skipping."));
		}
	}
}

void DataSourceWidget::onCustomContextMenuRequest(const QPoint & position)
{
	QMenu menu(this);
	menu.addAction(ui->actionLoad);
	menu.addAction(ui->actionUnload);
	menu.addAction(ui->actionRemove);
	menu.addSeparator();
	menu.addAction(ui->actionLoad_All);
	menu.addAction(ui->actionUnload_All);
	menu.addAction(ui->actionRemove_All);

	auto managedRuns = ds->currentRuns();

	std::size_t loaded = 0;
	for (const auto & p : managedRuns) {
		if (ds->isLoaded(p) == true) {
			++loaded;
		}
	}

	auto item = ui->dataTreeWidget->currentItem();

	if (item != nullptr) {

		auto path = utils::Filesystem::Path(item->data(2, Qt::DisplayRole).toString().toStdString().c_str());	

		ui->actionLoad->setDisabled(ds->isLoaded(path));
		ui->actionUnload->setEnabled(ds->isLoaded(path));
		ui->actionRemove->setEnabled(true);

		ui->actionLoad_All->setEnabled(loaded != managedRuns.size());
		ui->actionUnload_All->setEnabled(loaded > 0);
		ui->actionRemove_All->setEnabled(true);
	}
	else {
		ui->actionLoad->setEnabled(false);
		ui->actionUnload->setEnabled(false);
		ui->actionRemove->setEnabled(false);

		ui->actionLoad_All->setEnabled(loaded != managedRuns.size());
		ui->actionUnload_All->setEnabled(loaded > 0);
		ui->actionRemove_All->setEnabled(managedRuns.size() > 0);
	}

	menu.exec(position);
}

void DataSourceWidget::onLoad()
{
	auto item = ui->dataTreeWidget->currentItem();
	auto path = utils::Filesystem::Path(item->data(2, Qt::DisplayRole).toString().toStdString().c_str());
	try {
		ds->loadRun(path);
	}
	catch (...) {

	}
}

void DataSourceWidget::onLoadAll()
{
	auto managedRuns = ds->currentRuns();

	for (const auto & p : managedRuns) {
		try {
			if (ds->isLoaded(p) == false) {
				ds->loadRun(p);
			}
		}
		catch (...) {

		}
	}
}

void DataSourceWidget::onUnload()
{
	auto item = ui->dataTreeWidget->currentItem();
	auto path = utils::Filesystem::Path(item->data(2, Qt::DisplayRole).toString().toStdString().c_str());
	try {
		ds->unloadRun(path);
	}
	catch (...) {

	}
}

void DataSourceWidget::onUnloadAll()
{
	auto managedRuns = ds->currentRuns();

	for (const auto & p : managedRuns) {
		try {
			if (ds->isLoaded(p) == true) {
				ds->unloadRun(p);
			}
		}
		catch (...) {

		}
	}
}

void DataSourceWidget::onRemove()
{
	auto item = ui->dataTreeWidget->currentItem();
	auto path = utils::Filesystem::Path(item->data(2, Qt::DisplayRole).toString().toStdString().c_str());
	try {
		ds->removeRun(path);
		delete ui->dataTreeWidget->takeTopLevelItem(ui->dataTreeWidget->indexOfTopLevelItem(item));
	}
	catch (...) {

	}
}

void DataSourceWidget::onRemoveAll()
{
	auto managedRuns = ds->currentRuns();

	for (const auto & p : managedRuns) {
		try {
			ds->removeRun(p);
		}
		catch (...) {

		}
	}

	ui->dataTreeWidget->clear();
}