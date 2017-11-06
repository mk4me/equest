/********************************************************************
	created:	2012/03/12
	created:	12:3:2012   11:17
	filename: 	Manipulators.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MANIPULATORS_H__
#define HEADER_GUARD_KINEMATIC__MANIPULATORS_H__

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osg/CoordinateSystemNode>
#include <osgText/Text>

#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TabPlaneDragger>
#include <osgManipulator/TabPlaneTrackballDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Translate1DDragger>
#include <osgManipulator/Translate2DDragger>
#include <osgManipulator/TranslateAxisDragger>

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>

#include <iostream>

//! Klasa wzorowana na przykładzie z osg, pozwala zachować stały rozmiar manipulatora niezaleznie od odległości od kamery
class DraggerContainer : public osg::Group
{
public:
    //! ustawienie domyślnych parametrów
    DraggerContainer() : _draggerSize(100.0f), _active(true) {}
    //! Konstruktor kopiujący (w tylu osg)
    //! \param copy kopiowany obiekt
    //! \param copyop rodzaj kopii (płytka, głęboka, ... )
    DraggerContainer( const DraggerContainer& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    
public:
    META_Node( osgManipulator, DraggerContainer );
    //! Ustawienie manipulatora, który ma zachować jednakowy rozmiar
    //! \param dragger 
    void setDragger( osgManipulator::Dragger* dragger );
    //! \return ustawiony manipulator, który ma zachować jednakowy rozmiar
    osgManipulator::Dragger* getDragger() { return _dragger.get(); }
    //! \return ustawiony manipulator, który ma zachować jednakowy rozmiar
    const osgManipulator::Dragger* getDragger() const { return _dragger.get(); }
    //! nowy rozmiar manipulaotra
    //! \param size rozmiar w jednostkach sceny
    void setDraggerSize( float size ) { _draggerSize = size; }
    //! \return pobiera rozmiar manipulatora w jednostkach sceny
    float getDraggerSize() const { return _draggerSize; }
    //! zmiana aktywności manipulatora
    void setActive( bool b ) { _active = b; }
    //! \return aktywność manipulatora
    bool getActive() const { return _active; }
    //! Metoda wykonuje główną pracę, dostosowywuje rozmiar do faktycznej rozdzielczości okna, w którym odbywa się renderowanie
    //! \param nv obiekt odwiedzający węzeł (zgodnie ze wzorcem projektowym)
    void traverse( osg::NodeVisitor& nv );
    //! Ustawia punkt obrotu manipulatora
    //! \param pivot nowy punkt obrotu
    void setDraggerPivot( const osg::Vec3& pivot );
protected:
    //! opakowywany manipulator
    osg::ref_ptr<osgManipulator::Dragger> _dragger;
    //! stały rozmiar manipulatora
    float _draggerSize;
    //! czy aktualnie używany
    bool _active;
};
typedef osg::ref_ptr<DraggerContainer> DraggerContainerPtr;

//! Klasa służąca do obsługi manipulatorów
class Manipulators
{
public:
    //! rodzaje dostępnych manipulatorów
    enum DraggerType 
    {
        TabPlane,
        TabPlaneTrackball,
        TabBoxTrackball,
        Trackball,
        Translate1D,
        Translate2D,
        TranslateAxis,
        TabBox
    };

    //! Podłączenie manipulatora między parenta i childa
    //! \param parent do niego zostanie podpięty manipulator
    //! \param child obiekt, na którym będzie działał manipulator
    //! \param draggerContainer opakowany manipulator (stały rozmiar)
    //! \param pivot punkt zaczepienia manipulatora
    static void connect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer, const osg::Vec3& pivot );
    //! Rozłączenie manipulaotra
    //! \param parent od niego trzeba się odpiąć
    //! \param child obiekt, który przestaje być manipulowany
    //! \param draggerContainer odpinany manipulaotr
    static void disconnect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer);
    //! Metoda fabrykująca manipulatory
    //! \param type typ manipulaotra
    //! \return stworzony obiekt
    static osgManipulator::Dragger* createDragger(DraggerType type);
    //! Metoda fabrykująca manipulatory o stałym rozmiarze
    //! \param type  typ manipulaotra
    //! \return stworzony obiekt
    static DraggerContainer* creatreDraggerContainer(DraggerType type);
    //! Metoda tworzy nowy manipulator i podpina go pod węzeł, który chcemy manipulować
    //! \param scene węzeł, który będzie manipulowany
    //! \param type typ manipulatora
    //! \param fixedSizeInScreen czy manipulator ma mieć stały rozmiar
    //! \return węzeł zawierający manipulator
    static osg::Node* addDraggerToScene(osg::PositionAttitudeTransform* scene, DraggerType type, bool fixedSizeInScreen);
};
#endif
