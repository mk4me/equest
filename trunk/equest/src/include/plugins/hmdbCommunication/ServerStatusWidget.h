/********************************************************************
	created:  2014/07/16	19:13:30
	filename: ServerStatusWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBOCOMMUNICATION__SERVERSTATUSWIDGET_H__
#define __HEADER_GUARD_HMDBOCOMMUNICATION__SERVERSTATUSWIDGET_H__

#include <QtWidgets/QWidget>
#include <QtGui/QPixmap>

namespace Ui
{
	class ServerStatusWidget;
}

class ServerStatusWidget : public QWidget
{
	Q_OBJECT

public:

	static const char UnknownStatus = 0;
	static const char OnlineStatus = 2;
	static const char OfflineStatus = 1;
	static const char MixedStatus = 3;

public:

	ServerStatusWidget(const QPixmap & online = QPixmap(QString(":/hmdbCommunication/icons/dostepny.png")),
		const QPixmap & offline = QPixmap(QString(":/hmdbCommunication/icons/niedostepny.png")),
		const QPixmap & unknown = QPixmap(QString(":/hmdbCommunication/icons/nieznanydostepny.png")),
		const QPixmap & partialyOnline = QPixmap(QString(":/hmdbCommunication/icons/czesciowodostepny.png")),
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~ServerStatusWidget();

public slots:

	void setServerStatus(char status);

private:

	Ui::ServerStatusWidget * ui;
	const QPixmap partialyOnline;
	const QPixmap online;
	const QPixmap offline;
	const QPixmap unknown;

};

#endif	// __HEADER_GUARD_HMDBOCOMMUNICATION__SERVERSTATUSWIDGET_H__