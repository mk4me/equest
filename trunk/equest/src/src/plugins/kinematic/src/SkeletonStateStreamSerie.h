/********************************************************************
	created:	2015/02/16
	created:	13:12
	filename: 	SkeletonStreamStateSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SkeletonStateStreamSerie_H__
#define HEADER_GUARD_KINEMATIC__SkeletonStateStreamSerie_H__



#include <corelib/IVisualizer.h>
#include "AbstractSkeletonSerie.h"
#include <plugins/kinematic/Wrappers.h>
#include <osg/PositionAttitudeTransform>
#include "TrajectoriesDrawer.h"
#include <osgAnimation/Bone>

class KinematicVisualizer;
class SkeletalVisualizationSchemeHelper;
class IPointsSchemeDrawer;
class IConnectionsSchemeDrawer;
class GhostSchemeDrawer;
class SkeletonJointsMapping;
class TrajectoryDrawerManager;


//! Seria danych wizualizatora 3D wizualizująca animacje szkieletowa
class SkeletonStateStreamSerie : public QObject, public AbstractSkeletonStreamSerie
{
	Q_OBJECT;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, który stworzył serie danych
	SkeletonStateStreamSerie(KinematicVisualizer * visualizer,
		const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

	virtual ~SkeletonStateStreamSerie();


public:

	//! Ustawia nową nazwę serii
	//! \param name nowa nazwa
	virtual void setName(const std::string & name);
	//! \return nazwa serii
	virtual const std::string getName() const;
	//! \return ustawione dane
	virtual const core::VariantConstPtr & getData() const;

	virtual void update();

	virtual const osg::Vec3 pivotPoint() const;

	virtual const utils::TypeInfo & getRequestedDataType() const;


private:
	//! Obiekt aktualizujący
	threadingUtils::StreamStatusObserverPtr updater;
	//! Pozycja wynikająca z położenia roota szkieletu
	osg::Vec3 rootPosition;
	//! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
	//! wrapper przekazany serii
	core::VariantConstPtr data;
	//! Dane rozpakowane
	SkeletonWithStreamDataConstPtr skeletalData;
	//! Typ danych
	utils::TypeInfo requestedType;
	//! nazwa serii
	std::string name;
	kinematic::SkeletonState::NonRigidCompleteStateLocal currentState;

};

#endif
