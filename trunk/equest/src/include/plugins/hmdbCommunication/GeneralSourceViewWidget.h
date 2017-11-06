/********************************************************************
	created:  2014/08/20	17:16:33
	filename: GeneralSourceViewWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__HMDBCOMMUNICATION_GENERALSOURCEVIEWWIDGET_H__
#define __HEADER_GUARD__HMDBCOMMUNICATION_GENERALSOURCEVIEWWIDGET_H__

#include <plugins/hmdbCommunication/Export.h>
#include <QtWidgets/QWidget>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>

class DataViewConfigurationWidget;

namespace Ui
{
	class GeneralSourceViewWidget;
}

namespace hmdbCommunication
{
	class IDataSourcePerspective;
	class IDataSourceContent;
}

class DataViewWidget;
class OperationProgressWidget;

class HMDBCOMMUNICATION_EXPORT GeneralSourceViewWidget : public QWidget
{
	Q_OBJECT;

public:
	GeneralSourceViewWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~GeneralSourceViewWidget();

	DataViewConfigurationWidget * dataViewConfiguration();
	DataViewWidget * dataView();

private Q_SLOTS:

	void onOperationStarts();
	void onOperationEnds();

	void onPerspectiveChanged();
	void onContentChanged();
	void onFilterChanged();

private:
	Ui::GeneralSourceViewWidget * ui;
	DataViewWidget * dataView_;
	DataViewConfigurationWidget * dataViewConfiguration_;
	OperationProgressWidget * operationProgress;

};

#endif	// __HEADER_GUARD___IMUSOURCEVIEWWIDGET_H__