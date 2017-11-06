/********************************************************************
	created:	2015/01/21
	created:	13:12
	filename: 	SkeletonStreamSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SkeletonStreamSerie_H__
#define HEADER_GUARD_KINEMATIC__SkeletonStreamSerie_H__


#include <corelib/IVisualizer.h>
#include "KinematicSerie.h"
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
class SkeletonStreamSerie : public QObject, public KinematicSerie
{
	Q_OBJECT;

public:
	//! Konstuktor
	//! \param visualizer wizualizator, który stworzył serie danych
	SkeletonStreamSerie(KinematicVisualizer * visualizer,
		const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

	virtual ~SkeletonStreamSerie();

	private slots:
	//! zmiana osi, gdy otrzymamy obiekt w nietypowym ukł. współrzędnych
	//! \param xyz
	void setAxis(bool xyz);

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


	osg::Quat getQuatbyName(const std::string& name);
	void traverse(osgAnimation::Bone* root, const std::vector<osg::Quat>& quats, const std::vector<osg::Vec3>& points);
	osg::Matrix getGlobalTranslation(const std::string& name, const std::vector<osg::Vec3>& points);
private:

	osg::Matrix lToW;

	osg::ref_ptr<osg::PositionAttitudeTransform> localRootNode;
	//! Czy wyliczono juz poprawkę na wysokość
	bool heightCompensation;
	//! Obiekt aktualizujący
	threadingUtils::StreamStatusObserverPtr updater;
	//! Aktualna dodatkowa rotacja wynikająca ze zmiany osi
	osg::Quat preRot;
	//! Pozycja wynikająca z położenia roota szkieletu
	osg::Vec3 rootPosition;
	//! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
	//! wrapper przekazany serii
	core::VariantConstPtr data;
	//! Dane rozpakowane
	SkeletonDataStreamConstPtr skeletalData;
	//! Typ danych
	utils::TypeInfo requestedType;
	//! nazwa serii
	std::string name;
	//! czy operujemy na układzie XYZ czy innym
	bool xyzAxis;
	//! Obiekt rysujący punkty
	utils::shared_ptr<osgutils::IPointsSchemeDrawer> pointsDrawer;
	//! Obiekt rysujący połączenia
	utils::shared_ptr<osgutils::IConnectionsSchemeDrawer> connectionsDrawer;

	std::map<std::string, osgAnimation::Bone*> boneMap;
	osgAnimation::Bone* root;
};


#endif
