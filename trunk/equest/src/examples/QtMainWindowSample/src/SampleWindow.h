/********************************************************************
	created:  2011/03/02
	created:  2:3:2011   12:36
	filename: SampleWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_QTSAMPLE__SAMPLEWIDGET_H__
#define HEADER_GUARD_QTSAMPLE__SAMPLEWIDGET_H__

#include <QtWidgets/QMainWindow>
#include "ui_SampleWindow.h"

class SampleWindow : public QMainWindow, public Ui::SampleWindow
{
    Q_OBJECT;
public:
    SampleWindow();

public slots:
    void showHelloWorld();
};

#endif  // HEADER_GUARD_QTSAMPLE__SAMPLEWIDGET_H__
