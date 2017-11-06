/********************************************************************
	created:	2011/09/19
	created:	19:9:2011   14:55
	filename: 	CoreDockWidgetSet.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__DOCKWIDGETSET_H__
#define HEADER_GUARD_CORE__DOCKWIDGETSET_H__

#include <coreui/Export.h>
#include <boost/range.hpp>
#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>

class QDockWidget;

namespace coreUI {	

//! Klasa obsługuje grupę dock widgetów, sama będąc dock widgetem
class COREUI_EXPORT CoreDockWidgetSet : public QWidget
{
	Q_OBJECT;

public:
    //! niemodyfikowalny zakres wszystkich podległych widgetów zakładki
    typedef boost::iterator_range<std::list<QDockWidget*>::const_iterator> const_range;

public:
	//! Konstruktor
	//! \param title wyświetlana w tabie nazwa grupy
	//! \param parent 
	//! \param flags 
	explicit CoreDockWidgetSet(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	//! Typowy konstruktor Qt
	//! \param parent 
	//! \param flags 
	explicit CoreDockWidgetSet(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	//! Desturktor wirtualny
	virtual ~CoreDockWidgetSet();

public:
    //! Dodaje dock widget do grupy, jeśli nie ma na niego miejsca rzucany jest wyjątek
    //! \param widget dodawany dock widget
    //! \param area obszar okna (lewy, prawy, góra, dol)
    //! \param orientation orientacja (dzielenie w pionie lub poziomie)
    void addDockWidget(QDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation);
    //! Dodaje dock widget do grupy, jeśli nie ma na niego miejsca rzucany jest wyjątek
    //! \param widget dodawany dock widget
    //! \param orientation orientacja (dzielenie w pionie lub poziomie)
	void addDockWidget(QDockWidget* widget, Qt::Orientation orientation);
    //! Dodaje dock widget do grupy, jeśli nie ma na niego miejsca rzucany jest wyjątek
    //! \param widget dodawany dock widget
    void addDockWidget(QDockWidget* widget);
	//! \param widget Widget który usuwamy, za jego zniszczenie odpowiedzilany jest ten kto usuwa
	void removeDockWidget(QDockWidget * widget);
	//! \return czy można jeszcze dodać dock widget do grupy
	bool isAdditionPossible() const;
	//! blokuje lub odblokowuje możliwość dodawania widgetów
	//! \param additionPossible można / nie można dodać nowego widgeta
	void blockAddition(bool additionPossible);
	//! \return liczba dock widgetów w grupie
	int getNumWidgets() const;
	//! \return maksymalna liczba dock widgetów w grupie
	int getMaxWidgetsNumber() const;
	//! ustawia maksymalna liczbe dock widgetów w grupie
	//! \param val maksymalna liczbe dock widgetów
	void setMaxWidgetsNumber(int val);
    //! \return const range wszystkich dock widgetów w grupie
    const_range getDockWidgets() const;

signals:
	//! Sygnal z prosba o zamkniecie obiektu
	void widgetSetChange(unsigned int numberOfWidgets);

private Q_SLOTS:
    //! dock widget podpięty pod grupę został zamknięty
    //! \param object 
    void onDockWidgetClosed(QObject* object);
    //! dock widget podpięty pod grupę zmienił pozycję
    //! \param area 
    void onDockWidgetLocationChanged(Qt::DockWidgetArea area);

protected:

	virtual void closeEvent(QCloseEvent *event);

private:
    //! kontener dodawanych dock widgetów
	QMainWindow* mainWindow;
    //! dodane do grupy dock widgety
	std::list<QDockWidget*> widgetsList;
    //! maksymalna liczba dock widgetów w grupie
	int maxWidgetsNumber;
    //! czy możliwe jest dalsze dodawanie widgetów (niezalezne od maksymalnej liczby dock widgetów)
	bool additionPossible;
};

}

#endif
