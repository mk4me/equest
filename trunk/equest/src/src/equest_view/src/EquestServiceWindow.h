/********************************************************************
	created:	2013/07/08
	created:	8:7:2013   11:18
	filename: 	MdeServiceWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__MDESERVICEWINDOW_H__
#define HEADER_GUARD_MDE_VIEW__MDESERVICEWINDOW_H__

#include <map>
#include <QtWidgets/QMainWindow>
#include <coreui/CoreDockWidget.h>

class QMenu;

class MdeServiceWindow : public QMainWindow
{
    Q_OBJECT;
public:
    MdeServiceWindow(const QString& idName);
	virtual ~MdeServiceWindow();

public:
    //! 
    void saveLayout();
    //! 
    void loadLayout();
    //! 
    //! \param serviceName 
    //! \param widget 
    void createMdeDock( QString serviceName, QWidget* widget );

private:
    coreUI::CoreDockWidget* dockWrap(QWidget* widget, const QString& dockTitle, const QString& serviceName, int propNo );

private:
    QString id;
    std::map<QString, int> serviceCount;
    QMenu* viewMenu;
};


#endif
