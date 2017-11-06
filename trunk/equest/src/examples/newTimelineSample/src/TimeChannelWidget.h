/********************************************************************
    created:  2011/04/06
    created:  6:4:2011   10:10
    filename: TimeChannelWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMECHANNELWIDGET_H__
#define HEADER_GUARD_TIMELINE__TIMECHANNELWIDGET_H__

#include <utils/Debug.h>
#include <QtWidgets/QFrame>

//! Klasa reprezentujaca pojedynczy kanal w czasie - rysuje jego zakres na osi ze wzgledu na parametry begin i end,
//! które stanowia znormalizowane wartosci. Root zajmuje zawsze caly dostepny kanal, jego dlugosc jest punktem 
//! odniesienia dla pozostalych kanalow. Zakres poczatku i konca to [0;1] gdzie poczatek musi byc przed lub rowno z koncem
class TimeChannelWidget : //public QWidget 
    public QFrame 
{
	
	Q_OBJECT;
		
private:

    //! Znormalizowany poczatek kanalu w czasie
    double begin;
    //! \return
        
    //! Znormalizowany koniec kanalu w czasie
    double end;

    //! Znormalizowany aktualny czas kanalu
    double current;

    //! Znormalizowana polowa szerokosci dla aktualnego czasu
    double currentTimeHalfWidth;
	
public:
                
    TimeChannelWidget(QWidget* parent = 0, Qt::WindowFlags f = 0, double begin = 0, double end = 0, double current = 0);

    //! Zwraca znormalizowany poczatek kanalu 
    double getBegin() const;

    //! \param begin Znormalizowany poczatek kanalu - musi byc mniejszy lub rowny koncowi
    void setBegin(double begin);

    //! \return Znormalizowany koniec kanalu
    double getEnd() const;

    //! \param end Znormalizowany koniec kanalu - musi byc wiekszy lub rowny poczatkowi
    void setEnd(double end);

    //! \return Aktualny znormalizowany czas w kanale
    double getCurrent() const;

    //! \param current Znormalizowany aktualny czas w kanale
    void setCurrent(double current);

    //! \param begin begin Znormalizowany poczatek kanalu - musi byc mniejszy lub rowny koncowi
    //! \param end Znormalizowany koniec kanalu - musi byc wiekszy lub rowny poczatkowi
    void setRange(double begin, double end);

public slots:
    //! aktualizacja begina
    void beginUpdate(double begin);

    //! aktualizacja enda
    void endUpdate(double end);

protected:

    virtual void paintEvent(QPaintEvent * pEvent);
};

#endif
