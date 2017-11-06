#include <kinematiclib/SkeletonState.h>
#include <osg/PositionAttitudeTransform>
#include <boost/lexical_cast.hpp>
#include <kinematicUtils/RotationConverter.h>
#include <treecontainerlib/NodeT.h>
#include <utils/Debug.h>

using namespace kinematic;

template<int AngleInRadians>
osg::Vec3d resolveRadians(const osg::Vec3d& toResolve)
{
	static_assert((AngleInRadians == 0) || (AngleInRadians == 1), "Nalezy użyć jednej z dwóch wersji");
	return osg::Vec3d(0,0,0);
}

using RadiansResolver = osg::Vec3d(*)(const osg::Vec3d &);

template<>
osg::Vec3d resolveRadians<1>(const osg::Vec3d& toResolve)
{
	return toResolve;
}

template<>
osg::Vec3d resolveRadians<0>(const osg::Vec3d& toResolve)
{
	return kinematicUtils::toRadians(toResolve);
}

osg::Vec3d resolveRadians(const bool angleInRadians, const osg::Vec3d& toResolve)
{
	return angleInRadians == true ? toResolve : kinematicUtils::toRadians(toResolve);
}

LinearizedSkeleton::LocalMapping createActiveMapping(const Skeleton & skeleton,
	const acclaim::Skeleton::Bones & bones)
{
	auto ret = LinearizedSkeleton::createNonLeafMapping(skeleton);

	for (const auto & bd : bones)
	{
		if (bd.second.isActive() == false){
			ret.data().right.erase(bd.second.name);
		}
	}

	return ret;
}

SkeletonState::AcclaimActiveSkeletonMappingLocal SkeletonState::createAcclaimActiveMappingLocal(
	const Skeleton & skeleton, const acclaim::Skeleton::Bones & bones)
{
	const auto skeletonMapping = LinearizedSkeleton::createNonLeafMapping(skeleton);	

	AcclaimActiveSkeletonMapping ret;

	for (const auto & bd : bones)
	{
		if (bd.second.isActive() == true){
			auto it = skeletonMapping.data().right.find(bd.second.name);
			if (it != skeletonMapping.data().right.end())
			{
				ret.insert({ it->get_left(), bd.first });
			}
		}
	}

	//root - bo w mapowaniu mogą się róznić nazwy rootów
	ret.insert({ skeletonMapping.data().left.begin()->get_left(), bones.begin()->first });

	return AcclaimActiveSkeletonMappingLocal(ret);
};

SkeletonState::AcclaimActiveSkeletonMappingGlobal SkeletonState::createAcclaimActiveMappingGlobal(const Skeleton & skeleton,
	const acclaim::Skeleton::Bones & bones)
{
	const auto skeletonMapping = LinearizedSkeleton::createCompleteMapping(skeleton);

	AcclaimActiveSkeletonMapping ret;

	for (const auto & bd : bones)
	{
		if (bd.second.isActive() == true){
			auto it = skeletonMapping.data().right.find(bd.second.name);
			if (it != skeletonMapping.data().right.end())
			{
				ret.insert({ it->get_left(), bd.first });
			}
		}
	}

	//root - bo w mapowaniu mogą się róznić nazwy rootów
	ret.insert({ skeletonMapping.data().left.begin()->get_left(), bones.begin()->first });

	return AcclaimActiveSkeletonMappingGlobal(ret);
}

SkeletonState::AcclaimNameActiveSkeletonMappingLocal SkeletonState::createAcclaimNameActiveMappingLocal(const Skeleton & skeleton,
	const acclaim::Skeleton::Bones & bones)
{
	const auto skeletonMapping = LinearizedSkeleton::createNonLeafMapping(skeleton);

	AcclaimNameActiveSkeletonMapping ret;

	for (const auto & bd : bones)
	{
		if (bd.second.isActive() == true){
			auto it = skeletonMapping.data().right.find(bd.second.name);
			if (it != skeletonMapping.data().right.end())
			{
				ret.insert({ bd.second.name, it->get_left() });
			}
		}
	}

	//root - bo w mapowaniu mogą się róznić nazwy rootów
	ret.insert({ bones.begin()->second.name, skeletonMapping.data().left.begin()->get_left() });

	return AcclaimNameActiveSkeletonMappingLocal(ret);
}

SkeletonState::AcclaimNameActiveSkeletonMappingGlobal SkeletonState::createAcclaimNameActiveMappingGlobal(const Skeleton & skeleton,
	const acclaim::Skeleton::Bones & bones)
{
	const auto skeletonMapping = LinearizedSkeleton::createCompleteMapping(skeleton);

	AcclaimNameActiveSkeletonMapping ret;

	for (const auto & bd : bones)
	{
		if (bd.second.isActive() == true){
			auto it = skeletonMapping.data().right.find(bd.second.name);
			if (it != skeletonMapping.data().right.end())
			{
				ret.insert({ bd.second.name, it->get_left() });
			}
		}
	}

	//root - bo w mapowaniu mogą się róznić nazwy rootów
	ret.insert({ bones.begin()->second.name, skeletonMapping.data().left.begin()->get_left() });

	return AcclaimNameActiveSkeletonMappingGlobal(ret);
}

std::array<unsigned int, 3> positionIndicies(const acclaim::Bone & bone)
{
	std::array<unsigned int, 3> ret;
	unsigned int idx = 0;
	unsigned int pos = 0;
	for (const auto & dof : bone.dofs)
	{
		switch (dof.channel)
		{
		case kinematicUtils::ChannelType::TX:
			ret[0] = idx;
			++pos;
			break;

		case kinematicUtils::ChannelType::TY:
			ret[1] = idx;
			++pos;
			break;

		case kinematicUtils::ChannelType::TZ:
			ret[2] = idx;
			++pos;
			break;
		}	

		if (pos == 3){
			break;
		}

		++idx;
	}

	return ret;
}

std::array<unsigned int, 3> orientationIndicies(const acclaim::Bone & bone)
{
	std::array<unsigned int, 3> ret;
	unsigned int idx = 0;
	unsigned int pos = 0;
	for (const auto & dof : bone.dofs)
	{
		switch (dof.channel)
		{
		case kinematicUtils::ChannelType::RX:
			ret[pos++] = idx;
			break;

		case kinematicUtils::ChannelType::RY:
			ret[pos++] = idx;			
			break;

		case kinematicUtils::ChannelType::RZ:
			ret[pos++] = idx;			
			break;
		}	

		if (pos == 3){
			break;
		}

		++idx;
	}

	return ret;
}

osg::Vec3d extractVector(const acclaim::MotionData::ChannelValues & channelValues, int XPos, int YPos, int ZPos)
{
	return{ channelValues[XPos], channelValues[YPos], channelValues[ZPos] };
}

osg::Vec3d extractPosition(const acclaim::MotionData::ChannelValues & channelValues,
	const acclaim::Bone & bone)
{
	if (bone.dofs.size() != channelValues.size()){
		throw std::runtime_error("Acclaim motion data mismatch skeleton structure degrees of freedom for node " + bone.name + ". In model declared " + boost::lexical_cast<std::string>(bone.dofs.size()) + " dof`s but data contains " + boost::lexical_cast<std::string>(channelValues.size()) + " dof`s.");
	}

	osg::Vec3d ret(0, 0, 0);
	unsigned char transi = 0;
	unsigned char i = 0;

	for (const auto & dof : bone.dofs)
	{
		switch (dof.channel)
		{
		case kinematicUtils::ChannelType::TX:
			ret.x() = channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::TY:
			ret.y() = channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::TZ:
			ret.z() = channelValues[i];
			++transi;
			break;
		}

		++i;

		if (transi == 3){
			break;
		}
	}

	return ret;
}

SkeletonState::NonRigidJointState convert(const acclaim::Skeleton::Bones & skeletonBones,
	const acclaim::MotionData::BoneData & boneData, const bool angleInRadians,
	const acclaim::Bone::HelperMotionData & helperMotionData)
{
	auto it = std::find_if(skeletonBones.begin(), skeletonBones.end(), [&boneData](const acclaim::Skeleton::Bones::value_type & val)
	{
		return val.second.name == boneData.name;
	});

	if (it == skeletonBones.end()){
		throw std::runtime_error("Acclaim motion data mismatch skeleton structure - missing " + boneData.name + " node");
	}

	const auto & bone = it->second;	

	if (bone.dofs.size() != boneData.channelValues.size()){
		throw std::runtime_error("Acclaim motion data mismatch skeleton structure degrees of freedom for node " + boneData.name + ". In model declared " + boost::lexical_cast<std::string>(bone.dofs.size()) + " dof`s but data contains " + boost::lexical_cast<std::string>(boneData.channelValues.size()) + " dof`s.");
	}

	SkeletonState::NonRigidJointState ret;
	ret.position = osg::Vec3d(0, 0, 0);
	ret.orientation = osg::Quat(0, 0, 0, 1);

	osg::Vec3d rot(0, 0, 0);
	unsigned int i = 0;
	unsigned int roti = 0;
	unsigned int transi = 0;

	for (const auto & dof : bone.dofs)
	{
		switch (dof.channel)
		{
		case kinematicUtils::ChannelType::TX:
			ret.position.x() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::TY:
			ret.position.y() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::TZ:
			ret.position.z() = boneData.channelValues[i];
			++transi;
			break;

		case kinematicUtils::ChannelType::RX:
			rot[roti++] = boneData.channelValues[i];
			break;

		case kinematicUtils::ChannelType::RY:
			rot[roti++] = boneData.channelValues[i];
			break;

		case kinematicUtils::ChannelType::RZ:
			rot[roti++] = boneData.channelValues[i];
			break;

		case acclaim::DegreeOfFreedom::L:
			//TODO - przesuń dziecko
			break;

		default:
			throw std::runtime_error("Unsupported acclaim degree of freedom: " + boost::lexical_cast<std::string>(dof.channel));
		}

		++i;
	}

	ret.orientation = helperMotionData.cInv * kinematicUtils::convert(resolveRadians(angleInRadians, rot), bone.rotationOrder()) * helperMotionData.c;

	return ret;
}

SkeletonState::BoneRotationConverter createBoneOrienationExtractor(
	const acclaim::Bone & bone,	RadiansResolver radiansResolver)
{
	auto rotOrder = bone.rotationOrder();

	osg::Quat c = kinematicUtils::convert(radiansResolver(bone.axis), bone.axisOrder);
	osg::Quat cInv = c.inverse();

	auto indicies = orientationIndicies(bone);
	kinematicUtils::EulerConverter angleConverter = kinematicUtils::eulerConverter(rotOrder);

	return [&](const acclaim::MotionData::ChannelValues & channelValues)
	{
		return cInv * angleConverter(radiansResolver(osg::Vec3d(channelValues[indicies[0]], channelValues[indicies[1]], channelValues[indicies[2]]))) * c;
	};	
}

SkeletonState::RotationConvertersMap createRotationConvertersMap(const acclaim::Skeleton & skeleton,
	RadiansResolver radiansResolver)
{
	SkeletonState::RotationConvertersMap ret;

	for (const auto & bd : skeleton.bones)
	{
		if (bd.second.isActive() == true){
			ret.insert({ bd.second.name, createBoneOrienationExtractor(bd.second, radiansResolver) });
		}
	}

	return ret;
}

void orderChanges(const acclaim::Skeleton & skeleton, const int currentBoneID,
	const std::map<std::string, SkeletonState::NonRigidJointState> & changes,
	SkeletonState::NonRigidCompleteState & state)
{
	const auto & b = skeleton.bones.find(currentBoneID)->second;	
	auto changeIT = changes.find(b.name);
	if (changeIT != changes.end()) {
		state.push_back(changeIT->second);
	}
	
	auto lIT = skeleton.hierarchy.left.lower_bound(currentBoneID);
	auto uIT = skeleton.hierarchy.left.upper_bound(currentBoneID);

	for (; lIT != uIT; ++lIT)
	{
		orderChanges(skeleton, lIT->get_right(), changes, state);
	}
}

SkeletonState::RigidPartialStateLocal SkeletonState::convert(
	const acclaim::Skeleton::Bones & bones,
	const acclaim::Skeleton::Mapping & aMapping,
	const acclaim::MotionData::BonesData & motionData,
	const AcclaimActiveSkeletonMappingLocal & activeMapping,
	const acclaim::Skeleton::HelperMotionData & helperMotionData,
	const bool angleInRadians)
{
	RigidPartialStateLocal ret;	

	for (const auto & bd : motionData)
	{
		auto it = aMapping.right.find(bd.name);
		if (it == aMapping.right.end()){
			throw std::runtime_error("Acclaim motion data mismatch mapping - missing " + bd.name + " bone");
		}

		const auto aNodeID = it->get_left();
		auto hIT = helperMotionData.find(aNodeID);
		if (hIT == helperMotionData.end()){
			throw std::runtime_error("Acclaim motion data mismatch skeleton motion helper data - missing " + bd.name + " bone");
		}

		auto aIT = activeMapping.data().right.find(aNodeID);
		if (aIT == activeMapping.data().right.end()){
			throw std::runtime_error("Acclaim motion data mismatch active skeleton mapping - missing " + bd.name + " bone");
		}

		const auto jointState = ::convert(bones, bd, angleInRadians, hIT->second);
		ret.data().orientations.insert({ aIT->get_left(), jointState.orientation });

		if (aIT->get_left() == 0){
			ret.data().position = jointState.position;
		}
	}

	return ret;
}

SkeletonState::RigidPartialStateLocal SkeletonState::convert(const acclaim::MotionData::BonesData & motionData,
	const AcclaimNameActiveSkeletonMappingLocal & activeMapping,
	const ConvertHelper & convertHelper)
{
	RigidPartialStateLocal ret;
	bool wasRoot = false;
	
	for (const auto & bd : motionData)
	{
		auto nit = activeMapping.data().find(bd.name);
		auto cit = convertHelper.rotationConverters.find(bd.name);
		auto q = cit->second(bd.channelValues);
		ret.data().orientations.insert({ nit->second, q});

		if ((wasRoot == false) && (bd.name == "root")){			
			ret.data().position = convertHelper.rootPositionExtractor(bd.channelValues);
			wasRoot = true;
		}
	}	

	return ret;
}

acclaim::MotionData::BonesData SkeletonState::convert(const acclaim::Skeleton & skeleton,
	const SkeletonState::RigidCompleteStateLocal & skeletonState,
	const AcclaimActiveSkeletonMappingLocal & activeMapping,
	const acclaim::Skeleton::HelperMotionData & helperMotionData)
{
	if (skeletonState.data().orientations.empty() == true && skeleton.bones.empty() == false){
		throw std::runtime_error("Empty skeleton state");
	}

	acclaim::MotionData::BonesData ret;
	const bool angleInRadians = skeleton.units.isAngleInRadians();
	
	acclaim::MotionData::BoneData bd;
	bd.name = "root";
	{
		auto helperData = helperMotionData.begin()->second;

		//osg::Quat c = kinematicUtils::convert(resolveRadians(angleInRadians, skeleton.orientation), skeleton.axisOrder);
		//osg::Quat cinv = c.inverse();

		//auto orient = kinematicUtils::convert(c * skeletonState.root()->value().localOrientation() * cinv, skeleton.rotationOrder());
		auto rotationOrder = acclaim::Skeleton::rotationOrder(skeleton);
		auto orient = kinematicUtils::convert(helperData.c * skeletonState.data().orientations.front() * helperData.cInv, rotationOrder);
		if (angleInRadians == false){
			orient = kinematicUtils::toDegrees(orient);
		}

		orient = kinematicUtils::orderedAngles(orient, rotationOrder);

		auto position = skeletonState.data().position;

		for (auto dO : skeleton.dataOrder)
		{			
			switch (dO)
			{
			case kinematicUtils::ChannelType::TX:
				bd.channelValues.push_back(position.x());
				break;
			case kinematicUtils::ChannelType::TY:
				bd.channelValues.push_back(position.y());
				break;
			case kinematicUtils::ChannelType::TZ:
				bd.channelValues.push_back(position.z());
				break;
			case kinematicUtils::ChannelType::RX:
				bd.channelValues.push_back(orient.x());
				break;
			case kinematicUtils::ChannelType::RY:
				bd.channelValues.push_back(orient.y());
				break;
			case kinematicUtils::ChannelType::RZ:
				bd.channelValues.push_back(orient.z());
				break;

			default:
				throw std::runtime_error("Unrecognized data channel for AMC file");
				break;
			}
		}
	}

	ret.push_back(bd);

	for (unsigned int i = 1; i < skeletonState.data().orientations.size(); ++i)
	{
		auto it = activeMapping.data().left.find(i);
		if (it != activeMapping.data().left.end()){
			auto acclaimID = it->get_right();
			acclaim::MotionData::BoneData bd;
			const auto & bData = skeleton.bones.find(acclaimID)->second;
			bd.name = bData.name;
			const auto & hData = helperMotionData.find(acclaimID)->second;
			auto rotationOrder = bData.rotationOrder();
			auto orient = kinematicUtils::convert(hData.c * skeletonState.data().orientations[i] * hData.cInv, rotationOrder);

			if (angleInRadians == false){
				orient = kinematicUtils::toDegrees(orient);
			}

			orient = kinematicUtils::orderedAngles(orient, rotationOrder);

			for (auto dO : bData.dofs)
			{
				switch (dO.channel)
				{
				case kinematicUtils::ChannelType::RX:
					bd.channelValues.push_back(orient.x());
					break;
				case kinematicUtils::ChannelType::RY:
					bd.channelValues.push_back(orient.y());
					break;
				case kinematicUtils::ChannelType::RZ:
					bd.channelValues.push_back(orient.z());
					break;
				case acclaim::DegreeOfFreedom::L:
					//TODO - co zrobić z tym przesunięciem?
					break;

				default:
					throw std::runtime_error("Unrecognized data channel for AMC file");
					break;
				}
			}

			ret.push_back(bd);
		}
	}

	return ret;
}

SkeletonState::NonRigidJointState convert(biovision::JointConstPtr joint, const biovision::MotionData::ChannelData & frameData)
{
	if (joint->channels.size() != frameData.size()){
		throw std::runtime_error("Biovision motion data mismatch skeleton structure - channels size mismatch. Joint: " + joint->name + ". Motion data channels size: " + boost::lexical_cast<std::string>(frameData.size()) + ". Joint channels size: " + boost::lexical_cast<std::string>(joint->channels.size()));
	}

	SkeletonState::NonRigidJointState ret;
	ret.position = osg::Vec3(0, 0, 0);
	ret.orientation = osg::Quat(0, 0, 0, 1);

	osg::Vec3 rot(0, 0, 0);
	unsigned int roti = 0;
	unsigned int i = 0;	
	for (const auto & channel : joint->channels)
	{
		switch (channel)
		{
		case kinematicUtils::ChannelType::TX:
			ret.position.x() = frameData[i];
			break;

		case kinematicUtils::ChannelType::TY:
			ret.position.y() = frameData[i];
			break;

		case kinematicUtils::ChannelType::TZ:
			ret.position.z() = frameData[i];
			break;

		case kinematicUtils::ChannelType::RX:
			rot[roti++] = osg::DegreesToRadians(frameData[i]);
			break;

		case kinematicUtils::ChannelType::RY:
			rot[roti++] = osg::DegreesToRadians(frameData[i]);
			break;

		case kinematicUtils::ChannelType::RZ:
			rot[roti++] = osg::DegreesToRadians(frameData[i]);
			break;

		default:
			throw std::runtime_error("Unsupported biovision channel: " + boost::lexical_cast<std::string>(channel));
		}

		++i;
	}

	ret.orientation = kinematicUtils::convert(rot, joint->rotationOrder);

	return ret;
}

void orderChanges(biovision::JointConstPtr joint,
	const std::map<std::string, SkeletonState::NonRigidJointState> & changes,
	SkeletonState::RigidCompleteState & change)
{
	change.orientations.push_back(changes.find(joint->name)->second.orientation);

	for (auto j : joint->joints)
	{
		orderChanges(j, changes, change);
	}
}

SkeletonState::RigidCompleteState SkeletonState::convert(const biovision::Skeleton & skeleton,
	const biovision::MotionData::FrameJointData & motionData)
{
	RigidCompleteState ret;

	std::map<std::string, NonRigidJointState> changes;	

	unsigned int i = 0;	
	for (const auto & joint : skeleton.sourceOrderJoints)
	{
		changes.insert({ joint->name, ::convert(joint, motionData[i++]) });
	}

	orderChanges(skeleton.root, changes, ret);

	//root - pozucja, zawsze pierwszy w hierarchi
	ret.position = changes.begin()->second.position;

	return ret;
}

SkeletonState::RigidCompleteStateGlobal SkeletonState::globalRigidState(const Skeleton & skeleton)
{
	RigidCompleteStateGlobal ret;

	if (skeleton.root() != nullptr){
		ret.data().position = skeleton.root()->value().globalPosition();
		auto filter = [&ret](Skeleton::JointConstPtr joint)
				{
					ret.data().orientations.push_back(joint->value().globalOrientation());
				};
		LinearizedSkeleton::Visitor::visitNonLeaf(skeleton, filter);
	}

	return ret;
}

SkeletonState::RigidCompleteStateLocal SkeletonState::localRigidState(const Skeleton & skeleton)
{
	RigidCompleteStateLocal ret;

	if (skeleton.root() != nullptr){
		ret.data().position = skeleton.root()->value().localPosition();
		auto filter = [&ret](Skeleton::JointConstPtr joint)
				{
					ret.data().orientations.push_back(joint->value().localOrientation());
				};
		LinearizedSkeleton::Visitor::visitNonLeaf(skeleton, filter);
	}

	return ret;
}

SkeletonState::NonRigidCompleteStateGlobal SkeletonState::globalNonRigidState(const Skeleton & skeleton)
{
	NonRigidCompleteStateGlobal ret;

	if (skeleton.root() != nullptr){		
		auto filter = [&ret](Skeleton::JointConstPtr joint)
				{
					ret.data().push_back({ joint->value().globalPosition(), joint->value().globalOrientation() });
				};
		LinearizedSkeleton::Visitor::visitNonLeaf(skeleton, filter);
	}

	return ret;
}

SkeletonState::NonRigidCompleteStateLocal SkeletonState::localNonRigidState(const Skeleton & skeleton)
{
	NonRigidCompleteStateLocal ret;

	if (skeleton.root() != nullptr){
		auto filter = [&ret](Skeleton::JointConstPtr joint)
				{
					ret.data().push_back({ joint->value().localPosition(), joint->value().localOrientation() });
				};
		LinearizedSkeleton::Visitor::visitNonLeaf(skeleton, filter);
	}

	return ret;
}

void SkeletonState::applyLocalState(Skeleton & skeleton, const RigidCompleteState & state)
{
	kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
		[&state](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().setLocal(state.orientations[idx]);
	});
	
	skeleton.root()->value().setGlobal(state.position);
}

void SkeletonState::applyLocalState(Skeleton & skeleton, const RigidPartialState & state)
{
	auto it = state.orientations.begin();
	if (it != state.orientations.end()){
		kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
			[&state, &it](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
		{
			if (it != state.orientations.end()){
				if (it->first == idx){
					joint->value().setLocal(it->second);
					++it;
				}
			}
		});
	}

	skeleton.root()->value().setGlobal(state.position);
}

void SkeletonState::applyLocalState(Skeleton & skeleton, const NonRigidCompleteState & state)
{
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton,
		[&state](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().setLocal(state[idx].position, state[idx].orientation);
	});	
}

void SkeletonState::applyLocalState(Skeleton & skeleton, const NonRigidPartialState & state)
{
	if (state.empty() == true){
		return;
	}

	auto it = state.begin();
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton,
		[&state, &it](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		if (it != state.end()){
			if (it->first == idx){
				joint->value().setLocal(it->second.position, it->second.orientation);
				++it;
			}
		}
	});
}

void SkeletonState::applyGlobalState(Skeleton & skeleton, const RigidCompleteState & state)
{
	kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
		[&state](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().setGlobal(state.orientations[idx]);
	});

	skeleton.root()->value().setGlobal(state.position);
}

void SkeletonState::applyGlobalState(Skeleton & skeleton, const RigidPartialState & state)
{
	auto it = state.orientations.begin();
	if (it != state.orientations.end()){
		kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
			[&state, &it](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
		{
			if (it != state.orientations.end()){
				if (it->first == idx){
					joint->value().setGlobal(it->second);
					++it;
				}
			}
		});
	}

	skeleton.root()->value().setGlobal(state.position);
}

void SkeletonState::applyGlobalState(Skeleton & skeleton, const NonRigidCompleteState & state)
{
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton,
		[&state](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().setGlobal(state[idx].position, state[idx].orientation);
	});
}

void SkeletonState::applyGlobalState(Skeleton & skeleton, const NonRigidPartialState & state)
{
	if (state.empty() == true){
		return;
	}

	auto it = state.begin();
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(skeleton,
		[&state, &it](kinematic::Skeleton::JointPtr joint, kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		if (it != state.end()){
			if (it->first == idx){
				joint->value().setGlobal(it->second.position, it->second.orientation);
				++it;
			}
		}
	});
}

SkeletonState::RotationConvertersMap SkeletonState::prepareRotationConvertersMap(const acclaim::Skeleton & skeleton)
{
	RadiansResolver radiansResolver = &resolveRadians < 0 > ;

	if (skeleton.units.isAngleInRadians()){
		radiansResolver = &resolveRadians < 1 > ;
	}
	
	return createRotationConvertersMap(skeleton, radiansResolver);
}

SkeletonState::ConvertHelper SkeletonState::prepareConvertHelper(const acclaim::Skeleton & skeleton)
{
	auto indicies = positionIndicies(skeleton.bones.begin()->second);

	return{ prepareRotationConvertersMap(skeleton), [&indicies](const acclaim::MotionData::ChannelValues & channelValues)
	{
		return osg::Vec3d(channelValues[indicies[0]], channelValues[indicies[1]], channelValues[indicies[2]]);
	} };
}
