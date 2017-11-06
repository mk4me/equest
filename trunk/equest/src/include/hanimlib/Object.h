/********************************************************************
created:  2014/12/05	13:18:23
filename: Object.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HANIMLIB__OBJECT_H__
#define __HEADER_GUARD_HANIMLIB__OBJECT_H__

#include <utils/SmartPtr.h>
#include <list>
#include <type_traits>

namespace hAnim
{
	template<typename T>
	using sequence = std::list < utils::shared_ptr<T> > ;

	struct Object
	{
	public:
		enum Type
		{
			Displacer,
			Joint,
			Segment,
			Site
		};

	protected:
		Object(const Type type);
		Object(const Object & Other);
		Object(Object && Other);
	public:
		//! Destruktor wirtualny
		virtual ~Object();
		//! Typ Obiektu
		const Type type;

		static sequence<Object> filter(const Type type, const sequence<Object> & src);

		template<typename T>
		static sequence<T> filter(sequence<Object> & src, T * dummy = nullptr)
		{
			static_assert(std::is_base_of<Object, T>::value, "Must be H-Anim hierarchy type - inherit from Object");

			T t;
			sequence<T> ret;

			for (auto val : src)
			{
				if (val->type == t.type){
					ret.push_back(utils::dynamic_pointer_cast<T>(val));
				}
			}

			return ret;
		}
	};
}


#endif	// __HEADER_GUARD_HANIMLIB__OBJECT_H__