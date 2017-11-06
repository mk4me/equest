/********************************************************************
	created:	2012/06/04
	created:	4:6:2012   20:00
	filename: 	ContextEventFilter.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__CONTEXTEVENTFILTER_H__
#define HEADER_GUARD_MDE_VIEW__CONTEXTEVENTFILTER_H__

#include <set>
#include <QtWidgets/QWidget>
#include <QtCore/QObject>
#include <utils/SmartPtr.h>

namespace coreUI {
class IAppUsageContextManager;
}
//! filtruje zmiany focusów widgetów, obsługuje zdarzenia dla kontekstów, wywołuje przydzielanie kontekstu dla odpowiedniego widgetu
class ContextEventFilter : public QObject
{
    Q_OBJECT
public:
    //! konstruktor, przyjmuje na własność główne okno
    //! \param mainWindow potrzebne, aby uzyskać dostęp do kontekstów
    ContextEventFilter(coreUI::IAppUsageContextManager * mainWindow);
    //! destruktor
    virtual ~ContextEventFilter() {}

public:
    //! rejestruje widget, któremu przekazywany będzie focus
    //! \param widget  widget, który nie zostanie zniszczony
    void registerPermamentContextWidget(QWidget * widget);
    //! rejestruje widget, któremu przekazywany będzie focus
    //! \param widget widget, który trzeba wyrejstrować z systemu w przypadku jego zamknięcia 
    void registerClosableContextWidget(QWidget * widget);
    //! usuwa widget z kolekcji
    //! \param widget usuwany widget
    void unregisterPermamentContextWidget(QWidget * widget);
    //! usuwa widget z kolekcji
    //! \param widget usuwany widget
    void unregisterClosableContextWidget(QWidget * widget);

public:
    //! filtruje zdarzenia dla zarejstrowanych widgetów, 
    //! dzięki tej metodzie wysyłane są informacje, dla jakich widgetów powinien zostać aktywowany kontekst
    //! \param obj 
    //! \param event 
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    //! 
    coreUI::IAppUsageContextManager* mainWindow;
    //! 
    std::set<QWidget*> permamentWidgets;
    //! 
    std::set<QWidget*> closableWidgets;
};
DEFINE_SMART_POINTERS(ContextEventFilter);

#endif
