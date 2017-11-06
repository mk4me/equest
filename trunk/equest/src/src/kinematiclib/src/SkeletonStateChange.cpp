#include <kinematiclib/SkeletonStateChange.h>
#include <utils//Debug.h>

using namespace kinematic;

SkeletonStateChange::RigidCompleteStateChange SkeletonStateChange::difference(
	const SkeletonState::RigidCompleteState & src,
	const SkeletonState::RigidCompleteState & dest)
{
	UTILS_ASSERT(src.orientations.size() == dest.orientations.size(), "States size mismatch");

	RigidCompleteStateChange ret;
	ret.translation = dest.position - src.position;	

	for (unsigned int i = 0; i < dest.orientations.size(); ++i)
	{
		ret.rotations.push_back(dest.orientations[i] / src.orientations[i]);
	}

	return ret;
}

SkeletonStateChange::RigidPartialStateChange SkeletonStateChange::difference(
	const SkeletonState::RigidCompleteState & src,
	const SkeletonState::RigidPartialState & dest)
{
	RigidPartialStateChange ret;
	ret.translation = dest.position - src.position;

	for (const auto & o : dest.orientations)
	{
		ret.rotations.insert(ret.rotations.end(), { o.first, o.second / src.orientations[o.first] });
	}

	return ret;
}

SkeletonStateChange::RigidPartialStateChange SkeletonStateChange::difference(
	const SkeletonState::RigidPartialState & src,
	const SkeletonState::RigidPartialState & dest)
{
	RigidPartialStateChange ret;
	ret.translation = dest.position - src.position;	

	for (const auto & o : dest.orientations)
	{
		auto oIT = src.orientations.find(o.first);
		if (oIT != src.orientations.end()){
			ret.rotations.insert(ret.rotations.end(), { o.first, o.second / oIT->second });
		}
	}

	return ret;
}

SkeletonStateChange::RigidPartialStateChange SkeletonStateChange::difference(
	const SkeletonState::RigidPartialState & src,
	const SkeletonState::RigidCompleteState & dest)
{
	RigidPartialStateChange ret;
	ret.translation = dest.position - src.position;	

	for (const auto & o : src.orientations)
	{
		ret.rotations.insert(ret.rotations.end(), { o.first, dest.orientations[o.first] / o.second });
	}

	return ret;
}

SkeletonStateChange::NonRigidCompleteStateChange SkeletonStateChange::difference(
	const SkeletonState::NonRigidCompleteState & src,
	const SkeletonState::NonRigidCompleteState & dest)
{
	UTILS_ASSERT(src.size() == dest.size(), "States size mismatch");

	NonRigidCompleteStateChange ret;	

	for (unsigned int i = 0; i < dest.size(); ++i)
	{
		ret.push_back({ dest[i].position - src[i].position , dest[i].orientation / src[i].orientation });
	}

	return ret;
}

SkeletonStateChange::NonRigidPartialStateChange SkeletonStateChange::difference(
	const SkeletonState::NonRigidCompleteState & src,
	const SkeletonState::NonRigidPartialState & dest)
{
	NonRigidPartialStateChange ret;	

	for (const auto & d : dest)
	{
		NonRigidJointStateChange sc = { d.second.position - src[d.first].position, d.second.orientation / src[d.first].orientation };
		ret.insert(ret.end(), std::make_pair(d.first, sc));
	}

	return ret;
}

SkeletonStateChange::NonRigidPartialStateChange SkeletonStateChange::difference(
	const SkeletonState::NonRigidPartialState & src,
	const SkeletonState::NonRigidPartialState & dest)
{
	NonRigidPartialStateChange ret;	

	for (const auto & d : dest)
	{
		auto oIT = src.find(d.first);
		if (oIT != src.end()){
			NonRigidJointStateChange sc = { d.second.position - oIT->second.position, d.second.orientation / oIT->second.orientation };
			ret.insert(ret.end(), std::make_pair(d.first, sc));
		}
	}

	return ret;
}

SkeletonStateChange::NonRigidPartialStateChange SkeletonStateChange::difference(
	const SkeletonState::NonRigidPartialState & src,
	const SkeletonState::NonRigidCompleteState & dest)
{
	NonRigidPartialStateChange ret;	

	for (const auto & d : src)
	{
		NonRigidJointStateChange sc = { dest[d.first].position - d.second.position, dest[d.first].orientation / d.second.orientation };
		ret.insert(ret.end(), std::make_pair(d.first, sc));
	}

	return ret;
}

void SkeletonStateChange::applyLocalChange(Skeleton & skeleton,
	const RigidCompleteStateChange & stateChange)
{
	skeleton.root()->value().localUpdate(stateChange.translation);	 
	auto v = [&stateChange](Skeleton::JointPtr joint, const LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().localUpdate(stateChange.rotations[idx]);
	};

	LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton, v);
}

void SkeletonStateChange::applyLocalChange(Skeleton & skeleton,
	const RigidPartialStateChange & stateChange)
{
	skeleton.root()->value().localUpdate(stateChange.translation);
	auto v = [&stateChange](Skeleton::JointPtr joint, const LinearizedSkeleton::NodeIDX idx)
	{
		auto it = stateChange.rotations.find(idx);
		if (it != stateChange.rotations.end()){
			joint->value().localUpdate(it->second);
		}
	};

	LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton, v);
}

void SkeletonStateChange::applyLocalChange(Skeleton & skeleton,
	const NonRigidCompleteStateChange & stateChange)
{		
	auto v = [&stateChange](Skeleton::JointPtr joint, const LinearizedSkeleton::NodeIDX idx)
	{		
		const auto & d = stateChange[idx];
		joint->value().localUpdate(d.translation, d.rotation);
	};

	LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton, v);
}

void SkeletonStateChange::applyLocalChange(Skeleton & skeleton,
	const NonRigidPartialStateChange & stateChange)
{	
	auto v = [&stateChange](Skeleton::JointPtr joint, const LinearizedSkeleton::NodeIDX idx)
	{
		auto it = stateChange.find(idx);
		if (it != stateChange.end()){			
			joint->value().localUpdate(it->second.translation, it->second.rotation);
		}		
	};

	LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton, v);
}

void SkeletonStateChange::applyGlobalChange(Skeleton & skeleton,
	const RigidCompleteStateChange & stateChange)
{
	skeleton.root()->value().globalUpdate(stateChange.translation);	
	auto v = [&stateChange](Skeleton::JointPtr joint, const LinearizedSkeleton::NodeIDX idx)
	{
		joint->value().globalUpdate(stateChange.rotations[idx]);
	};

	LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton, v);
}

void SkeletonStateChange::applyGlobalChange(Skeleton & skeleton,
	const RigidPartialStateChange & stateChange)
{
	skeleton.root()->value().globalUpdate(stateChange.translation);	 
	auto v = [&stateChange](Skeleton::JointPtr joint, const LinearizedSkeleton::NodeIDX idx)
	{
		auto it = stateChange.rotations.find(idx);
		if (it != stateChange.rotations.end()){
			joint->value().globalUpdate(it->second);
		}
	};

	LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton, v);
}

void SkeletonStateChange::applyGlobalChange(Skeleton & skeleton,
	const NonRigidCompleteStateChange & stateChange)
{	
	auto v = [&stateChange](Skeleton::JointPtr joint, const LinearizedSkeleton::NodeIDX idx)
	{
		const auto & d = stateChange[idx];
		joint->value().globalUpdate(d.translation, d.rotation);
	};

	LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton, v);
}

void SkeletonStateChange::applyGlobalChange(Skeleton & skeleton,
	const NonRigidPartialStateChange & stateChange)
{	
	auto v = [&stateChange](Skeleton::JointPtr joint, const LinearizedSkeleton::NodeIDX idx)
	{
		auto it = stateChange.find(idx);
		if (it != stateChange.end()){
			joint->value().globalUpdate(it->second.translation, it->second.rotation);
		}
	};

	LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton, v);
}