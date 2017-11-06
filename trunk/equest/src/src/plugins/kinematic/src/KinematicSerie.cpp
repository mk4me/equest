#include "PCH.h"
#include "KinematicSerie.h"

KinematicSerieBase::KinematicSerieBase() :
	matrixTransform(new osg::MatrixTransform())	
{

}	

KinematicSerieBase::~KinematicSerieBase()
{

}

KinematicSerieBase::MatrixTransformPtr KinematicSerieBase::getMatrixTransformNode()
{
	return matrixTransform;
}

void KinematicSerieBase::resetTransform()
{
	matrixTransform->setMatrix(osg::Matrix());
}

const osg::Vec3 KinematicSerieBase::pivotPoint() const
{
	return osg::Vec3(0.0, 0.0, 0.0);
}

EventSeriePolicy::EventSeriePolicy() : time(-1.0)
{

}

void EventSeriePolicy::resetTransform()
{
	//KinematicSerieBase::resetTransform();
    // ustawienie czasu (powinno odświeżyć)
    setTime(0.0);
}

void EventSeriePolicy::setTime( double val )
{
    time = val; 
    // ustawienie czasu w klasie pochodnej
    setLocalTime(val);
}

void EventSeriePolicy::setEvents(c3dlib::EventsCollectionConstPtr val)
{
    events = val;
}

void EventSeriePolicy::setName(const std::string & name)
{
	this->name = name;
}

const std::string EventSeriePolicy::getName() const
{
	return name;
}

