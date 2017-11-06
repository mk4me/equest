/********************************************************************
	created:  2014/12/05	14:01:13
	filename: Site.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HANIMLIB__SITE_H__
#define __HEADER_GUARD_HANIMLIB__SITE_H__

#include <hanimlib/Object.h>
#include <osg/Vec3>
#include <osg/Quat>
#include <string>

namespace hAnim
{
	struct Site : public Object
	{
		Site();
		Site(const Site & Other);
		Site(Site && Other);

		virtual ~Site();

		osg::Vec3 center;
		sequence<Object> children;
		std::string name;
		osg::Quat rotation;
		osg::Vec3 scale;
		osg::Quat scaleOrientation;
		osg::Vec3 translation;
	};
}

#endif	// __HEADER_GUARD_HANIMLIB__SITE_H__