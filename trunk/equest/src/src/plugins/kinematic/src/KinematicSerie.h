/********************************************************************
	created:	2012/05/23
	created:	23:5:2012   21:12
	filename: 	KinematicSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__KINEMATICSERIE_H__
#define HEADER_GUARD_KINEMATIC__KINEMATICSERIE_H__

#include <plugins/c3d/EventSerieBase.h>
#include <osg/MatrixTransform>

class IGhostSerie
{
public:
	virtual ~IGhostSerie() {}

public:
	virtual const bool ghostVisible() const = 0;
	virtual void setGhostVisible(const bool visible) = 0;
};


class KinematicSerieBase
{
public: 
	typedef osg::ref_ptr<osg::MatrixTransform> MatrixTransformPtr;	

public:
	KinematicSerieBase();
	virtual ~KinematicSerieBase();

public:
	//! \return macierz, na której operują manipulatory
	MatrixTransformPtr getMatrixTransformNode();
	//! Metoda niweluje efekty działania manipulatorów
	virtual void resetTransform();

	virtual const osg::Vec3 pivotPoint() const;

protected:
	//! macierz, na której działają manipulatory
	MatrixTransformPtr matrixTransform;
};

template <class SeriePolicy>
class KinematicSerieBaseT : public KinematicSerieBase, public SeriePolicy
{};
typedef KinematicSerieBaseT<plugin::IVisualizer::ISerie> KinematicSerie;


class EventSeriePolicy : public EventSerieBase
{
public:
    //! W konstruktorze tworzone są już węzły i zerowany czas
	EventSeriePolicy();
    virtual ~EventSeriePolicy() {}
public:
    //! KinematicSerie jest serią zawierającą eventy. Klasy pochodne maja możliwość ich obsługi
	virtual void setEvents(c3dlib::EventsCollectionConstPtr val);
    //! Zwraca czas 
    double getTime() const { return time; }
    //! Ustawia czas. Metoda celowo nie jest wirtualna, klasy dziedziczące powinny implementować setLocalTime
    //! \param val ustawiany czas
    void setTime(double val);
	//! Metoda niweluje efekty działania manipulatorów
	virtual void resetTransform();
	//! Ustawia nową nazwę serii
	//! \param name nowa nazwa
	virtual void setName(const std::string & name);
	//! \return nazwa serii
	virtual const std::string getName() const;

protected:
    //! Abstrakcyjny setter do czasu, metoda z inną sygnaturą może uchronić przed błędami
    //! \param time ustawiany czas
    virtual void setLocalTime(double time) = 0;

protected:
    //! zdarzenia związane z próba pomiarowa
	c3dlib::EventsCollectionConstPtr events;
private:
    //! ustawiony czas dla serii
    double time;
	//! nazwa serii
	std::string name;
};

/*! Podstawowa i abstrakcyjna seria danych w wizualizatorze 3D */
typedef KinematicSerieBaseT<EventSeriePolicy> KinematicTimeSerie;


#endif
