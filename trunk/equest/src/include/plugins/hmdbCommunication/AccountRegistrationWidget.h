/********************************************************************
	created:  2014/07/17	22:40:41
	filename: AccountRegistrationWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__ACCOUNTREGISTRATIONWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__ACCOUNTREGISTRATIONWIDGET_H__

#include <QtGui/QWidget>

namespace Ui
{
	class AccountRegistrationWidget;
}

class AccountRegistrationWidget : public QWidget
{

	Q_OBJECT
	Q_PROPERTY(QString passwordRepeat READ passwordRepeat WRITE setPasswordRepeat RESET resetPasswordRepeat NOTIFY passwordRepeatChanged)
	Q_PROPERTY(QString userName READ userName WRITE setUserName RESET resetUserName NOTIFY userNameChanged)
	Q_PROPERTY(QString surname READ surname WRITE setSurname RESET resetSurname NOTIFY surnameChanged)
	Q_PROPERTY(QString email READ email WRITE setEmail RESET resetEmail NOTIFY emailChanged)

public:

	AccountRegistrationWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~AccountRegistrationWidget();

	const QString passwordRepeat() const;
	const QString userName() const;
	const QString surname() const;
	const QString email() const;

public slots:

	void setPasswordRepeat(const QString & passwordRepeat);
	void setUserName(const QString & userName);
	void setSurname(const QString & surname);
	void setEmail(const QString & email);

	void resetPasswordRepeat();
	void resetUserName();
	void resetSurname();
	void resetEmail();

signals:

	void passwordRepeatChanged(const QString & login);
	void userNameChanged(const QString & password);
	void surnameChanged(const QString & login);
	void emailChanged(const QString & password);

private:

	Ui::AccountRegistrationWidget * ui;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__ACCOUNTREGISTRATIONWIDGET_H__