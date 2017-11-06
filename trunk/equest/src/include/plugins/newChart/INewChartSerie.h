/********************************************************************
	created:	2012/04/30
	created:	30:4:2012   13:19
	filename: 	INewChartSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__INEWCHARTSERIE_H__
#define HEADER_GUARD_NEW_CHART__INEWCHARTSERIE_H__

#include <plugins/c3d/EventSerieBase.h>

class QwtPlotCurve;
class EventsHelper;
class Scales;
//! Niektóre elementy serii danych wizualizatora wykresów są edytowane z zewnątrz, 
//! dlatego należy interfejsować serie danych, aby dać dostęp do obiektu bez konieczności linkowania się z pluginem
//! taką funkcjonalność w obrębie serii zapewnia właśnie ten interfejs
class INewChartSerie
{
public:
	//! destruktor
	virtual ~INewChartSerie() {}

public:
    //! pobiera kolor serii
    virtual QColor getColor() const = 0;
    //! ustawia kolor serii
    //! \param color kolor w formacie Qt
    virtual void setColor(const QColor& color) = 0;


};
typedef utils::shared_ptr<INewChartSerie> INewChartSeriePtr;
typedef utils::shared_ptr<const INewChartSerie> INewChartSerieConstPtr;


#endif
