/********************************************************************
	created:  2014/07/17	08:18:10
	filename: UserLoginWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__USERLOGINWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__USERLOGINWIDGET_H__

#include <QtGui/QWidget>

namespace Ui
{
	class UserLoginDataWidget;
}

class UserLoginWidget : public QWidget
{

	Q_OBJECT
	Q_PROPERTY(QString login READ login WRITE setLogin RESET resetLogin NOTIFY loginChanged)
	Q_PROPERTY(QString password READ password WRITE setPassword RESET resetPassword NOTIFY passwordChanged)

public:
	UserLoginWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~UserLoginWidget();

	const QString login() const;
	const QString password() const;

public slots:

	void setLogin(const QString & login);
	void setPassword(const QString & password);

	void resetLogin();
	void resetPassword();	

signals:

	void loginChanged(const QString & login);
	void passwordChanged(const QString & password);	

private:

	Ui::UserLoginDataWidget * ui;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__USERLOGINWIDGET_H__