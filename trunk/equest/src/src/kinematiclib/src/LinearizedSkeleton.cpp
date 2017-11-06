#include <kinematiclib/LinearizedSkeleton.h>

using namespace kinematic;

struct NameKeyExtractor
{
	std::string operator()(const Skeleton::JointData & jointData) const
	{
		return jointData.name();
	}
};


LinearizedSkeleton::Order LinearizedSkeleton::createCompleteOrder(const Skeleton & skeleton)
{
	return Visitor::createCompleteOrderKey(skeleton, NameKeyExtractor());
}

LinearizedSkeleton::Order LinearizedSkeleton::createNonLeafOrder(const Skeleton & skeleton)
{
	return Visitor::createNonLeafOrderKey(skeleton, NameKeyExtractor());
}

LinearizedSkeleton::GlobalMapping LinearizedSkeleton::createCompleteMapping(const Skeleton & skeleton)
{
	return GlobalMapping(Visitor::createCompleteMappingKey(skeleton, NameKeyExtractor()));
}

LinearizedSkeleton::LocalMapping LinearizedSkeleton::createNonLeafMapping(const Skeleton & skeleton)
{
	auto order = Visitor::createNonLeafOrderKey(skeleton, NameKeyExtractor());
	
	return LocalMapping(treeContainer::Linearization::convert(order));
}