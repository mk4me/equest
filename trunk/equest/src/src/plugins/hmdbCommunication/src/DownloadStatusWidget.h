/********************************************************************
    created:  2012/03/18
    created:  18:3:2012   23:20
    filename: DownloadStatusWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__DOWNLOADSTATUSWIDGET_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__DOWNLOADSTATUSWIDGET_H__

#include "ui_DownloadStatusWidget.h"

//! Klasa widgeta obsługującego progress pobierania
class DownloadStatusWidget : public QWidget, public Ui::DownloadStatusWidget
{
	Q_OBJECT

public:
	//! domyślny konstruktor
	DownloadStatusWidget(QWidget * parent = nullptr);
	//! Wirtualny destruktor
	virtual ~DownloadStatusWidget();
};

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__DOWNLOADSTATUSWIDGET_H__
