/********************************************************************
	created:  2014/07/16	13:13:38
	filename: ResetPasswordWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__RESETPASSWORDWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__RESETPASSWORDWIDGET_H__

#include <QtGui/QWidget>

namespace Ui
{
	class ResetPasswordWidget;
}

class ResetPasswordWidget : public QWidget
{

	Q_OBJECT

public:
	ResetPasswordWidget(const QString & defaultServerURL = QString(), QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~ResetPasswordWidget();

private:

	const bool verify() const;

private slots:

	void resetServiceURL();
	void resetPassword();

private:
	Ui::ResetPasswordWidget * ui;
	const QString defaultServerURL;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__RESETPASSWORDWIDGET_H__