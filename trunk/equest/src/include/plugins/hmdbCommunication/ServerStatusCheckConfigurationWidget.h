/********************************************************************
	created:  2014/07/16	18:44:42
	filename: ServerStatusCheckConfigurationWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__SERVERSTATUSCHECKCONFIGURATIONWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__SERVERSTATUSCHECKCONFIGURATIONWIDGET_H__

#include <QtGui/QWidget>

namespace Ui
{
	class ServerStatusCheckConfigurationWidget;
}

class ServerStatusCheckConfigurationWidget : public QWidget
{
	Q_OBJECT
		Q_PROPERTY(int tiemoutMS READ tiemoutMS WRITE setTiemoutMS RESET resetTimeoutMS NOTIFY timeoutMSChanged)

public:

	ServerStatusCheckConfigurationWidget(QWidget * parent = nullptr,
		Qt::WindowFlags f = 0);

	virtual ~ServerStatusCheckConfigurationWidget();

	const int tiemoutMS() const;

public slots:

	void setTiemoutMS(const int timeoutMS);
	void resetTimeoutMS();

signals:

	void timeoutMSChanged(int);

private:

	Ui::ServerStatusCheckConfigurationWidget * ui;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__SERVERSTATUSCHECKCONFIGURATIONWIDGET_H__