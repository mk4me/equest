/********************************************************************
	created:	2012/05/28
	created:	28:5:2012   10:19
	filename: 	NewChartEventStateData.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTEVENTSTATEDATA_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTEVENTSTATEDATA_H__

#include <plugins/c3d/C3DChannels.h>

//! Klasa dostarcza danych do wizualizacji kawałka krzywej związanego z eventem
//! \version 0.9.1
class NewChartEventStateData : public QwtSeriesData<QPointF>
{
public:
    //! Konstruktor wymaga zasilenia obiektu w dane
    //! \param channel DataChannel z danymi dla krzywej
    //! \param startTime początkowy czas dla eventu
    //! \param endTime końcowy czas dla eventu
	NewChartEventStateData(c3dlib::ScalarChannelReaderInterfaceConstPtr channel, std::size_t startTimeIdx, std::size_t endTimeIdx);

public:
    //! \return liczba próbek przedziału
    virtual size_t size() const;
    //! Zwraca próbkę z przedziału eventa
    //! \param i indeks próbki
    //! \return wartość próbki
    virtual QPointF sample( size_t i ) const;
    //! Obszar otaczający krzywą
    virtual QRectF boundingRect() const;

private:
    //! DataChannel z danymi dla krzywej
	const c3dlib::ScalarChannelReaderInterfaceConstPtr channel;
    //! indeks początkowego czas dla eventu
    const int startIndex;
    //! indeks końcowego czas dla eventu
    const int endIndex;
	//! Zakres
	const QRectF boundingRect_;
};

#endif
