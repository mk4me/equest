#include <hanimlib/Object.h>

using namespace hAnim;

Object::Object(const Type type)
	: type(type)
{

}

Object::Object(const Object & Other)
	: type(Other.type)
{

}

Object::Object(Object && Other)
	: type(Other.type)
{

}

Object::~Object()
{

}

sequence<Object> Object::filter(const Type type, const sequence<Object> & src)
{
	sequence<Object> ret(src);

	ret.remove_if([type](sequence<Object>::value_type val)
	{
		return val->type != type;
	});

	return ret;
}