/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   8:50
	filename: 	NewChartState.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSTATE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSTATE_H__

#include <QtCore/QObject>
#include <utils/SmartPtr.h>

class NewChartVisualizer;
class QwtPlot;
class QEvent;
class QPainter;
class QwtPlotCanvas;

//! Klasa bazowa dla stanów wizualizatora
//! \version 0.9.1
class NewChartState : public QObject
{
    Q_OBJECT
public:
    typedef utils::shared_ptr<QwtPlot> QwtPlotPtr;

public:
    //! kostruktor pobiera obiekt wizualizatora, którym będzie się zarządzało
    NewChartState(NewChartVisualizer* visualizer);
	virtual ~NewChartState() { }

public:
    //! Obsługa eventów Qt
    virtual bool stateEventFilter( QObject *, QEvent * ) = 0;
    //! Metoda odrysowująca stan wizualizatora
    virtual void draw(QPainter *) = 0;
    //! wywoływane, stan staje się aktywny
    virtual void stateBegin() {}
    //! wywoływane, gdy stan przestaje być aktywny
    virtual void stateEnd() {}

protected:
    //! wizualizator, którym będzie się zarządzało
    NewChartVisualizer* visualizer;
    //! Wykres qwt (serce wizualizatora)
    QwtPlot* plot;
    //! płótno wykresu
    QwtPlotCanvas* canvas;
};
DEFINE_SMART_POINTERS(NewChartState);

#endif
