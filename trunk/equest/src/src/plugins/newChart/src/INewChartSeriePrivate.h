/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   18:08
	filename: 	ChartSerie.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__INEWCHARTSERIEPRIVATE_H__
#define HEADER_GUARD_NEW_CHART__INEWCHARTSERIEPRIVATE_H__

#include <corelib/IVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <plugins/newChart/INewChartSerie.h>
#include "NewChartEvents.h"
#include "Scales.h"
#include <corelib/IVisualizer.h>

class NewChartCurve;
class NewChartVisualizer;

//! Podstawowa seria danych wizualizatora 2D
class INewChartSeriePrivate : public INewChartSerie
{
public:
    //! pobierz krzywą qwt
    virtual const QwtPlotCurve* getCurve() const = 0;
   //! pobierz krzywą qwt
    virtual QwtPlotCurve* getCurve() = 0;
   //! ustaw widzialność krzywej
   //! \param visible widoczna / niewidoczna
    virtual void setVisible(bool visible) = 0;
   //! \return czy krzywa jest widoczna
    virtual bool isVisible() const = 0;
   //! \return obiekt ułatwiający zarządzanie eventami
    virtual EventsHelperPtr getEventsHelper() const { return EventsHelperPtr(); }
    virtual bool isActive() const = 0;
	//! ustawia aktywność krzywej
	//! \param val aktywna / nieaktywna
    virtual void setActive(bool val) = 0;
    virtual Scales getScales() const = 0;
    //! ustawia szerokość krzywej
    //! \param width nowa szerokość w pikselach
    virtual void setWidth(int width) = 0;
    //! odlacza krzywa od wykresu
    virtual void removeItemsFromPlot() = 0;
    //! ustawia kolejność wyświetlania
    //! \param z
    //! \param replot
    virtual void setZ(double z, bool replot = false)= 0;
    //! \return składowa z odpowiedzialna z kolejność wyświetlania
    virtual double z() const= 0;

    const plugin::IVisualizer::ISerie* asISerie() const { return dynamic_cast<const plugin::IVisualizer::ISerie*>(this); }
    plugin::IVisualizer::ISerie* asISerie()  { return dynamic_cast<plugin::IVisualizer::ISerie*>(this); }

	//! \return skala w poziomie
	virtual double getXScale() const = 0;
	//! ustawia skale w poziomie dla krzywej
	//! \param val nowa wartość skali
	virtual void setXScale(double val) = 0;
	//! \return skala w pionie
	virtual double getYScale() const = 0;
	//! ustawia skale w pionie dla krzywej
	//! \param val nowa wartość skali
	virtual void setYScale(double val) = 0;
	//! \return przesunięcie w poziomie
	virtual double getXOffset() const = 0;
	//! ustawia przesunięcie w poziomie
	//! \param val nowa wartość przesunięcia
	virtual void setXOffset(double val) = 0;
	//! \return przesunięcie w pionie
	virtual double getYOffset() const = 0;
	//! ustawia przesunięcie w pionie
	//! \param val nowa wartość przesunięcia
	virtual void setYOffset(double val) = 0;
	//! \return przesunięcie krzywej
	virtual QPointF getOffset() const = 0;
	//! ustawia przesunięcie
	//! \param offset nowa wartość przesunięcia
	virtual void setOffset(const QPointF& offset) = 0;

	virtual void updateData(const core::VariantConstPtr & data) = 0;
};

#endif
