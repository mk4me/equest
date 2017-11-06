/********************************************************************
	created:  2014/08/20	16:45:46
	filename: OperationProgressWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__OPERATIONPROGRESSWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__OPERATIONPROGRESSWIDGET_H__

#include <plugins/hmdbCommunication/Export.h>
#include <QtCore/QTimer>
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include <utils/SmartPtr.h>
#include <coreui/CoreCursorChanger.h>


namespace Ui
{
	class OperationProgressWidget;
}

class HMDBCOMMUNICATION_EXPORT OperationProgressWidget : public QWidget
{
	Q_OBJECT;

public:
	OperationProgressWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~OperationProgressWidget();

	void setOperation(DataViewWidget::IOperation * op);

public slots:

	void resetStatus();

private slots:

	void refreshOperationStatus();
	void onCancel();

private:
	utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger;
	QTimer timer;
	Ui::OperationProgressWidget * ui;
	DataViewWidget::IOperation * operation;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__OPERATIONPROGRESSWIDGET_H__