#include "PCH.h"
#include "OsgSerie.h"

OsgSerie::OsgSerie(KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data) : 
	visualizer(visualizer), requestedType(requestedType),
	data(data), name("Osg")
{
	osg::ref_ptr<const osg::PositionAttitudeTransform> patTest = data->get();
	const core::VariantPtr var = data->clone();
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = var->get();
	matrixTransform->addChild(pat);
}


void OsgSerie::update()
{
}

osg::Matrix OsgSerie::getInitialMatrix() const
{
    return osg::Matrix();
}


const core::VariantConstPtr & OsgSerie::getData() const
{
    return data;
}

const std::string OsgSerie::getName() const
{
    return name;
}

void OsgSerie::setName( const std::string & name )
{
    this->name = name;
}

const utils::TypeInfo & OsgSerie::getRequestedDataType() const
{
	return requestedType;
}




