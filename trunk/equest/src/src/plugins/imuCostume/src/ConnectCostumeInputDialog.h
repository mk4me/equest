/********************************************************************
	created:  2015/05/09	20:26:40
	filename: ConnectCostumeInputDialog.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___CONNECTCOSTUMEINPUTDIALOG_H__
#define __HEADER_GUARD___CONNECTCOSTUMEINPUTDIALOG_H__

#include <QtWidgets/QDialog>

namespace Ui
{
	class ConnectCostumeInputDialog;
}

class ConnectCostumeInputDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ConnectCostumeInputDialog(QWidget *parent = 0, Qt::WindowFlags f = 0,
		const QString & ip = QString("192.168.001.173"));

	virtual ~ConnectCostumeInputDialog();

	QString ip() const;
	unsigned int port() const;

private:

	Ui::ConnectCostumeInputDialog * ui;
};

#endif	// __HEADER_GUARD___CONNECTCOSTUMEINPUTDIALOG_H__