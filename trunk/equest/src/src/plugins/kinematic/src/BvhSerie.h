///********************************************************************
//	created:	2014/08/19
//	created:	13:59
//	filename: 	BvhSerie.h
//	author:		Wojciech Kniec
//	
//	purpose:	
//*********************************************************************/
//
//#ifndef HEADER_GUARD_KINEMATIC__BvhSerie_H__
//#define HEADER_GUARD_KINEMATIC__BvhSerie_H__
//
//
//#include <corelib/IVisualizer.h>
//#include "KinematicSerie.h"
//#include "OsgSchemeDrawer.h"
//#include <biovisionformatslib/BvhParser.h>
//
//class KinematicVisualizer;
//
////! Seria danych wizualizatora 3D dostarczająca węzeł osg
//class BvhSerie : public QObject, public KinematicTimeSerie
//{    
//public:
//    //! Konstuktor
//    //! \param visualizer wizualizator, który stworzył serie danych
//	BvhSerie(KinematicVisualizer * visualizer,
//		const utils::TypeInfo & requestedType,
//		const core::VariantConstPtr & data);
//
//public:
//    //! Ustawia nowa nazwę serii
//    virtual void setName(const std::string & name);
//    //! \return nazwa serii
//    virtual const std::string getName() const;
//    //! \return ustawione dane
//    virtual const core::VariantConstPtr & getData() const;
//
//	virtual void update();
//
//	virtual const utils::TypeInfo & getRequestedDataType() const;
//	//! \return długość kanału w sekundach
//	
//
//    //! \return macierz serii z transformacją, która nie została zmieniona przez manipulatory
//    virtual osg::Matrix getInitialMatrix() const;
//
//private:
//	typedef std::map<biovision::JointConstPtr, int> IDMap;
//
//private:
//	void traverse(biovision::JointConstPtr joint, const osg::Matrix & parentMat, std::vector<osg::Vec3> & positions);
//
//	virtual void setLocalTime(double time);
//
//	virtual double getLength() const;
//
//	virtual double getBegin() const;
//
//	virtual double getEnd() const;
//	IDMap generateIds(const biovision::Skeleton& skeleton);
//	SegmentsDescriptors generateConnections(const biovision::Skeleton& skeleton, const IDMap& idmap);
//	osg::Quat getJointCurrentLocalRotation(biovision::JointConstPtr joint);
//	osg::Vec3 getCurrentRootPosition(biovision::JointConstPtr root);
//private:
//    //! Wizualizator, który utworzył serie
//	KinematicVisualizer * visualizer;
//    //! wrapper przekazany serii
//    core::VariantConstPtr data;
//	//! Informacja o zadanym typie
//	utils::TypeInfo requestedType;
//    //! nazwa serii
//    std::string name;
//	//! Obiekt rysujący punkty
//	utils::shared_ptr<IPointsSchemeDrawer> pointsDrawer;
//	//! Obiekt rysujący połączenia
//	utils::shared_ptr<IConnectionsSchemeDrawer> connectionsDrawer;
//	osg::ref_ptr<osg::PositionAttitudeTransform> localRootNode;
//
//	biovision::BVHDataConstPtr bvhData;
//	int currentFrame;
//};
//DEFINE_SMART_POINTERS(BvhSerie)
//
//
//#endif
