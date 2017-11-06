#include "LoginDialog.h"


LoginDialog::LoginDialog(QWidget* parent, const QString& user, const QString& pass) :
	QDialog(parent)
{
	setupUi(this);
	loginLineEdit->setText(user);
	passwordLineEdit->setText(pass);
}

QString LoginDialog::getUser()
{
	return loginLineEdit->text();
}

QString LoginDialog::getPassword()
{
	return passwordLineEdit->text();
}
