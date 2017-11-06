/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:14
	filename: 	NewChartCurve.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTCURVE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTCURVE_H__

#include <qwt_plot_curve.h>

class NewChartSeriesData;

//! Klasa rozszerza podstawowa funkcjonalność Qwt przede wszystkim o możliwość transformacji (przesunięcie + skala)
//! pamiętać należy, ze nie jest to QObject, dlatego trzeba uwazac przy zarządzaniu pamiecia
//! \version 0.9.1
class NewChartCurve : public QwtPlotCurve
{
public:
    //! Konstruktor ustawia domyślne wartości (offset = 0, skala = 1)
    //! \param title tytuł wykresu
    explicit NewChartCurve( const QString &title = QString::null );

public:
    //! \return skala krzywej dla argumentów
    double getXScale() const { return xScale; }
    //! ustawia skalę krzywej dla argumentów, krzywa musi istnieć
    //! \param val nowa skala
    void setXScale(double val);
    //! \return skala krzywej dla wartości
    double getYScale() const { return yScale; }
    //! ustawia skalę krzywej dla wartości, krzywa musi istnieć
    //! \param val nowa skala
    void setYScale(double val);
    //! \return przesunięcie krzywej dla argumentów
    double getXOffset() const { return xOffset; }
    //! ustawia przesunięcie, krzywa musi istnieć
    //! \param val przesunięcie krzywej dla argumentów
    void setXOffset(double val);
    //! ustawia przesunięcie, krzywa musi istnieć
    //! \param point nowe przesunięcie krzywej 
    void setOffset(const QPointF& point);
    //! \return pobiera przesunięcie krzywej, krzywa musi istnieć
    QPointF getOffset() const;
    //! \return przesunięcie krzywej dla wartości
    double getYOffset() const { return yOffset; }
    //! ustawia przesunięcie, krzywa musi istnieć
    //! \param val przesunięcie krzywej dla wartości
    void setYOffset(double val);
    //! Zasilenie krzywej w dane
    //! \param data Obiekt z danymi, stworzony na porzeby krzywej, przykrywa DataChannel
    void setSamples(NewChartSeriesData* data);

protected:
    //! jawne wskazanie, której metody użyć (usunięcie warn. 'via dominance')
    virtual void dataChanged() { QwtPlotSeriesItem::dataChanged(); }
    //! jawne wskazanie, której metody użyć (usunięcie warn. 'via dominance')
    virtual void setRectOfInterest( const QRectF& r ) { QwtSeriesStore<QPointF>::setRectOfInterest(r); }
    //! jawne wskazanie, której metody użyć (usunięcie warn. 'via dominance')
    virtual size_t dataSize() const { return QwtSeriesStore<QPointF>::dataSize(); }
    //! jawne wskazanie, której metody użyć (usunięcie warn. 'via dominance')
    virtual QRectF dataRect() const { return QwtSeriesStore<QPointF>::dataRect(); }

private:
    //! przesunięcie w poziomie
    double xOffset;
    //! przesunięcie w pionie
    double yOffset;
    //! skala w poziomie
    double xScale;
    //! skala w pionie
    double yScale;
};

#endif
