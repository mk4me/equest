/********************************************************************
	created:	2012/04/30
	created:	30:4:2012   13:19
	filename: 	INewChartVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__INEWCHARTVISUALIZER_H__
#define HEADER_GUARD_NEW_CHART__INEWCHARTVISUALIZER_H__

#include <corelib/IVisualizer.h>

#include <c3dlib/C3DTypes.h>
#include <QtCore/QVariant>
#include <plugins/newChart/INewChartCurveGenerator.h>


//! niektóre elementy wizualizatora wykresów są edytowane z zewnątrz, 
//! dlatego należy interfejsować serie danych, aby dać dostęp do obiektu bez konieczności linkowania się z pluginem
//! taką funkcjonalność w obrębie wizualizatora zapewnia właśnie ten interfejs
//! \version 0.9.1
class INewChartVisualizer : public plugin::IVisualizer
{
public:

	enum Axis
	{
		AxisX,
		AxisY
	};

public:
	virtual ~INewChartVisualizer() {}

public:
    //! Ustawia tytuł całego wykresu
    //! \param title nowy tytuł
    virtual void setTitle( const QString& title ) = 0;
    //! \return tytuł wykresu 
    virtual QString getTitle() const = 0;
	//! \param axis Typ osi
	//! \param min Minimalna wartość osi
	//! \param max Maksymalna wartość osi
	//! \param steps Ilość pośrednich podziałek
	virtual void setAxisScale(const Axis axis, const double min, const double max, unsigned int steps = 10) = 0;

	//TODO : gdzie to przeniesc? osobny serwis to przerost formy nad trescia.
	static void addGenerator(INewChartCurveGeneratorConstPtr generator)
	{
		generators.push_back(generator);
	}
protected:
	static std::vector<INewChartCurveGeneratorConstPtr> generators;
};
typedef utils::shared_ptr<INewChartVisualizer> INewChartVisualizerPtr;
typedef utils::shared_ptr<const INewChartVisualizer> INewChartVisualizerConstPtr;


#endif
