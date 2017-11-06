/********************************************************************
	created:	2015/02/16
	created:	16:2:2015   12:24
	filename: 	AbstractSkeletonSerie.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__AbstractSkeletonSerie_H__
#define HEADER_GUARD_KINEMATIC__AbstractSkeletonSerie_H__

#include <corelib/IVisualizer.h>
#include "KinematicSerie.h"
#include <plugins/kinematic/Wrappers.h>
#include <osg/PositionAttitudeTransform>
#include "TrajectoriesDrawer.h"
#include <osgAnimation/Bone>
#include <osgManipulator/TranslateAxisDragger>
#include <osg/Switch>

class KinematicVisualizer;
class SkeletalVisualizationSchemeHelper;
class IPointsSchemeDrawer;
class IConnectionsSchemeDrawer;
class SkeletonJointsMapping;
class TrajectoryDrawerManager;

struct NonDummyJointFilter
{
	template<typename NPtr>
	bool operator()(NPtr node) const
	{
		const auto pos = node->value().localPosition();
		return !((pos[0] == 0.0) && (pos[1] == 0.0) && (pos[2] == 0.0));
	}
};

template<typename Visitor>
using NonDummyJointVisitor = treeContainer::FilterVisitor< NonDummyJointFilter, Visitor >;

//! struktura odpowiedzialna za rysowanie osi dla jointów szkieletu
struct PointsOrientationsDrawer
{
public:
	void init(kinematic::Skeleton::JointConstPtr root);
	osg::ref_ptr<osg::Switch> getNode();
	void update();
	void setLength(double length);
	void setVisible(bool visible);
	void setVisible(bool visible, unsigned int idx);
	bool isVisible(unsigned int idx) const;
	bool isVisible() const;

private:
	std::vector<osg::ref_ptr<osgManipulator::TranslateAxisDragger>> pointAxes;
	kinematic::Skeleton::JointConstWeakPtr root;
	double scale = 1.0;
	osg::ref_ptr<osg::Switch> localNode = new osg::Switch();
	bool visible = true;
};

//! Seria danych wizualizatora 3D wizualizująca animacje szkieletowa
class AbstractSkeletonSerie : public KinematicSerieBase, public IGhostSerie
{

public:
	//! Konstuktor
	//! \param visualizer wizualizator, który stworzył serie danych
	AbstractSkeletonSerie(KinematicVisualizer * visualizer,
		const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

	virtual ~AbstractSkeletonSerie() {}

public:

	//! \return ustawione dane
	virtual const core::VariantConstPtr & getData() const;

	virtual void update();

	virtual const osg::Vec3 pivotPoint() const;

	virtual const utils::TypeInfo & getRequestedDataType() const;

	virtual const bool ghostVisible() const { return false; }
	virtual void setGhostVisible(const bool visible) {}

	//utils::shared_ptr<TrajectoryDrawerManager> getTrajectoriesManager();

	PointsOrientationsDrawer * pointsAxisDrawer();
	void setJointsOrientationsVisible();
	void setBodyPlanesVisible();

	//! Obiekt rysujący punkty
	utils::shared_ptr<osgutils::IPointsSchemeDrawer> getPointsDrawer() const
	{
		return pointsDrawer;
	}

	//! Obiekt rysujący połączenia
	osgutils::IConnectionDrawerWithDescriptors getConnectionsDrawer() const
	{
		return std::make_pair(connectionsDrawer, connections);
	}

	kinematic::SkeletonPtr getSkeleton();

protected:
	//! Abstrakcyjny setter do czasu, metoda z inną sygnaturą może uchronić przed błędami
	//! \param time ustawiany czas
	//virtual void setLocalTime(double time);


	void init(double ratio, int pointsCount, kinematic::SkeletonPtr skeleton, const kinematic::LinearizedSkeleton::GlobalMapping& mapping);
	//! zmiana osi, gdy otrzymamy obiekt w nietypowym ukł. współrzędnych
	//! \param xyz
	void setAxis(bool xyz);


private:

	void initBodyPlanes();

private:

	osg::Matrix lToW;

	//! Aktualna dodatkowa rotacja wynikająca ze zmiany osi
	osg::Quat preRot;
	//! Pozycja wynikająca z położenia roota szkieletu
	osg::Vec3 rootPosition;
	//! Czas faktycznej aktualizacji
	double lastUpdateTime;
	//! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
	//! wrapper przekazany serii
	core::VariantConstPtr data;
	//! Typ danych
	utils::TypeInfo requestedType;
	//! czy operujemy na układzie XYZ czy innym
	bool xyzAxis;
	//! Obiekt rysujący punkty
	utils::shared_ptr<osgutils::IPointsSchemeDrawer> pointsDrawer;
	//! Obiekt rysujący połączenia
	utils::shared_ptr<osgutils::IConnectionsSchemeDrawer> connectionsDrawer;

	kinematic::SkeletonPtr skeleton;
	PointsOrientationsDrawer pointsAxesDrawer;
	osg::ref_ptr<osg::Switch> bodyPlanesSwitch;
	osg::ref_ptr<osg::PositionAttitudeTransform> bodyPlanesPAT;

	std::list<kinematic::LinearizedSkeleton::NodeIDX> nonDummyJoints;
protected:
	//! stworzone połączenia między punktami
	osgutils::SegmentsDescriptors connections;
	//! mapowanie joint -> index
	//std::map<kinematic::Skeleton::JointConstPtr, unsigned int> joint2Index;
	kinematic::LinearizedSkeleton::GlobalMapping nodesMapping;

	osg::ref_ptr<osg::PositionAttitudeTransform> localRootNode;

};

template <class SeriePolicy>
class AbstractSkeletonSerieT : public AbstractSkeletonSerie, public SeriePolicy
{
public:
	AbstractSkeletonSerieT(KinematicVisualizer * visualizer, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data) :
				AbstractSkeletonSerie(visualizer, requestedType, data) {}
	virtual ~AbstractSkeletonSerieT() {}
};
typedef AbstractSkeletonSerieT<plugin::IVisualizer::ISerie> AbstractSkeletonStreamSerie;
typedef AbstractSkeletonSerieT<EventSeriePolicy> AbstractSkeletonTimeSerie;

#endif
