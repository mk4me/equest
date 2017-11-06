#include <hanimlib/Joint.h>

using namespace hAnim;

Joint::Joint()
	: Object(Object::Joint), center(0, 0, 0), limitOrientation(0, 0, 0, 1),
rotation(0, 0, 0, 1), scale(1, 1, 1), scaleOrientation(0, 0, 0, 1),
translation(0, 0, 0)
{
	llimit.fill(osg::Vec3(-osg::PI, -osg::PI, -osg::PI));
	ulimit.fill(osg::Vec3(osg::PI, osg::PI, osg::PI));
}

Joint::Joint(const Joint & Other)
	: Object(Other), center(Other.center), children(Other.children), displacers(Other.displacers),
	llimit(Other.llimit), limitOrientation(Other.limitOrientation), name(Other.name),
	rotation(Other.rotation), scale(Other.scale), scaleOrientation(Other.scaleOrientation),
	skinCoordIndex(Other.skinCoordIndex), skinCoordWeight(Other.skinCoordWeight),
	stiffness(Other.stiffness), translation(Other.translation),
	ulimit(Other.ulimit)
{

}

Joint::Joint(Joint && Other)
	: Object(std::move(Other)), center(Other.center), children(std::move(Other.children)),
		displacers(std::move(Other.displacers)),
	llimit(Other.llimit), limitOrientation(Other.limitOrientation), name(std::move(Other.name)),
	rotation(Other.rotation), scale(Other.scale), scaleOrientation(Other.scaleOrientation),
	skinCoordIndex(std::move(Other.skinCoordIndex)), skinCoordWeight(std::move(Other.skinCoordWeight)),
	stiffness(std::move(Other.stiffness)), translation(Other.translation),
	ulimit(Other.ulimit)
{

}

Joint::~Joint()
{

}