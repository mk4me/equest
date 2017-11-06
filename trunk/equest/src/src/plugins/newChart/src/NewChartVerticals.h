/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   8:56
	filename: 	NewChartVerticals.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTVERTICALS_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTVERTICALS_H__

//#include <qwt/qwt_plot_curve.h>
#include "NewChartState.h"
#include "NewChartLabel.h"
#include "NewChartMarker.h"
#include "NewChartLabelState.h"

//! Stan wizualizatora odpowiadający za etykiety roznicowe
class NewChartVerticals : public NewChartLabelState
{
public:
    //! Konstruktor
    //! \param visualizer wizualizator, na którym stan będzie operował
    //! \param style styl połączeń między wskazywanymi przez etykietę punktami
	NewChartVerticals(NewChartVisualizer* visualizer, NewChartVerticalsConnection::ConnectionStyle style);
    virtual ~NewChartVerticals() {}

public:
    //! Filtrowanie eventów Qt (głównie myszka) zarządzanie etykietami
    virtual bool stateEventFilter(QObject *, QEvent *);
    //! rozpoczyna stan, zmienia kursor, podpina pod wykres dodatkowy wskaźnik (ułatwiający dodawanie nowych etykiet)
    virtual void stateBegin();
    //! kończy stan, usuwa dodatkowy wskaźnik z wykresu
    virtual void stateEnd();

private:
    //! Wstawienie nowej etykiety
    //! \param point1 pierwszy wskazywany punkt
    //! \param point2 drugi wskazywany punkt
    //! \param currentSerie seria, z która jest zwiazana etykieta
    //! \param color kolor obwódki etykiety
	void insertNewMarker(const QPointF& point1, const QPointF& point2, const INewChartSeriePrivate* currentSerie, const QColor& color = QColor(Qt::blue));
    //! odświeżenie etykiet
    void updateLabels();

private:
    //! dodatkowy wskaźnik (ułatwiający dodawanie nowych etykiet)
    NewChartLabelPtr labelMarker;
    //! etykieta, która jest obecnie przesuwana (może to być dowolna etykieta)
    LabelDataConstPtr currentLabel;
    //! wybrany, pierwszy punkt
	utils::shared_ptr<std::pair<const INewChartSeriePrivate*, QPointF>> point1;
    //! styl połączenia (pionowe, poziome, proste)
	NewChartVerticalsConnection::ConnectionStyle style;
    //! marker obrazujący pierwszy wskazywany punkt 
    NewChartMarker marker;
};
typedef utils::shared_ptr<NewChartVerticals> NewChartVerticalsPtr;
typedef utils::shared_ptr<const NewChartVerticals> NewChartVerticalsConstPtr;


#endif
