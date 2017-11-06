
/********************************************************************
	created:	2011/11/18
	created:	18:11:2011   11:21
	filename: 	NewChartPicker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART_VISUALIZER__NEWCHARTPICKER_H__
#define HEADER_GUARD_NEW_CHART_VISUALIZER__NEWCHARTPICKER_H__

#include <QtCore/QObject>

class QPoint;
class QCustomEvent;
class QwtPlot;
class QwtPlotItem;

#include "NewChartSerie.h"
#include "NewChartState.h"

//! klasa reprezentuje stan wizualizatora pozwalający wybor aktywnej serii danych
class NewChartPicker: public NewChartState
{
    Q_OBJECT
public:
    //! Konstruktor pobiera wizualizator, na którym stan będzie operował
    NewChartPicker( NewChartVisualizer* visualizer );
	virtual ~NewChartPicker() {}

public:
    //! filtrowanie eventów Qt, obsługa myszki
    virtual bool stateEventFilter( QObject *, QEvent * );
    //! picker nie musi się odrysowywać
    virtual void draw(QPainter* painter) {} 
    //! rozpoczęcie stanu, zmiana kursora
    virtual void stateBegin();
    //! nic nie trzeba robic po zakończeniu stanu
    virtual void stateEnd() {}
    //! \return tolerancja w pikselach wg której zaliczane jest klikniecie na krzywa
    int getPixelTolerance() const { return pixelTolerance; }
    //! ustawia tolerancje wg której zaliczane jest klikniecie na krzywa
    //! \param val nowa tolerancja w pikselach
    void setPixelTolerance(int val) { pixelTolerance = val; }
    //! ustawia aktualnie aktywna krzywa, klikniecia na nia beda ignorowane
    //! \param curve aktualnie aktywna krzywa
    void setCurrentCurve(QwtPlotCurve * curve);

signals:
    //! sygnał emitowany, gdy kliknięta zostanie krzywa
    //! \param curve kliknięta krzywa
    void serieSelected(QwtPlotItem* curve);

private:
    //! metoda obsługuje kliknięcie w wykres, sprawdza, czy kliknięto w krzywą i ewentualnie ja zaznacza
    //! \param pos kliknięta pozycja
    bool select( const QPoint & pos);

private:
    //! płótno wykresu
    //QwtPlotCanvas* canvas;
    //! tolerancja, czyli maksymalna odległość od krzywej zaliczająca kliknięcie w nią
    int pixelTolerance;
    //! aktualnie wybrana krzywa, kliknięcie w nią będzie ignorowane
    QwtPlotCurve* currentCurve;
    //! flaga określająca, czy kursor się porusza, jeśli tak, to nie można wykonać wszystkich akcji pickera
    bool move;
};
typedef utils::shared_ptr<NewChartPicker> NewChartPickerPtr;
typedef utils::shared_ptr<const NewChartPicker> NewChartPickerConstPtr;

#endif
