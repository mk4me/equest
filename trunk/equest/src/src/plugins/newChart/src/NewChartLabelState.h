/********************************************************************
	created:	2011/11/28
	created:	28:11:2011   22:37
	filename: 	NewChartLabelState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLABELSTATE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLABELSTATE_H__

#include <boost/tuple/tuple.hpp>
#include "NewChartState.h"

class NewChartSerie;
class NewChartDot;
class NewChartLabel;
class INewChartSeriePrivate;

//! Stan wykresu, który operuje w jakis sposób na etykietach
class NewChartLabelState : public NewChartState
{
public:
    //! Konstruktor pobiera wizualizator, na którym stan będzie operował
    NewChartLabelState(NewChartVisualizer* visualizer);
	virtual ~NewChartLabelState() {}

public:
    //! w tej klasie metoda jeszcze nie jest wykorzystywana
    virtual void draw(QPainter *);
    //! ustawienie stanu, zmiana kursora
    virtual void stateBegin();
    //! zakończenie stanu, nieobsługiwane
    virtual void stateEnd();
    //! usuwa etykiety z wykresu związane z konkretną seria
    //! \param serie seria, dla której usuwamy
    void removeSerieLabels(const INewChartSeriePrivate* serie);
    //! zarządza widocznością etykiet z wykresu zwiazanych z konkretną seria
    //! \param serie seria, dla której zmieniamy widoczność elementów
    //! \param visible widoczny / ukryty
    void setVisible(const INewChartSeriePrivate* serie, bool visible);

protected:
    //! prosta struktura zawierająca informacje o etykiecie
    struct LabelData
    {
        //! obiekt etykiety
        NewChartLabel* label;
        //! pierwsze połącznie etykiety 
        NewChartDot* dot1;
        //! drugie połączenie etykiety (może być puste)
        NewChartDot* dot2;
        //! seria zwiazana z etykietą
        const INewChartSeriePrivate* serie;
    };
    DEFINE_SMART_POINTERS(LabelData);

	typedef boost::tuple<const INewChartSeriePrivate*, QPointF, double> SeriePointDist;

protected:
    //! Realizuje przesunięcie pojedynczej etykiety
    //! \param pos nowa pozycja
    //! \param curve krzywa pod która jest podpięta etykieta
    //! \param label przesuwana etykieta
    void move(const QPoint& pos, const QwtPlotCurve* curve, NewChartLabel* label);
	//! usuwa etykietę z wykresu 
	void removeLabel(LabelDataPtr data);
    //! próbuje pobrać dane etykiety
    //! \param pos pozycja na wykresie (współrzędne ekranowe)
    //! \return dane etykiety lub pusty wskaźnik
    LabelDataConstPtr getLabel(const QPoint& pos);
    //! próbuje pobrać dane etykiety
    //! \param pos pozycja na wykresie (współrzędne ekranowe)
    //! \param curve krzywa zwiazana z etykietą
    //! \return dane etykiety lub pusty wskaźnik
    LabelDataConstPtr getLabel(const QPoint& pos, const QwtPlotCurve* curve);
    //! próbuje pobrać dane etykiety
    //! \param label etykieta, dla której pobierane są dane
    //! \return dane etykiety lub pusty wskaźnik
    LabelDataConstPtr getLabel(const NewChartLabel* label) const;
    //! Pobiera najbliższy punkt od klikniecia
    //! \param pos pozycja klikniecia
    //! \return krotka <najbliższa seria, punkt na niej, odległość od punktu klikniecia>
    SeriePointDist getClosestPoint(const QPoint& pos) const;

protected:
    //! wszystkie etykiety związane ze stanem wizualizatora
    std::vector<LabelDataPtr> labels;

};
DEFINE_SMART_POINTERS(NewChartLabelState);


#endif
