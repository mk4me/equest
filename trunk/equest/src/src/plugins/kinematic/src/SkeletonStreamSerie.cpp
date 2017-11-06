#include "PCH.h"
#include "SkeletonStreamSerie.h"
#include "KinematicVisualizer.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <threadingUtils/StreamData.h>

static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

SkeletonStreamSerie::SkeletonStreamSerie(KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data) :
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	xyzAxis(false),
	name("SkeletonData"),
	pointsDrawer(new osgutils::PointsDrawer(3)),
	connectionsDrawer(new osgutils::ConnectionsSphereDrawer(10)),
	heightCompensation(false),
	localRootNode(new osg::PositionAttitudeTransform)
{
	UTILS_ASSERT(data->data()->getTypeInfo() == typeid(SkeletonDataStream));
	data->getMetadata("core/name", name);
	skeletalData = data->get();

	pointsDrawer->init(skeletalData->jointsCount);
	pointsDrawer->setSize(0.02);

	connectionsDrawer->init(skeletalData->connections);
	connectionsDrawer->setSize(0.005);

	pointsDrawer->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
	connectionsDrawer->setColor(osg::Vec4(0.0, 1.0, 0.0, 0.5));

	localRootNode->addChild(pointsDrawer->getNode());
	localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	//setAxis(true);

	updater.reset(new plugin::StreamDataSerieUpdater(this));

	skeletalData->jointsStream->attach(updater);
}

SkeletonStreamSerie::~SkeletonStreamSerie()
{
	if (skeletalData != nullptr){
		skeletalData->jointsStream->detach(updater);
	}
}

void SkeletonStreamSerie::setAxis(bool xyz)
{
	//TODO - sprawdzic jak to dziala
	xyzAxis = xyz;
	localRootNode->setAttitude(xyz == true ? invQXYZ : osg::Quat());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
}

void SkeletonStreamSerie::setName(const std::string & name)
{
	this->name = name;
}

const std::string SkeletonStreamSerie::getName() const
{
	return name;
}

const core::VariantConstPtr & SkeletonStreamSerie::getData() const
{
	return data;
}

void SkeletonStreamSerie::update()
{
	std::vector<osg::Vec3> points;
	skeletalData->jointsStream->data(points);

	if (heightCompensation == false){
		heightCompensation = true;
		float minZ = std::numeric_limits<float>::max();
		for (auto it = points.begin(); it != points.end(); ++it){
			minZ = std::min(minZ, (*it).z());
		}

		minZ -= pointsDrawer->size(0);

		localRootNode->setPosition(osg::Vec3(0, 0, -minZ));
		localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
	}

	rootPosition = osg::Vec3(0, 0, 0);

	for (auto it = points.begin(); it != points.end(); ++it){
		rootPosition += *it;
	}

	rootPosition /= points.size();

	pointsDrawer->update(points);
	connectionsDrawer->update(points);
}

const utils::TypeInfo & SkeletonStreamSerie::getRequestedDataType() const
{
	return requestedType;
}

const osg::Vec3 SkeletonStreamSerie::pivotPoint() const
{
	return rootPosition * lToW;
}