/********************************************************************
    created:  2014/04/03
    created:  3:4:2014   18:40
    filename: MessageDialog.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MESSAGEDIALOG_H__
#define HEADER_GUARD___MESSAGEDIALOG_H__

#include <QtWidgets/QDialog>
#include "ui_messageDialog.h"

class MessageDialog : public QDialog, public Ui::MessageDialog
{
	Q_OBJECT

public:

	MessageDialog(QWidget * parent, const QString & title, const QString & content);

	virtual ~MessageDialog();

	const QString getText() const;
};

#endif	//	HEADER_GUARD___MESSAGEDIALOG_H__
