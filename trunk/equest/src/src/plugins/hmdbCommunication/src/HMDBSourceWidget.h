/********************************************************************
	created:  2014/07/17	15:21:13
	filename: HMDBSourceWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCEWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCEWIDGET_H__

#include <QtWidgets/QWidget>

class QSplitter;
class QMainWindow;
class SourceOptionsWidget;

namespace hmdbCommunication
{
	class HMDBSource;
}

class HMDBSourceWidget : public QWidget
{
	Q_OBJECT

public:

	HMDBSourceWidget(hmdbCommunication::HMDBSource * source, QWidget * parent = nullptr,
		Qt::WindowFlags f = 0);

	virtual ~HMDBSourceWidget();

private:

	QSplitter * mainWidget;
	QMainWindow * viewsWidget;
	SourceOptionsWidget * sourceOptionsWidget;

	hmdbCommunication::HMDBSource * source;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCEWIDGET_H__