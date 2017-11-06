//#include "PCH.h"
//#include "BvhSerie.h"
//
//BvhSerie::BvhSerie(KinematicVisualizer * visualizer,
//	const utils::TypeInfo & requestedType,
//	const core::VariantConstPtr & data) :
//	visualizer(visualizer), requestedType(requestedType),
//	data(data), name("Bvh"),
//	localRootNode(new osg::PositionAttitudeTransform),
//	currentFrame(0)
//{
//	bvhData = data->get();
//	
//	data->getMetadata("core/name", name);
//	
//	const auto& joints = generateIds(bvhData->model);
//
//	//pointsDrawer = utils::make_shared<PointsDrawer>(3);
//	//pointsDrawer->init(joints.size());
//	//pointsDrawer->setSize(1.02);
//
//	connectionsDrawer = utils::make_shared<ConnectionsSphereDrawer>(3);
//	SegmentsDescriptors segments = generateConnections(bvhData->model, joints);
//	connectionsDrawer->init(segments);
//	connectionsDrawer->setSize(0.505);
//
//	//pointsDrawer->setColor(osg::Vec4(1.0, 0.0, 0.0, 0.5));
//	connectionsDrawer->setColor(osg::Vec4(0.0, 1.0, 0.0, 0.5));
//
//	//localRootNode->addChild(pointsDrawer->getNode());
//	localRootNode->addChild(connectionsDrawer->getNode());
//	//localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
//	localRootNode->setScale(osg::Vec3(0.01, 0.01, 0.01));
//	osg::Quat rot; rot.makeRotate(3.1415 / 2, osg::Vec3(1, 0, 0));
//	localRootNode->setAttitude(rot);
//	matrixTransform->addChild(localRootNode);
//	requestUpdate();
//	//setAxis(true);
//
//	//updater.reset(new SkeletonStreamSerieUpdater(this));
//
//	//skeletalData->jointsStream->attachObserver(updater);
//}
//
//
//void BvhSerie::update()
//{
//	
//}
//
//osg::Matrix BvhSerie::getInitialMatrix() const
//{
//    return osg::Matrix();
//}
//
//
//const core::VariantConstPtr & BvhSerie::getData() const
//{
//    return data;
//}
//
//const std::string BvhSerie::getName() const
//{
//    return name;
//}
//
//void BvhSerie::setName( const std::string & name )
//{
//    this->name = name;
//}
//
//const utils::TypeInfo & BvhSerie::getRequestedDataType() const
//{
//	return requestedType;
//}
//
//
//void BvhSerie::traverse(biovision::JointConstPtr joint, const osg::Matrix & parentMat, std::vector<osg::Vec3> & positions)
//{	
//	int idx = joint->id;
//	osg::Vec3 dir = joint->direction;
//	double len = joint->length;
//	osg::Vec3 shift = joint->name == "root" ? getCurrentRootPosition(joint) : dir * len;
//	osg::Quat pc = getJointCurrentLocalRotation(joint);
//	
//
//	osg::Matrix rotM; pc.get(rotM);
//	
//	osg::Matrix t = osg::Matrix::identity();
//	t.setTrans(shift);
//
//	auto result = joint->name == "root" ? osg::Matrix::identity() : rotM * t * parentMat;
//	positions.push_back(result.getTrans());
//
//
//	for(auto& j : joint->children) {
//		traverse(j, result, positions);
//	}
//}
//
//void BvhSerie::setLocalTime(double time)
//{
//	std::vector<osg::Vec3> points;
//	currentFrame = time / bvhData->second->getFrameTime();
//	auto root = bvhData->first->getSkeleton().getRoot();
//	traverse(root, osg::Matrix::identity(), points);
//	auto rootP = getCurrentRootPosition(root);
//	/*for (auto& p : points) {
//		p += rootP;
//	}*/
//	localRootNode->setPosition(rootP*0.01);
//	localRootNode->setAttitude(getJointCurrentLocalRotation(root));
//	//pointsDrawer->update(points);
//	connectionsDrawer->update(points);
//}
//
//double BvhSerie::getLength() const
//{
//	double t = (bvhData->second->getFrames().size() -1) * bvhData->second->getFrameTime();
//	return t > 0 ? t : 0.0;
//}
//
//double BvhSerie::getBegin() const
//{
//	return 0.0;
//}
//
//double BvhSerie::getEnd() const
//{
//	return getLength();
//}
//
//SegmentsDescriptors BvhSerie::generateConnections(const biovision::Skeleton& skeleton, const IDMap& idmap)
//{
//	SegmentsDescriptors sd;
//	typedef biovision::JointConstPtr Joint;
//	std::function<void(Joint, const IDMap&, SegmentsDescriptors&)> rec = [&](Joint joint, const IDMap& map, SegmentsDescriptors& sd) {
//		
//		for (auto& child : joint->children) {
//			SegmentDescriptor s;
//			s.range.first = map.at(joint);
//			s.range.second = map.at(child);
//			s.length = (child->length);
//			sd.push_back(s);
//			rec(child, map, sd);
//		}
//	};
//
//	rec(skeleton.getRoot(), idmap, sd);
//	return sd;
//}
//
//BvhSerie::IDMap BvhSerie::generateIds(const biovision::Skeleton& skeleton)
//{
//	typedef biovision::JointConstPtr Joint;
//	IDMap ids;
//	std::function<void(Joint, IDMap&)> rec = [&](Joint joint, IDMap& map) {
//		map[joint] = map.size();
//		for (auto& child : joint->children) {
//			rec(child, map);
//		}
//	};
//
//	rec(skeleton.getRoot(), ids);
//	return ids;
//}
//
//osg::Quat BvhSerie::getJointCurrentLocalRotation(biovision::JointConstPtr joint)
//{
//	// TODO : to trzeba zoptymalizowac
//	using namespace kinematic;
//	std::vector<SkeletalData::singleJointStatePtr> frame = bvhData->second->getFrames()[currentFrame]->jointsData;
//	for (auto state : frame) {
//		if (state->name == joint->name) {
//			double rx, ry, rz;
//            int index = -1;
//			index = acclaim::DegreeOfFreedom::getChannelIndex(acclaim::DegreeOfFreedom::RX, joint->dofs);
//			rx = ((index != -1 || joint->name != "root") ? state->channelValues[index] : 0.0);
//			index = acclaim::DegreeOfFreedom::getChannelIndex(acclaim::DegreeOfFreedom::RY, joint->dofs);
//			ry = ((index != -1 || joint->name != "root") ? state->channelValues[index] : 0.0);
//			index = acclaim::DegreeOfFreedom::getChannelIndex(acclaim::DegreeOfFreedom::RZ, joint->dofs);
//            rz = ((index != -1 || joint->name != "root") ? state->channelValues[index] : 0.0);
//            double mul = bvhData->skeleton->getAngle() == SkeletalModel::radians ? 1.0 : osg::DegreesToRadians(1.0);
//
//						            
//            osg::Quat mX; mX.makeRotate(mul * (rx), osg::Vec3(1,0,0));
//            osg::Quat mY; mY.makeRotate(mul * (ry), osg::Vec3(0,1,0));
//            osg::Quat mZ; mZ.makeRotate(mul * (rz), osg::Vec3(0,0,1)); 
//		     
//			return SkeletonUtils::createRotation(mX, mY, mZ, joint->order);
//
//			/*static int dd = 0;
//			switch (dd) {
//			case 0 +  0: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rx, mul * ry, mul * rz);
//			case 1 +  0: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rx, mul * rz, mul * ry);
//			case 2 +  0: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rz, mul * rx, mul * ry);
//			case 3 +  0: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rz, mul * ry, mul * rx);
//			case 5 +  0: return kinematic::SkeletonUtils::getQuatFromEuler(mul * ry, mul * rx, mul * rz);
//			case 6 +  0: return kinematic::SkeletonUtils::getQuatFromEuler(mul * ry, mul * rz, mul * rx);
//
//			case 0 +  7: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * rx, -mul * ry, -mul * rz);
//			case 1 +  7: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * rx, -mul * rz, -mul * ry);
//			case 2 +  7: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * rz, -mul * rx, -mul * ry);
//			case 3 +  7: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * rz, -mul * ry, -mul * rx);
//			case 5 +  7: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * ry, -mul * rx, -mul * rz);
//			case 6 +  7: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * ry, -mul * rz, -mul * rx);
//
//			case 0 + 14: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * rx, mul * ry, mul * rz);
//			case 1 + 14: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * rx, mul * rz, mul * ry);
//			case 2 + 14: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * rz, mul * rx, mul * ry);
//			case 3 + 14: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * rz, mul * ry, mul * rx);
//			case 5 + 14: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * ry, mul * rx, mul * rz);
//			case 6 + 14: return kinematic::SkeletonUtils::getQuatFromEuler(-mul * ry, mul * rz, mul * rx);
//
//			case 0 + 21: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rx, -mul * ry, mul * rz);
//			case 1 + 21: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rx, -mul * rz, mul * ry);
//			case 2 + 21: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rz, -mul * rx, mul * ry);
//			case 3 + 21: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rz, -mul * ry, mul * rx);
//			case 5 + 21: return kinematic::SkeletonUtils::getQuatFromEuler(mul * ry, -mul * rx, mul * rz);
//			case 6 + 21: return kinematic::SkeletonUtils::getQuatFromEuler(mul * ry, -mul * rz, mul * rx);
//
//			case 0 + 28: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rx, mul * ry, -mul * rz);
//			case 1 + 28: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rx, mul * rz, -mul * ry);
//			case 2 + 28: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rz, mul * rx, -mul * ry);
//			case 3 + 28: return kinematic::SkeletonUtils::getQuatFromEuler(mul * rz, mul * ry, -mul * rx);
//			case 5 + 28: return kinematic::SkeletonUtils::getQuatFromEuler(mul * ry, mul * rx, -mul * rz);
//			case 6 + 28: return kinematic::SkeletonUtils::getQuatFromEuler(mul * ry, mul * rz, -mul * rx);
//			}*/
//			
//
//		}
//	}
//	return osg::Quat();
//}
//
//osg::Vec3 BvhSerie::getCurrentRootPosition(biovision::JointConstPtr root)
//{
//
//	using namespace kinematic;
//	std::vector<biovision::MotionData::singleJointStatePtr> frame = bvhData->data->getFrames()[currentFrame]->jointsData;
//	for (auto state : frame) {
//		if (state->name == "root") {
//			double rx, ry, rz;
//			int index = -1;	  
//			index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TX, root->dofs);
//			rx = state->channelValues[index];							  
//			index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TY, root->dofs);
//			ry = state->channelValues[index];
//			index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TZ, root->dofs);
//			rz = state->channelValues[index];						
//			return osg::Vec3(rx, ry, rz);
//		}
//	}
//
//	UTILS_ASSERT(false);
//	return osg::Vec3();
//}
//
//
//
//
