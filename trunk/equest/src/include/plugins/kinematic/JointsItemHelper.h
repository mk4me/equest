/********************************************************************
	created:  2016/07/06 20:49
	filename: JointsItemHelper.h
	author:	  Wojciech Knieæ
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN_KINEMATIC__JOINTSITEMHELPER_H__
#define HEADER_GUARD_PLUGIN_KINEMATIC__JOINTSITEMHELPER_H__

#include <plugins/kinematic/Export.h>
//#include <QtCore/QString>
//#include <QtWidgets/QTreeWidgetItem>
#include <plugins/subject/IMotion.h>
//#include <plugins/newChart/INewChartSerie.h>
#include <corelib/Visualizer.h>
#include <corelib/HierarchyHelper.h>
#include <plugins/hmmlib/MultiserieColorStrategy.h>
#include <plugins/hmmlib/Export.h>
#include <plugins/newChart/NewChartItemHelper.h>

//! klasa pomocnicza przy tworzeniu wizualizatora jointów (leniwe parsowanie)
class KINEMATIC_EXPORT JointsItemHelper : public core::HierarchyHelper
{
public:
	//! Konstruktor
	//! \param motion motion, z którego beda wyciagane elementy do stworzenia JointAnglesCollection
	JointsItemHelper(core::VariantConstPtr skeletonWithStates);

public:
	//! zwraca utworzone serie danych
	//! \param visualizer wizualizator, który bêdzie tworzy³ serie
	//! \param path œcie¿ka dla timeline
	//! \param series tutaj trafia stworzone serie 
	virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series);
	//! \return wizualizator 3D
	virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
	//! \return typ JointAnglesCollection
	std::vector<utils::TypeInfo> getTypeInfos() const;

private:
	//! motion, z którego beda wyciagane elementy do stworzenia JointAnglesCollection
	core::VariantConstPtr skeletonWithStates;
};
typedef utils::shared_ptr<JointsItemHelper> JointsItemHelperPtr;
typedef utils::shared_ptr<const JointsItemHelper> JointsItemHelperConstPtr;
#endif