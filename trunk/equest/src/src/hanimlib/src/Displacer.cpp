#include <hanimlib/Displacer.h>

using namespace hAnim;

Displacer::Displacer()
	: Object(Object::Displacer), weight(-1)
{

}

Displacer::Displacer(const Displacer & other)
	: Object(other), coordIndex(other.coordIndex),
	displacements(other.displacements), weight(other.weight)
{

}

Displacer::Displacer(Displacer && other) :
	Object(std::move(other)), coordIndex(std::move(coordIndex)),
	displacements(std::move(other.displacements)), weight(other.weight)
	
{

}

Displacer::~Displacer()
{

}