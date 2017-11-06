/********************************************************************
	created:	2013/07/01
	created:	1:7:2013   20:03
	filename: 	VisualizerEventFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__VISUALIZEREVENTFILTER_H__
#define HEADER_GUARD_MDE_VIEW__VISUALIZEREVENTFILTER_H__

#include <QtCore/QObject>
#include "ContextEventFilter.h"

//! filtruje zmiany focusów widgetów, obsługuje zdarzenia dla kontekstów, wywołuje przydzielanie kontekstu dla odpowiedniego widgetu
class VisualizerEventFilter : public QObject
{
    Q_OBJECT
public:
    //! konstruktor, przyjmuje na własność główne okno
    //! \param mainWindow potrzebne, aby uzyskać dostęp do kontekstów
    VisualizerEventFilter(ContextEventFilterWeakPtr contextFilter);
    //! destruktor
    virtual ~VisualizerEventFilter() {}

protected:
    //! filtruje zdarzenia dla zarejstrowanych widgetów, 
    //! dzięki tej metodzie wysyłane są informacje, dla jakich widgetów powinien zostać aktywowany kontekst
    //! \param obj 
    //! \param event 
    virtual bool eventFilter(QObject *obj, QEvent *event);

Q_SIGNALS:
    void focusOn(QWidget* w);

private:
    ContextEventFilterWeakPtr contextEventFilter;
};
DEFINE_SMART_POINTERS(VisualizerEventFilter);

#endif
