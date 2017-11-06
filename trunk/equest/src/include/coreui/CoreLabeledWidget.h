/********************************************************************
    created:  2013/12/20
    created:  20:12:2013   12:32
    filename: CoreLabeledWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CORELABELEDWIDGET_H__
#define HEADER_GUARD___CORELABELEDWIDGET_H__

#include <coreui/Export.h>
#include <QtWidgets/QWidget>

class QLabel;
class QHBoxLayout;

namespace coreUI
{
	//! Kalsa s³u¿¹ca do rozszerzania widgetów o ich opisy
	class COREUI_EXPORT CoreLabeledWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit CoreLabeledWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
		virtual ~CoreLabeledWidget();

		QLabel * label();
		const QLabel * label() const;

		void setLabeledWidget(QWidget * widget, const bool buddy = true);

		QWidget * labeledWidget();
		const QWidget * labeledWidget() const;

		static CoreLabeledWidget * create(const QString & text, QWidget * widget,
			const bool buddy = true);

	private:
		QLabel * label_;
		QWidget * labeledWidget_;
		QHBoxLayout * layout_;
	};
}

#endif	//	HEADER_GUARD___CORELABELEDWIDGET_H__
