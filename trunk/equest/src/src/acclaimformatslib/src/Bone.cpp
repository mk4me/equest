#include <acclaimformatslib/Bone.h>
#include <kinematicUtils/RotationConverter.h>

using namespace acclaim;

const osg::Vec3d Bone::defaultAxis = osg::Vec3d(0, 0, 0);

DegreeOfFreedom Bone::defaultRotationDof(const kinematicUtils::Channel channel,
	const kinematicUtils::AngleUnitType angleUnit)
{
	DegreeOfFreedom ret(channel);
	switch (angleUnit)
	{
	case kinematicUtils::Deg:
		ret.minLimit = -180.0;
		ret.maxLimit = 180.0;
		break;

	case kinematicUtils::Rad:
		ret.minLimit = -osg::PI;
		ret.maxLimit = osg::PI;
		break;	
	}

	return ret;
}

Bone::Dofs Bone::defaultRotationDofs(const kinematicUtils::AngleUnitType angleUnit)
{
	Dofs ret;
	ret.reserve(3);

	ret.push_back(defaultRotationDof(kinematicUtils::ChannelType::RX, angleUnit));
	ret.push_back(defaultRotationDof(kinematicUtils::ChannelType::RY, angleUnit));
	ret.push_back(defaultRotationDof(kinematicUtils::ChannelType::RZ, angleUnit));

	return ret;
}

Bone::Dofs Bone::defaultRootDofs(const kinematicUtils::AngleUnitType angleUnit)
{
	Dofs ret;
	ret.reserve(6);

	ret.push_back(DegreeOfFreedom(kinematicUtils::ChannelType::TX));
	ret.push_back(DegreeOfFreedom(kinematicUtils::ChannelType::TY));
	ret.push_back(DegreeOfFreedom(kinematicUtils::ChannelType::TZ));
	ret.push_back(defaultRotationDof(kinematicUtils::ChannelType::RZ, angleUnit));
	ret.push_back(defaultRotationDof(kinematicUtils::ChannelType::RY, angleUnit));
	ret.push_back(defaultRotationDof(kinematicUtils::ChannelType::RX, angleUnit));

	return ret;
}

Bone::Bone() : id(-1), axis(defaultAxis), length(-1),
axisOrder(defaultAxisOrder), direction(0.0, 0.0, 0.0)
{

}

Bone::Bone(const Bone & Other) : id(Other.id), name(Other.name), 
direction(Other.direction), length(Other.length), axis(Other.axis),
axisOrder(Other.axisOrder), dofs(Other.dofs)
{

}

Bone::Bone(Bone && Other) : id(Other.id), name(std::move(Other.name)),
direction(Other.direction), length(Other.length), axis(Other.axis),
axisOrder(Other.axisOrder), dofs(std::move(Other.dofs))
{
	Other.id = -1;
	Other.direction = osg::Vec3(0, 0, 0);
	Other.length = -1;
	Other.axis = osg::Vec3(0, 0, 0);
}

Bone::~Bone()
{

}

const bool Bone::isActive() const
{
	return dofs.empty() == false;
}

const kinematicUtils::AxisOrder::Type Bone::rotationOrder() const
{
	std::string s;
	int count = dofs.size();
	for (int i = 0; i < count && s.size() < 3; ++i) {
		if (dofs[i].channel == kinematicUtils::ChannelType::RX) {
			s += "X";
		}
		else if (dofs[i].channel == kinematicUtils::ChannelType::RY) {
			s += "Y";
		}
		else if (dofs[i].channel == kinematicUtils::ChannelType::RZ) {
			s += "Z";
		}
	}
	return Axis::getAxisOrder(s);
}

Bone::HelperMotionData Bone::helperData(const Bone & bone, const bool angleInRadians)
{
	const osg::Quat c = kinematicUtils::convert(angleInRadians == true ? bone.axis : kinematicUtils::toRadians(bone.axis), bone.axisOrder);

	return{ c, c.inverse() };
}