#include "Viewer.h"
#include "osgViewer/Viewer"
#include "acclaimformatslib/AsfParser.h"
#include "acclaimformatslib/AmcParser.h"
#include "acclaimformatslib/MotionData.h"
#include "kinematiclib/Skeleton.h"
#include "kinematiclib/SkeletonState.h"
#include "osg/Vec3"
#include <vector>
#include "osgutils/OsgSchemeDrawer.h"
#include "osgGA/TrackballManipulator"
#include "osg/PositionAttitudeTransform"
#include <iostream>
#include <fstream>
#include <utils/Debug.h>
#include <osgManipulator/TranslateAxisDragger>

void kinematicTest::Viewer::start()
{
	std::ifstream fileAsf("./testFiles/test.asf");
	//std::ifstream fileAsf("c:/ProgramData/PJWSTK/IMU-MDE/plugins/imuCostume/recordings/20150601_101449_192.168.001.173_1234/skeleton.asf ");
	std::ofstream fileAsfOut("./testFiles/testOut.asf");
	auto acclaimSkeleton = acclaim::AsfParser::parse(fileAsf, true);

	kinematic::Skeleton kSkeleton;
	kinematic::Skeleton::convert(acclaimSkeleton, kSkeleton);
	acclaim::Skeleton aSkeleton;
	kinematic::Skeleton::convert(kSkeleton, aSkeleton, acclaimSkeleton.axisOrder, kinematicUtils::Deg);

	for (const auto & bd : acclaimSkeleton.bones)
	{
		auto it = aSkeleton.bones.find(bd.first);
		it->second.axis = bd.second.axis;
		it->second.axisOrder = bd.second.axisOrder;
		it->second.dofs = bd.second.dofs;
	}

	acclaim::AsfParser::serialize(fileAsfOut, aSkeleton);
	fileAsfOut.close();
	//auto acclaimSkeleton = asf.parse("./testFiles/B0238.asf");

	//std::ifstream fileAmc("./testFiles/test.amc");	
	//std::ofstream fileAmcOut("./testFiles/testOut.amc");
	//acclaim::MotionData acclaimData;
	//acclaim::AmcParser::parse(acclaimData, fileAmc);
	//std::vector<kinematic::SkeletonState::RigidPartialStateLocal> kData;
	//kData.reserve(acclaimData.frames.size());
	//std::vector<kinematic::SkeletonState::RigidPartialStateLocal> kData2;
	//kData2.reserve(acclaimData.frames.size());
	//auto motionHelperData = acclaim::Skeleton::helperMotionData(acclaimSkeleton);

	const auto kmapping = kinematic::LinearizedSkeleton::createNonLeafMapping(kSkeleton);
	const auto amapping = acclaim::Skeleton::createMapping(aSkeleton.bones);
	const auto acclaimActiveMapping = kinematic::SkeletonState::createAcclaimActiveMappingLocal(kSkeleton, aSkeleton.bones);

	//auto ch = kinematic::SkeletonState::prepareConvertHelper(aSkeleton);
	//auto acclaimNameActiveMapping = kinematic::SkeletonState::createAcclaimNameActiveMappingLocal(kSkeleton, aSkeleton.bones);


	//{
	//	utils::ScopeTimePrinter sp("conversion");
	//	for (const auto & fd : acclaimData.frames)
	//	{
	//		kData.push_back(kinematic::SkeletonState::convert(aSkeleton.bones, amapping, fd.bonesData, acclaimActiveMapping, motionHelperData, aSkeleton.units.isAngleInRadians()));
	//	}
	//}

	//{
	//	utils::ScopeTimePrinter sp("conversion-tuning");
	//	for (const auto & fd : acclaimData.frames)
	//	{
	//		kData2.push_back(kinematic::SkeletonState::convert(fd.bonesData, acclaimNameActiveMapping, ch));
	//	}

	//}

	//acclaim::MotionData cData(acclaimData.frameTime);		

	//unsigned int frameNumber = 1;

	////for (const auto & fd : kData)
	//{
	//	utils::ScopeTimePrinter sp("post processing");
	//	for (const auto & fd : kData)
	//	{
	//		acclaim::MotionData::FrameData frame;
	//		frame.id = frameNumber++;
	//		kinematic::SkeletonState::applyState(kSkeleton, fd);
	//		frame.bonesData = kinematic::SkeletonState::convert(aSkeleton, kinematic::SkeletonState::localRigidState(kSkeleton), acclaimActiveMapping, motionHelperData);
	//		cData.frames.push_back(frame);
	//	}
	//}

	////acclaim::AmcParser::serialize(acclaimData, fileAmcOut);
	//acclaim::AmcParser::serialize(cData, kinematicUtils::AngleUnitType::Deg, fileAmcOut);
	//fileAmcOut.close();

	kinematic::Skeleton skeleton;
	kinematic::Skeleton::convert(acclaimSkeleton, skeleton);
	//

	const auto mapping = kinematic::LinearizedSkeleton::createCompleteMapping(skeleton);
	//for (const auto& p : mapping.data()) {
	//	std::cout << p.get_left() << std::string(", ") << p.get_right() << std::endl;
	//}

	auto root = new osg::Switch;
	auto axis = new osgManipulator::TranslateAxisDragger;
	axis->setupDefaultGeometry();
	root->addChild(axis);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
	root->addChild(pat);
	osgViewer::Viewer viewer;
	osg::Quat att; att.makeRotate(90, 1, 0, 0);
	//pat->setAttitude(att);
	float x = 0, y = 20, z = -2.5;
	float a = 0, b = 0, c = 0;

	viewer.setSceneData(root);
	int frameIdx = 0;
	viewer.setUpViewInWindow(1090, 100, 500, 500);
	//viewer.run();
	osg::ref_ptr<osgGA::TrackballManipulator> tm = new osgGA::TrackballManipulator;
	osgutils::PointsDrawer pointsDrawer(3);
	pointsDrawer.init(mapping.data().size());
	pointsDrawer.setSize(0.01);
	pointsDrawer.setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	pat->addChild(pointsDrawer.getNode());

	osgutils::SegmentsDescriptors connections;

	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton, [&mapping, &connections]
		(kinematic::Skeleton::JointPtr joint, const kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		for (const auto & c : joint->children())
		{
			osgutils::SegmentDescriptor sd;
			sd.length = c->value().localPosition().length();
			sd.range.first = idx;
			sd.range.second = mapping.data().right.find(c->value().name())->get_left();
			connections.push_back(sd);
		}
	});

	osgutils::ConnectionsSphereDrawer	connectionsDrawer(3);
	connectionsDrawer.init(connections);
	connectionsDrawer.setSize(0.04);
	connectionsDrawer.setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	pat->addChild(connectionsDrawer.getNode());

	auto pos = getPos(skeleton);
	std::transform(pos.begin(), pos.end(), pos.begin(), [&](const osg::Vec3& p) { return p + osg::Vec3(a, b, c); });
	//pat->setPosition(osg::Vec3(x, y, z));
	pointsDrawer.update(pos);
	connectionsDrawer.update(pos);

	viewer.setCameraManipulator(tm);
	viewer.run();
	//viewer.realize();

	////int framesCount = acclaimData.frames.size();
	//int framesCount = kData.size();

	//while (!viewer.done()) {
	//	viewer.frame();		
	//	frameIdx = frameIdx >= (framesCount - 1) ? 0 : ++frameIdx;		
	//	kinematic::SkeletonState::applyState(skeleton, kData[frameIdx]);
	//	auto pos = getPos(skeleton);
	//	std::transform(pos.begin(), pos.end(), pos.begin(), [&](const osg::Vec3& p) { return p + osg::Vec3(a, b, c); });
	//	pat->setPosition(osg::Vec3(x,y,z));
	//	pointsDrawer.update(pos);
	//	connectionsDrawer.update(pos);	
	//}
}

std::vector<osg::Vec3> kinematicTest::Viewer::getPos(const kinematic::Skeleton &skeleton)
{	
	std::vector<osg::Vec3> pos;

	kinematic::LinearizedSkeleton::Visitor::visit(skeleton, [&pos](kinematic::Skeleton::JointConstPtr node)
	{
		pos.push_back(node->value().globalPosition());
	});

	return pos;
}
