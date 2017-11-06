/********************************************************************
	created:	2015/09/07
	created:	13:06
	filename: 	UpdateFilesDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMDBCOMMUNICATION__UpdateFilesDialog_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__UpdateFilesDialog_H__

#include "ui_UpdateFilesDialog.h"
#include "QStringListModel"
#include <plugins/hmdbCommunication/Types.h>

class UpdateFilesDialog : public QDialog, private Ui::UpdatedFilesDialog
{
	Q_OBJECT;
	public:
		UpdateFilesDialog(QWidget* parent, const hmdbCommunication::StorageFileNames& files);
		virtual ~UpdateFilesDialog() {}
private:
	QStringListModel model;
};


#endif // HEADER_GUARD 
