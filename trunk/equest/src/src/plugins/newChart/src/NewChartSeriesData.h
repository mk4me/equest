/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:14
	filename: 	NewChartSeriesData.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSERIESDATA_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSERIESDATA_H__

#include <plugins/c3d/C3DChannels.h>

//! Klasa rozszerza podstawowy kontener próbek z Qwt
//! dzięki niej można zasilić wykres bez konieczności kopiowania danych
//! Dodatkowo dodana została możliwość transformacji wykresu (offset + skala)
class NewChartSeriesData : public QwtSeriesData<QPointF>
{
public:
    //! Konstruktor, trzeba dostarczyć dane do wykresu
    //! \param channel DataChannel, będzie przechowywany na własność
    //! \param offset początkowe przesunięcie (domyślnie = (0,0))
    //! \param scaleX początkowa skala w poziomie (domyślnie 1)
    //! \param scaleY początkowa skala w pionie (domyślnie 1)
	NewChartSeriesData(c3dlib::ScalarChannelReaderInterfaceConstPtr channel,
        QPointF offset = QPointF(), double scaleX = 1.0, double scaleY = 1.0);
	~NewChartSeriesData();

public:
    //! \return liczba próbek
    virtual size_t size() const;
    //! Konwertuje próbkę z DataChannel na format obsługiwany przez Qwt
    //! \param i numer próbki
    //! \return wartość konkretnej próbki
    virtual QPointF sample( size_t i ) const;
    //! \return wielkość obszaru zajmowanego przez krzywa
    virtual QRectF boundingRect() const;

public:
    //! \return przesunięcie wykresu
    QPointF getOffset() const { return offset; }
    //! ustawia przesunięcie wykresu
    //! \param val nowe przesunięcie
    void setOffset(QPointF val) { offset = val; }
    //! \return skala dla wartości
    double getScaleY() const { return scaleY; }
    //! ustawia skalę dla wartości
    //! \param val nowa skala, nie może być równa 0
    void setScaleY(double val) { scaleY = val; }
    //! \return skala dla argumentów
    double getScaleX() const { return scaleX; }
    //! ustawia skalę dla argumentów
    //! \param val nowa skala, nie może być równa 0
    void setScaleX(double val) { scaleX = val; }

private:
    //! DataChannel z danymi
	c3dlib::ScalarChannelReaderInterfaceConstPtr channel;
    //! ustawione przesunięcie
    QPointF offset;
    //! ustawiona skala pozioma
    double scaleX;
    //! ustawiona skala pionowa
    double scaleY;
};



#endif
