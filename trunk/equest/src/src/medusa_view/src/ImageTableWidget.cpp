#include "MdePCH.h"
#include "ImageTableWidget.h"
#include "ImageTableModel.h"
#include "AnalisisWidget.h"
#include "corelib/ISourceManager.h"
#include "plugins/hmdbCommunication/IHMDBSource.h"
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include "utils/SmartPtr.h"
#include "QtCore/QSortFilterProxyModel"

ImageTableTab::ImageTableTab(AnalisisWidget* aw, const QIcon& icon, const QString& label) : 
	SimpleTab(new ImageTableWidget(), icon, label),
	analysisWidget(aw)
{
	tableWidget = qobject_cast<ImageTableWidget*>(getMainWidget());
	tableWidget->setStyleSheet("background-color:white");
	tableModel = new ImageTableModel(tableWidget);
	QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(tableWidget);
	proxyModel->setSourceModel(tableModel);
	tableWidget->setSortingEnabled(true);
	tableWidget->setModel(proxyModel);
	connect(tableWidget, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onTableClicked(const QModelIndex&)));
}

void ImageTableTab::setActive(bool val)
{
	if (val) {
		//tableModel->rebuildTable();
	    tableWidget->resizeColumnsToContents();
		tableModel->clearTable();
		auto worker = tableModel->getWorker().release();
		QThread* thread = new QThread;
		worker->moveToThread(thread);

		connect(worker, SIGNAL(rowProduced(const ImageTableRow&)), tableModel, SLOT(addRow(const ImageTableRow&)));
		connect(thread, SIGNAL(started()), worker, SLOT(process()));
		connect(thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
		connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
		thread->start();
	}
}

void ImageTableTab::onTableClicked(const QModelIndex& index)
{
	if (index.isValid()) {
		QString cellText = index.data().toString();
		auto perspective = tryGetPerspective();
		if (perspective) {
			auto item = perspective->tryGetHierarchyItem(cellText.toStdString() + std::string(".png"));
			if (item) {
				analysisWidget->createVisualizer(item, item->getDefaultHelper());
			}
		}
		cellText.begin();
	}
}

dicom::DicomPerspectiveConstPtr ImageTableTab::tryGetPerspective() const
{
	auto comm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (comm != nullptr) {
		auto count = comm->viewManager()->hierarchyPerspectivesCount();
		for (int i = 0; i < count; ++i) {
			dicom::DicomPerspectiveConstPtr dp = utils::dynamic_pointer_cast<const dicom::DicomPerspective>(comm->viewManager()->hierarchyPerspective(i));
			if (dp) {
				return dp;
			}
		}
	}
	return dicom::DicomPerspectiveConstPtr();
}

