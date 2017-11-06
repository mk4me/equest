/********************************************************************
	created:  2016/07/21
	filename: DataSourceWidget.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__DATASOURCEWIDGET_H__
#define __HEADER_GUARD_SKIDATA__DATASOURCEWIDGET_H__

#include <QtWidgets/QWidget>

namespace Ui {
	class SkiDataSourceWidget;
}

namespace skidata
{
	class SkiDataSource;

	class DataSourceWidget : public QWidget
	{
		Q_OBJECT

	public:
		DataSourceWidget(SkiDataSource * ds, QWidget * parent = nullptr, const Qt::WindowFlags f = 0);
		virtual ~DataSourceWidget();

	private slots:

		void onOpen();
		void onCustomContextMenuRequest(const QPoint & position);
		
		void onLoad();
		void onLoadAll();

		void onUnload();
		void onUnloadAll();

		void onRemove();
		void onRemoveAll();

	private:
		
		SkiDataSource * ds;
		Ui::SkiDataSourceWidget * ui;
	};
}

#endif  // __HEADER_GUARD_SKIDATA__DATASOURCEWIDGET_H__