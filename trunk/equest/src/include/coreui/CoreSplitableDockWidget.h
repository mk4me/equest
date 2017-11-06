/********************************************************************
    created:  2013/01/30
    created:  30:1:2013   23:35
    filename: CoreSplitableDockWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CORESPLITABLEDOCKWIDGET_H__
#define HEADER_GUARD___CORESPLITABLEDOCKWIDGET_H__

#include <coreui/Export.h>
#include <coreui/CoreDockWidget.h>
//#include <QtWidgets/QMainWindow>

namespace coreUI {

class COREUI_EXPORT CoreSplitableDockWidget : public CoreDockWidget
{
	Q_OBJECT

private:

	void init();

protected:
	explicit CoreSplitableDockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	explicit CoreSplitableDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

public:

	virtual ~CoreSplitableDockWidget();

public slots:
	void splitVerticaly();
	void splitHorizontaly();

	void split(Qt::Orientation orientation);

private:

	virtual CoreSplitableDockWidget * clone() const = 0;

signals:

	void splited(CoreSplitableDockWidget * newDockWidget, Qt::Orientation orientation);

};

}

#endif	//	HEADER_GUARD___CORESPLITABLEDOCKWIDGET_H__
