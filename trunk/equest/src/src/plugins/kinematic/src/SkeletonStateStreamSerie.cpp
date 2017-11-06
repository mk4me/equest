#include "PCH.h"
#include "SkeletonStateStreamSerie.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <threadingUtils/StreamData.h>

SkeletonStateStreamSerie::SkeletonStateStreamSerie(KinematicVisualizer * visualizer,	const utils::TypeInfo & requestedType,const core::VariantConstPtr & data) :
	visualizer(visualizer),	data(data), requestedType(requestedType),
	AbstractSkeletonStreamSerie(visualizer, requestedType, data),
	name("SkeletonData")
{
	UTILS_ASSERT(data->data()->getTypeInfo() == typeid(SkeletonWithStreamData));
	data->getMetadata("core/name", name);
	skeletalData = data->get();
	auto skeleton = utils::make_shared<kinematic::Skeleton>(*skeletalData->skeleton);

	auto ratio = 1.0;
	int pointsCount = skeletalData->nodesMapping.data().size();
	AbstractSkeletonSerie::init(ratio, pointsCount, skeleton, skeletalData->nodesMapping);

	updater.reset(new plugin::StreamDataSerieUpdater(this));
	skeletalData->states->attach(updater);
}

SkeletonStateStreamSerie::~SkeletonStateStreamSerie()
{
	if (skeletalData != nullptr){
		skeletalData->states->detach(updater);
	}
}


void SkeletonStateStreamSerie::setName(const std::string & name)
{
	this->name = name;
}

const std::string SkeletonStateStreamSerie::getName() const
{
	return name;
}

const core::VariantConstPtr & SkeletonStateStreamSerie::getData() const
{
	return data;
}

void SkeletonStateStreamSerie::update()
{	
	skeletalData->states->data(currentState);

	kinematic::SkeletonState::applyState(*getSkeleton(), currentState);
	AbstractSkeletonSerie::update();
}

const utils::TypeInfo & SkeletonStateStreamSerie::getRequestedDataType() const
{
	return requestedType;
}

const osg::Vec3 SkeletonStateStreamSerie::pivotPoint() const
{
	return rootPosition; // * lToW;
}

