/********************************************************************
	created:	2012/05/28
	created:	28:5:2012   21:10
	filename: 	NewChartDotFixed.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTDOTFIXED_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTDOTFIXED_H__

#include <plugins/c3d/C3DChannels.h>

//! punkt zaczepiony na wykresie, stała pozycja nie zmienia się wraz z wykresem
class NewChartDotFixed : public NewChartDot
{
public:
    //! Konstruktor
    //! \param position pozycja na wykresie
    //! \param size wielkość punktu
    NewChartDotFixed(const QPointF& position, int size = 2) :
        NewChartDot(size),
        position(position)
    {
    }

public:
    //! \return pozycja na wykresie
    virtual QPointF getPosition() const { return position; }
    //! Ustawia nową pozycje na wykresie
    virtual void setPosition(const QPointF& val) { position = val; }

private:
    //! pozycja na wykresie
    QPointF position;
};

#endif
