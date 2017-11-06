/********************************************************************
    created:  2012/05/06
    created:  6:5:2012   0:34
    filename: AboutDialog.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ABOUTDIALOG_H__
#define HEADER_GUARD___ABOUTDIALOG_H__

#include "ui_AboutDialog.h"

//! prosty dialog about
class AboutDialog : public QDialog, public Ui::AboutDialog
{
	Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~AboutDialog();
};

#endif	//	HEADER_GUARD___ABOUTDIALOG_H__
