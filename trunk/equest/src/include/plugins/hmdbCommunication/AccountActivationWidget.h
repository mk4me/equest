/********************************************************************
	created:  2014/07/16	12:18:53
	filename: AccountActivationWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__ACCOUNTACTIVATIONWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__ACCOUNTACTIVATIONWIDGET_H__

#include <QtGui/QWidget>

namespace Ui
{
	class AccountActivationWidget;	
}

class AccountActivationWidget : public QWidget
{

	Q_OBJECT
	Q_PROPERTY(QString login READ login WRITE setLogin RESET resetLogin NOTIFY loginChanged)
	Q_PROPERTY(QString activationCode READ activationCode WRITE setActivationCode RESET resetActivationCode NOTIFY activationCodeChanged)

public:
	
	AccountActivationWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~AccountActivationWidget();

	const QString login() const;
	const QString activationCode() const;

public slots:

	void setLogin(const QString & login);
	void setActivationCode(const QString & activationCode);

	void resetLogin();
	void resetActivationCode();

signals:

	void loginChanged(const QString & login);
	void activationCodeChanged(const QString & activationCode);

private:
	Ui::AccountActivationWidget * ui;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__ACCOUNTACTIVATIONWIDGET_H__