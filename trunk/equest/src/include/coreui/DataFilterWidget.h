/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   10:20
	filename: 	DataFilterWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__DATAFILTERWIDGET_H__
#define HEADER_GUARD_COREUI__DATAFILTERWIDGET_H__

#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <functional>
#include <osg/Vec3>
#include <plugins/subject/SubjectDataFilters.h>
#include <corelib/IFilterCommand.h>
#include <coreui/FilterEntryWidget.h>
#include <coreui/Export.h>

namespace Ui {
    class FilterWidget;
}

namespace coreUI {

class COREUI_EXPORT BundleFilter : public core::IFilterCommand
{
public:
    virtual ~BundleFilter() {}

public:
    virtual core::IHierarchyItemPtr getFilteredTree( core::IHierarchyItemConstPtr root );
    virtual QString getName() const;
    virtual QIcon getIcon() const;
    virtual void addFilterCommand(core::IFilterCommandPtr command);

private:
    std::vector<core::IFilterCommandPtr> commands;
};
DEFINE_SMART_POINTERS(BundleFilter)

//! widget przechowujący grupę filtrów
class COREUI_EXPORT DataFilterWidget : public QWidget
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param name nazwa GUI
    //! \param icon ikona
    //! \param hmmWindow widok HMM
    DataFilterWidget(const QString& name, const QIcon& icon);
	virtual ~DataFilterWidget() {}

public:
    //! Dodaje nowy filtr do grupy
    //! \param bigLabelText duża etykieta dla tekstu
    //! \param command akcja filtrująca dane trafiające do drzewa
    //! \param icon ikona filtru
    void addFilter(const QString& bigLabelText, core::IFilterCommandPtr command, const QIcon& icon = QIcon());
    //!  Dodaje nowy filtr do grupy
    //! \param entry gotowy, przygotowany wcześniej filtr
    void addFilter(FilterEntryWidget* entry);
    //! zamyka możliwość dodawania kolejnych filtrów do grupy
    void closeFilters();
    //! \return czy dana grupa jest aktualnie aktywna?
    bool getActive() const { return active; }
    //! ustawia aktywność grupy
    //! \param val aktywuje / deaktywuje grupę
    void setActive(bool val);
    //! \return obrazek reprezentujacy grupę
    const QPixmap& getPixmap() const;
    //! \return nazwa grupy filtrów
    QString getName() const;
    //! usawia kolor grupy filtrów
    //! \param color nowy kolor
    void setColor(const QColor& color);
    //! \return liczba filtrów w grupie
    int getNumEntries() const;
    //! zwraca filtr 
    //! \param index ineks filtru w grupie
    //! \return filtr przydzielony do grupy
    const FilterEntryWidget* getEntry(int index) const;

protected:
    //! obsługuje zdarzenia kliknięcia myszki i wywołuje odpowiednie filtry
    //! \param object 
    //! \param event 
    bool eventFilter(QObject *object, QEvent *event);

Q_SIGNALS:
    //! zmieniła się aktywność grupy
    //! \param grupa
    void activityChanged(coreUI::DataFilterWidget*);
    //! grupa została kliknięta
    void clicked();
    //! sygnał informujący, że filtr został kliknięty
    //! \param filter kliknięty filtr
    void onFilterClicked(core::IFilterCommandPtr filter);

public:
    //! obsługuje zdarzenia kliknięcia myszki i wywołuje odpowiednie filtry
    //! \param e 
    virtual void mousePressEvent(QMouseEvent *e);
    //! deaktywuje filtry w grupie
    //! \param toSkip filtr, który powinien zostać pominięty
    void uncheckEntries(FilterEntryWidget* toSkip = nullptr);

public Q_SLOTS:
    //! resetuje ustawienia filtrów (zmienione np. przez konfiguratory)
    void resetFilters();

private Q_SLOTS:
    //! kliknięto grupę filtrów
    void onClick();
    void onFilterEntryClicked(core::IFilterCommandPtr filter);

private:
    //! czy grupa jest aktywna
    bool active;
    //! czy do grupy można jeszcze dodać następne filtry
    bool filtersClosed;
    //! filtry zawarte w grupie
    std::vector<FilterEntryWidget*> entries;
    Ui::FilterWidget* ui;
    BundleFilterPtr bundleFilter;
};
typedef utils::shared_ptr<DataFilterWidget> DataFilterWidgetPtr;
typedef utils::shared_ptr<const DataFilterWidget> DataFilterWidgetConstPtr;

}

#endif
