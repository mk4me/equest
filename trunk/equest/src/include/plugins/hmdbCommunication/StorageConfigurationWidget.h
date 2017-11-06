/********************************************************************
	created:  2014/07/16	19:55:45
	filename: StorageConfigurationWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__STORAGECONFIGURATIONWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__STORAGECONFIGURATIONWIDGET_H__

#include <QtGui/QWidget>

namespace Ui
{
	class StorageConfigurationWidget;
}

class StorageConfigurationWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString storageFile READ storageFile WRITE setStorageFile RESET resetStorageFile NOTIFY storageFileChanged)
	Q_PROPERTY(QString password READ password WRITE setPassword RESET resetPassword NOTIFY passwordChanged)

public:

	StorageConfigurationWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~StorageConfigurationWidget();

	const QString storageFile() const;
	const QString password() const;

public slots:

	void setStorageFile(const QString & storageFile);
	void setPassword(const QString & password);

	void resetStorageFile();
	void resetPassword();

	void openStorage();

signals:

	void storageFileChanged(const QString & storageFile);
	void passwordChanged(const QString & storage);	

private:

	Ui::StorageConfigurationWidget * ui;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__STORAGECONFIGURATIONWIDGET_H__