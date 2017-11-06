#include <hanimlib/Site.h>

using namespace hAnim;

Site::Site()
	: Object(Object::Site), center(0, 0, 0), rotation(0, 0, 0, 1),
scale(1, 1, 1), scaleOrientation(0, 0, 0, 1),
translation(0, 0, 0)
{

}

Site::Site(const Site & Other)
	: Object(Other), center(Other.center), children(Other.children),
	name(Other.name), rotation(Other.rotation), scale(Other.scale),
	scaleOrientation(Other.scaleOrientation), translation(Other.translation)
{

}

Site::Site(Site && Other)
	: Object(std::move(Other)), center(Other.center), children(std::move(Other.children)),
	name(std::move(Other.name)), rotation(Other.rotation), scale(Other.scale),
	scaleOrientation(Other.scaleOrientation), translation(Other.translation)
{

}

Site::~Site()
{

}