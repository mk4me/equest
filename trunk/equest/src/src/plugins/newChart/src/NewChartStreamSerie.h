/********************************************************************
	created:	2015/02/17
	created:	17:2:2011   09:08
	filename: 	NewChartStreamSerie.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSTREAMSERIE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSTREAMSERIE_H__

#include <corelib/IVisualizer.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/c3d/EventSerieBase.h>
#include <plugins/newChart/INewChartSerie.h>
#include "NewChartCurve.h"
#include "NewChartEvents.h"
#include "INewChartSeriePrivate.h"
#include "Scales.h"
#include <QtCore/QVector>

class NewChartCurve;
class NewChartVisualizer;

// Klasa wzięta z przykładu qwt - realtime_plot/incrementalplot.h
class CurveData
{
// A container class for growing data
public:
	CurveData();
	void append(double *x, double *y, int count);
	int count() const;
	int	size() const;
	const double *x() const;
	const double *y() const;
	void clear();
private:
	int d_count;
	QVector<double> d_x;
	QVector<double> d_y;
};

//! Strumieniowa seria danych wizualizatora 2D
class NewChartStreamSerie : public INewChartSeriePrivate, public plugin::IVisualizer::ISerie
{
    friend class NewChartVisualizer;
public:
    //! Konstruktor
    //! \param visualizer wizualzator, który tworzy serie danych
    NewChartStreamSerie(NewChartVisualizer * visualizer);
    virtual ~NewChartStreamSerie();

public:
    //! \return aktualna wartość (dla aktualnego czasu)
    double getCurrentValue() const;
    //! pobierz krzywą qwt
    const QwtPlotCurve* getCurve() const;
    //! pobierz krzywą qwt
    QwtPlotCurve* getCurve();
    //! ustaw widzialność krzywej
    //! \param visible widoczna / niewidoczna
    void setVisible(bool visible);
    //! \return czy krzywa jest widoczna
    bool isVisible() const;
    //! ustawienie koloru krzywej
    //! \param r składowa czerwona (0 - 255)
    //! \param g składowa zielona (0 - 255)
    //! \param b składowa niebieska (0 - 255)
    //! \param a składowa alpha (0 - 255)
    void setColor(int r, int g, int b, int a = 255);
    //! ustawienie koloru krzywej
    //! \param color kolor do ustawienia
    virtual void setColor(const QColor& color);
    //! \return kolor krzywej
    virtual QColor getColor() const;
    //! ustawia szerokość krzywej
    //! \param width nowa szerokość w pikselach
    void setWidth(int width);
    //! ustawia nazwę krzywej
    //! \param name nowa nazwa krzywej
    virtual void setName(const std::string & name);
    //! \return nazwa krzywej
    virtual const std::string getName() const;
    //! dostarcza ObjectWrapper z danymi
    //! \param data ObjectWrapper przechowujący interfejs ScalarChannelReaderInterface
    virtual void setData(const utils::TypeInfo & requestedData, const core::VariantConstPtr & data);
    //! \return ObjectWrapper przechowujący interfejs ScalarChannelReaderInterface
    virtual const core::VariantConstPtr & getData() const;

	virtual const utils::TypeInfo & getRequestedDataType() const;

	virtual void update();

    //! \return ekstrema krzywej
    Scales getScales() const;
    //! \return czy krzywa jest aktywna
    bool isActive() const { return active; }
    //! ustawia aktywność krzywej
    //! \param val aktywna / nieaktywna
    void setActive(bool val);
    //! odlacza krzywa od wykresu
    void removeItemsFromPlot();
    //! ustawia kolejność wyświetlania
    //! \param z
    //! \param replot
    void setZ(double z, bool replot = false);
    //! \return składowa z odpowiedzialna z kolejność wyświetlania
    double z() const;
    //! \return skala w poziomie
    double getXScale() const;
    //! ustawia skale w poziomie dla krzywej
    //! \param val nowa wartość skali
    void setXScale(double val);
    //! \return skala w pionie
    double getYScale() const;
    //! ustawia skale w pionie dla krzywej
    //! \param val nowa wartość skali
    void setYScale(double val);
    //! \return przesunięcie w poziomie
    double getXOffset() const;
    //! ustawia przesunięcie w poziomie
    //! \param val nowa wartość przesunięcia
    void setXOffset(double val);
    //! \return przesunięcie w pionie
    double getYOffset() const;
    //! ustawia przesunięcie w pionie
    //! \param val nowa wartość przesunięcia
    void setYOffset(double val);
    //! \return przesunięcie krzywej
    QPointF getOffset() const;
    //! ustawia przesunięcie
    //! \param offset nowa wartość przesunięcia
    void setOffset(const QPointF& offset);

	virtual void updateData(const core::VariantConstPtr & data);

private:
    //! wizualizator, który stworzył serie
    NewChartVisualizer* visualizer;
    //! nazwa serii
    std::string name;
    //! OW z danymi
    core::VariantConstPtr data;

	//! Obiekt aktualizujący
	threadingUtils::StreamStatusObserverPtr updater;

	utils::TypeInfo requestedType;
    //! wizualizowana krzywa
    NewChartCurve* curve;
    //! czy seria jest aktywna
    bool active;
    //! składowa okresla kolejność rysowania
    double _z;
    //! początkowa wartość składowej zd
    double _zBase;
	ScalarStreamConstPtr scalarStream;
    CurveData curveData;
	double a;
};

#endif
