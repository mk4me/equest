/********************************************************************
	created:  2016/07/06 13:00
	filename: NewChartItemHelper.h
	author:	  Wojciech Knieæ
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN_NEWCHART__NEWCHARTITEMHELPER_H__
#define HEADER_GUARD_PLUGIN_NEWCHART__NEWCHARTITEMHELPER_H__

#include <plugins/newChart/Export.h>
#include <plugins/newChart/INewChartSerie.h>
#include <corelib/Visualizer.h>
#include <corelib/HierarchyHelper.h>

//! klasa pomocnicza przy tworzeniu wizualizatora wykresów
//! klasa musi dostarczyæ ScalarChannelReaderInterface z danymi we wrapperze
class NEWCHART_EXPORT NewChartItemHelper : public core::WrappedItemHelper
{
public:
	NewChartItemHelper(const core::VariantConstPtr& wrapper, const c3dlib::EventsCollectionConstPtr& events = c3dlib::EventsCollectionConstPtr());

public:
	//! Tworzy wizualizator newChart
	virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
	//! zwraca utworzone serie danych
	//! \param visualizer wizualizator, który bêdzie tworzy³ serie
	//! \param path œcie¿ka dla timeline
	//! \param series tutaj trafia stworzone serie 
	virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series);
	//! \return typ ScalarChannelReaderInterface
	std::vector<utils::TypeInfo> getTypeInfos() const;

protected:
	c3dlib::EventsCollectionConstPtr events;
};
typedef utils::shared_ptr<NewChartItemHelper> NewChartItemHelperPtr;
typedef utils::shared_ptr<const NewChartItemHelper> NewChartItemHelperConstPtr;

//! klasa pomocnicza przy tworzeniu wykresów z wektora 3-elementowego
class NEWCHART_EXPORT NewVector3ItemHelper : public core::WrappedItemHelper
{
public:
	//! Konstruktor pobiera obiekt z wrapperem wektora 3D
	//! \param wrapper 
	NewVector3ItemHelper(const core::VariantConstPtr& wrapper,
							const c3dlib::EventsCollectionConstPtr& events = c3dlib::EventsCollectionConstPtr(),
							const QString & xAxisName = QString(),
							const QString & yAxisName = QString(),
							const QString & zAxisName = QString());

public:
	//! zwraca utworzone serie danych
	//! \param visualizer wizualizator, który bêdzie tworzy³ serie
	//! \param path œcie¿ka dla timeline
	//! \param series tutaj trafia stworzone serie
	virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series);
	//! \return wizualizator 2D
	virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
	//! \return typ ScalarChannelReaderInterface
	std::vector<utils::TypeInfo> getTypeInfos() const;
private:
	c3dlib::EventsCollectionConstPtr events;
	const QString xAxisName;
	const QString yAxisName;
	const QString zAxisName;
};
typedef utils::shared_ptr<NewVector3ItemHelper> NewVector3ItemHelperPtr;
typedef utils::shared_ptr<const NewVector3ItemHelper> NewVector3ItemHelperConstPtr;

#endif