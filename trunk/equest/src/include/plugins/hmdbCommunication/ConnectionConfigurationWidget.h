/********************************************************************
created:  2014/07/16	13:39:04
filename: ConnectionConfigurationWidget.cpp
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__CONNECTIONCONFIGURATIONWIDGET_CPP__
#define __HEADER_GUARD_HMDBCOMMUNICATION__CONNECTIONCONFIGURATIONWIDGET_CPP__

#include <QtGui/QWidget>

namespace Ui
{
	class ConnectionConfigurationWidget;
}

class ConnectionConfigurationWidget : public QWidget
{
	Q_OBJECT

public:

	ConnectionConfigurationWidget(const QString & defaultServiceURL = QString(),
		const QString & defaultServiceCAPath = QString(),
		const QString & defaultDataURL = QString(),
		const QString & defaultDataCAPath = QString(),
		const QString & defaultDataUser = QString(),
		const QString & defaultDataPassword = QString(),
		QWidget * parent = nullptr,
		Qt::WindowFlags f = 0);

	virtual ~ConnectionConfigurationWidget();

	const bool validate() const;

	const QString serviceURL() const;
	const QString serviceCAPath() const;	
	const QString dataFTPURL() const;
	const QString dataCAPath() const;	
	const QString dataFTPUser() const;
	const QString dataFTPPassword() const;

private:

	void resetConnection();

private:

	Ui::ConnectionConfigurationWidget * ui;
	const QString defaultServiceURL;
	const QString defaultServiceCAPath;
	const QString defaultDataURL;
	const QString defaultDataCAPath;
	const QString defaultDataUser;
	const QString defaultDataPassword;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__CONNECTIONCONFIGURATIONWIDGET_CPP__