/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   8:54
	filename: 	NewChartValueMarker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTVALUEMARKER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTVALUEMARKER_H__

#include "NewChartState.h"
#include "NewChartMarker.h"
#include "NewChartLabelState.h"

//! stan wizualizatora za etykiety z wartościami
class NewChartValueMarker : public NewChartLabelState
{
public:
    //! Konstruktor
    //! \param visualizer wizualizator, na którym stan będzie operował
    NewChartValueMarker(NewChartVisualizer* visualizer);
    virtual ~NewChartValueMarker() {}

public:
    //! Filtrowanie eventów Qt (głównie myszka) zarządzanie etykietami
    virtual bool stateEventFilter(QObject *, QEvent *);


	//! rozpoczyna stan, zmienia kursor, podpina pod wykres dodatkowy wskaźnik (ułatwiający dodawanie nowych etykiet)
    virtual void stateBegin();
    //! kończy stan, usuwa dodatkowy wskaźnik z wykresu
    virtual void stateEnd();

private:
    //! Wstawienie nowej etykiety
    //! \param point wskazywany punkt
    //! \param color kolor obwódki etykiety
    void insertNewMarker(const QPointF& point, const QColor& color = QColor(Qt::blue));
    //! odświeżenie etykiet
    void updateLabels();

private:
    //! dodatkowy wskaźnik (ułatwiający dodawanie nowych etykiet)
    NewChartMarker marker;
    //! etykieta, która jest obecnie przesuwana (może to być dowolna etykieta)
    LabelDataConstPtr currentLabel;
};
typedef utils::shared_ptr<NewChartValueMarker> NewChartValueMarkerPtr;
typedef utils::shared_ptr<const NewChartValueMarker> NewChartValueMarkerConstPtr;

#endif
