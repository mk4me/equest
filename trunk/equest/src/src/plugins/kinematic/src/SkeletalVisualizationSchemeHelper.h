///********************************************************************
//    created:  2013/11/29
//    created:  29:11:2013   10:58
//    filename: SkeletalVisualizationSchemeHelper.h
//    author:   Mateusz Janiak
//    
//    purpose:  
//*********************************************************************/
#ifndef HEADER_GUARD_KINEMATIC__SKELETALVISUALIZATIONSCHEMEHELPER_H__
#define HEADER_GUARD_KINEMATIC__SKELETALVISUALIZATIONSCHEMEHELPER_H__

#include <kinematiclib/Skeleton.h>
#include <osgutils/OsgSchemeDrawer.h>

class SkeletonJointsMapping
{
public:
	SkeletonJointsMapping() {}
	~SkeletonJointsMapping() {}

	void init(kinematic::SkeletonConstPtr skeleton,
		const std::vector<std::string> & indices = std::vector<std::string>());

	//! Metoda pomocnicza przy wypelnianiu wektora rotacji
	//! \param joint Joint o ktorego indeks pytamy
	//! \return indeks jointa lub -1 jezeli jointa nie ma
	const int jointIndex(const std::string & jointName) const;
	//! Metoda pomocnicza przy wypelnianiu wektora rotacji
	//! \param joint Joint o ktorego indeks pytamy
	//! \return indeks jointa lub -1 jezeli jointa nie ma
	const int jointIndex(kinematic::Skeleton::JointConstPtr joint) const;

	const unsigned int mappedJointsNumber() const;

	kinematic::SkeletonConstPtr skeleton() const;

	const osgutils::SegmentsDescriptors generateMappedConnectionsDescription() const;

private:

	void generateMappedConnectionsDescription( kinematic::Skeleton::JointConstPtr join,
		const unsigned int idx,
		osgutils::SegmentsDescriptors & cds) const;

private:
	//! Szkielet dla którego generowano mapowanie
	kinematic::SkeletonConstPtr skeleton_;
	//! ułatwia ineksowanie jointów
	std::map<std::string, unsigned int> namedVisJoints;
	//! ułatwia ineksowanie jointów
	std::map<kinematic::Skeleton::JointConstPtr, unsigned int> visJoints;
};


//! Klasa stanowi połączenie między jointami a ich wizualizacja
class SkeletalVisualizationSchemeHelper
{
public:

	//! Konstruktor ustawia domyślne wartości w tym kolory
	SkeletalVisualizationSchemeHelper(osgutils::IPointsSchemeDrawer * pointsSchemeDrawer,
		osgutils::IConnectionsSchemeDrawer * connectionsSchemeDrawer);
	//! Destrutkor wirtualny
	~SkeletalVisualizationSchemeHelper();

	//! Inicjuje połączenia i mapowanie jointow
	void init(const SkeletonJointsMapping * jointsMapping);
	
	void init(const SkeletonJointsMapping * jointsMapping,
		const osgutils::SegmentsDescriptors & connections);

    //! odświeżenie informacji o jointach
	void updateJointTransforms(const osg::Vec3 & rootPosition,
		const std::vector<osg::Quat> & rotations);

	void updateJointTransforms(const std::vector<osg::Vec3> & pointsPositions);

	void calculatePointsPositions(std::vector<osg::Vec3> & pointsPositions,
		const osg::Vec3 & rootPosition,
		const std::vector<osg::Quat> & rotations);


private:

	void innerCalculatePointsPositions(std::vector<osg::Vec3> & pointsPositions,
		const osg::Vec3 & rootPosition,
		const std::vector<osg::Quat> & rotations);

  //  //! obliczenie transformacji dla podanego czasu
  //  //! \param rotations rotacje jointów dla konkretnej klatki
  //  //! \param joint przetwarzany staw
  //  //! \param parentRot rotacja rodzica
  //  //! \param parentPos pozycja rodzica
	void updateJointTransforms(const std::vector<osg::Quat> & rotations, kinematic::Skeleton::JointConstPtr joint,
							   const osg::Quat & parentRot, const osg::Vec3 & parentPos,
							   std::vector<osg::Vec3> & pointsPositions);

private:
	//! Drawer chmury punktow
	osgutils::IPointsSchemeDrawer * pointsSchemeDrawer;
	//! Drawer polaczen miedzy punktami
	osgutils::IConnectionsSchemeDrawer * connectionsSchemeDrawer;
	//! Mapa danych wejsciowych do obiektów wizualizacji
	const SkeletonJointsMapping * jointsMapping;
	
};

#endif	//	HEADER_GUARD_KINEMATIC__SKELETALVISUALIZATIONSCHEMEHELPER_H__
