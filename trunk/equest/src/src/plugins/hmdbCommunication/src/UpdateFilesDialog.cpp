#include "CommunicationPCH.h"
#include <QtGui/QStandardItemModel>
#include "UpdateFilesDialog.h"


UpdateFilesDialog::UpdateFilesDialog(QWidget* parent, const hmdbCommunication::StorageFileNames& files):
	QDialog(parent)
{
	Ui::UpdatedFilesDialog::setupUi(this);
	QStringList list;
	for (auto& file : files) {
		list.push_back(QString::fromStdString(file.second.fileName));
	}
	model.setStringList(list);
	tableView->setModel(&model);
	tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}
