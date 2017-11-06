#ifndef HEADER_GUARD__SKELETALVISAULIZATIONSCHEME_H__
#define HEADER_GUARD__SKELETALVISAULIZATIONSCHEME_H__

#include <map>
#include <utils/Utils.h>
#include <utils/Debug.h>
UTILS_PUSH_WARNINGS
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Node>
UTILS_POP_WARNINGS

#include <boost/utility.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <kinematiclib/JointAnglesCollection.h>

#include "uniqueCollection.h"

class ISchemeDrawer;
typedef boost::shared_ptr<ISchemeDrawer> ISchemeDrawerPtr;
//! klasa stanowi polaczenie miedzy reprezentacja wewnetrzna, a kontrolka odrysowywujaca szkielet
class SkeletalVisualizationScheme
{
public:
    //! stan pojedynczego markera / jointa
    struct JointState {
        osg::Vec3 position;  //!< pozycja w globalnym ukladzie odniesienia
        osg::Vec4 color;     //!< kolor w formacie RGBA <0,1>
        bool isEffector;     //!< czy joint jest lisciem w hierarchii
    };

    //! struktura zawiera jedno polaczenie pomiedzy stawami lub markerami
    struct Connection {
        //! indeks pierwszego stawu/markera (zgodny z przekazywana tablica JointState)
        int index1;         
        //! indeks drugiego stawu/markera (zgodny z przekazywana tablica JointState)
        int index2;
        //! kolor połączenia (RGBA <0,1>)
        osg::Vec4 color;
    };

    
    //! dzieki tej funkcji kontrolka graficzna bedzie wiedziala, ze sie ma odswiezyc
    typedef void(*Refresh)(void);
    
private:
    SkeletalVisualizationScheme();

public:
    //! Metoda tworzy schemat wizualizacji, zachowujac przy okazji slaby wskaznik do niego
    static boost::shared_ptr<SkeletalVisualizationScheme> create();

public: // akcesory
    double getCurrentTime() const { return currentTime; }
    void setCurrentTime(double val);

    int getNumFrames() const { 
        UTILS_ASSERT(kinematicModel);  
        return kinematicModel->getNumPointsPerChannel(); 
    }
    double getFrameTime() const { 
        UTILS_ASSERT(kinematicModel); 
        return 1.0 / kinematicModel->getFrequency(); 
    }
    double getDuration() const { return getNumFrames() * getFrameTime(); }

    //! akcesory do zwracanych danych
    std::vector<JointState>  &getMarkersStates()  { return markersStates; }
    std::vector<JointState>  &getJointStates() { return jointMarkersStates; }
    std::vector<Connection>  &getJointConnections() { return jointConnections; }

    //model kinematyczny
    kinematic::JointAnglesCollectionPtr getKinematicModel() const { return kinematicModel; }
    void setKinematicModel(kinematic::JointAnglesCollectionPtr val);

    //! ustawia schemat odrysowywania modelu
    void setSchemeDrawer(ISchemeDrawerPtr drawer);
    ISchemeDrawerPtr getSchemeDrawer() const { return schemeDrawer; }
        
    //! zdarzenia
public:
    void addRefreshTreeListener        (Refresh val) { pushBackUnique(refreshTree, val); }
    void addRefreshMarkersListener     (Refresh val) { pushBackUnique(refreshMarkers, val); }
    void addRefreshSkeletonListener    (Refresh val) { pushBackUnique(refreshSkeleton, val); }
    void addRefreshTimeListener        (Refresh val) { pushBackUnique(refreshTime, val); }

    void removeRefreshTreeListener     (Refresh val) { removeFromVector(refreshTree, val); }
    void removeRefreshMarkersListener  (Refresh val) { removeFromVector(refreshMarkers, val); }
    void removeRefreshSkeletonListener (Refresh val) { removeFromVector(refreshSkeleton, val); }
    void removeRefreshTimeListener     (Refresh val) { removeFromVector(refreshTime, val); }

private:
           
    //! odswiezenie informacji o jointach
    void updateJoints();
    //! odswiezenie informacji o markerach
    void updateMarkers();
    //! obliczenie poczatkowych transformacji
    void computeBindPoses(kinematic::JointAnglesCollection model);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(double time);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(const std::vector<osg::Quat>& rotations, kinematic::hAnimJointPtr joint, osg::Quat parentRot, osg::Vec3 parentPos);
    void updateWorldTransforms(osg::Vec3 worldPosition, osg::Quat worldRotation);
    void updateSkinnedTransforms();

    void createSkeletonConnections(kinematic::JointPtr joint);

private:
    //! aktualny czas
    double currentTime; 
    //! pomocne przy zapelnianiu kolekcji jointow
    int counterHelper;
    //! stany markerow dla aktualnego czasu
    std::vector<JointState> markersStates;
    //! stany stawow dla aktualnego czasu
    std::vector<JointState> jointMarkersStates;
    //! ulatwia ineksowanie jointow
    std::map<kinematic::hAnimJointPtr, int> visJoints;
    //! zawiera informacje o polaczeniach miedzy stawami
    std::vector<Connection> jointConnections;
    //! aktualny wizualizator schematu
    ISchemeDrawerPtr schemeDrawer;
    //! model kinematyczny z danymi
    kinematic::JointAnglesCollectionPtr kinematicModel;
    //! slaby wskaznik do this
    boost::weak_ptr<SkeletalVisualizationScheme> weak;

    //kolecje z callbackami
    std::vector<Refresh> refreshTree;
    std::vector<Refresh> refreshMarkers;
    std::vector<Refresh> refreshSkeleton;
    std::vector<Refresh> refreshTime;
};

typedef boost::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemePtr;
typedef boost::weak_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemeWeak;

#endif
