/********************************************************************
	created:	2011/11/22
	created:	22:11:2011   12:30
	filename: 	NewChartMarker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTMARKER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTMARKER_H__

#include <plugins/c3d/C3DChannels.h>

//! Marker pokazujący aktualny czas i wartość krzywej
//! Wykorzystuje już istniejący obiekt z Qwt
//! Dodatkowo wprowadza opóźnienie przesuwania
class NewChartMarker : public QwtPlotMarker
{
public:
    //! konstruktor z podpiętymi danymi
    explicit NewChartMarker(c3dlib::ScalarChannelReaderInterfaceConstPtr reader);
    explicit NewChartMarker();
	virtual ~NewChartMarker() {}

protected:
    //! dziedziczona z QwtPlotMarker. Odrysowanie liniii wskazujących punkt
    virtual void drawLines( QPainter *, const QRectF &, const QPointF & ) const;
    //! dziedziczona z QwtPlotMarker. Odrysowanie etykiety
    virtual void drawLabel( QPainter *, const QRectF &, const QPointF & ) const;

public:
    //! rysuje punkt o określonym rozmiarze
    static void drawDot( QPainter * painter, const QPointF & point, int size = 5 );
    //! resetuje 'ped'. Pozycja etykiety wynika z punktu i przesunięcia
    void resetMomentum() const { positionSet = false; }
    //! ustawia wspoczynniki opóźnienia
    //! \param x współczynnik poziomy
    //! \param y współczynnik pionowy
    void setLerpRatios(float x, float y) { lerpX = x; lerpY = y; }
    //! ustawia jednolity współczynnik opóźnienia
    //! \param lerp współczynnik dla pionu i poziomu
    void setLerpRatios(float lerp) { lerpX = lerp; lerpY = lerp; }

private:
    //! wskazywane dane
	c3dlib::ScalarChannelReaderInterfaceConstPtr reader;
    //! aktualna pozycja etykiety
    mutable QPointF position;
    //! umożliwia opoznione ustawienie pozycji
    mutable bool positionSet;
    //! poziomy wspoczynniki opóźnienia
    float lerpX;
    //! pionowy wspoczynniki opóźnienia
    float lerpY;
};
typedef utils::shared_ptr<NewChartMarker> NewChartMarkerPtr;
typedef utils::shared_ptr<const NewChartMarker> NewChartMarkerConstPtr;


#endif
