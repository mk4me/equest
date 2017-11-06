#include <hanimlib/Segment.h>

using namespace hAnim;

Segment::Segment()
	: Object(Object::Segment), bboxCenter(0, 0, 0), bboxSize(-1, -1, -1),
centerOfMass(0, 0, 0), mass(-1)
{
	momentsOfInertia.fill(0);
}

Segment::Segment(const Segment & Other)
	: Object(Other), bboxCenter(Other.bboxCenter),
	centerOfMass(Other.centerOfMass), children(Other.children),
	coord(Other.coord), displacers(Other.displacers), mass(Other.mass),
	momentsOfInertia(Other.momentsOfInertia), name(Other.name)
{

}

Segment::Segment(Segment && Other)
	: Object(std::move(Other)), bboxCenter(Other.bboxCenter),
	centerOfMass(Other.centerOfMass), children(std::move(Other.children)),
	coord(std::move(Other.coord)), displacers(std::move(Other.displacers)),
	mass(Other.mass), momentsOfInertia(Other.momentsOfInertia),
	name(std::move(Other.name))
{

}

Segment::~Segment()
{

}