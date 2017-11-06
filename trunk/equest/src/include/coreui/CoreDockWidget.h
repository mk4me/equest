/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   16:34
    filename: CoreDockWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__COREDOCKWIDGET_H__
#define HEADER_GUARD_CORE__COREDOCKWIDGET_H__

#include <coreui/Export.h>
#include <QtWidgets/QDockWidget>

namespace coreUI {

//! Klasa rozszerza klasycznego QDockWidget o możliwosć przełączania trybu oddokowywania na przeciwny,
//! ustawaiania okna jako "niesmiertelnego" przy zamykaniu go oraz modyfikacji jego flag przy oddokowywaniu
class COREUI_EXPORT CoreDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    CoreDockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    CoreDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	//! Destruktor wirtualny
    virtual ~CoreDockWidget();

	//! \param dockWidget
	//! \return Czy DockWidget będzie żył po zamknięciu go
	static const bool isDockWidgetPermanent(const QDockWidget * dockWidget);
	//! \param dockWidget
	//! \param permament Czy DockWidget będzie żył po zamknięciu go
	static void setDockWidgetPermament(QDockWidget * dockWidget, const bool permanent);
	//! \param widget Widget który wrapujemy w CoreDockWidget wraz z naszym titlebarem
	//! \param windowTitle Tytuł naszego okna
	//! \patam allowedAreas Dozwolone obszary widgeta
	//! \param permament Czy DockWidget będzie żył po zamknięciu go
	static CoreDockWidget * embeddWidget(QWidget * widget,
		const QString & windowTitle, const Qt::DockWidgetArea allowedAreas,
		const bool permanent);

public slots:
    //! Zmienia stan dokowania.
    void toggleFloating();

public:
	//! \return Czy DockWidget będzie żył po zamknięciu go
    bool isPermanent() const;
	//! \param permament Czy DockWidget będzie żył po zamknięciu go
    void setPermanent(const bool permanent);
};

}

#endif  //  HEADER_GUARD_CORE__COREDOCKWIDGET_H__

