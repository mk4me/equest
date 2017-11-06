/********************************************************************
	created:  2014/12/16	19:57:52
	filename: Humanoid.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HANIM__HUMANOID_H__
#define __HEADER_GUARD_HANIM__HUMANOID_H__

#include <hanimlib/Object.h>
#include <hanimlib/Joint.h>
#include <hanimlib/Segment.h>
#include <hanimlib/Site.h>
#include <hanimlib/Displacer.h>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

namespace hAnim
{
	/// \brief  Klasa przechowuje hierarchie kości
	struct Humanoid
	{
		// Mapowianie rodzic -> dziecko - kilka dzieci może mieć tego samego rodzica
		typedef boost::bimap<boost::bimaps::multiset_of<std::string>, boost::bimaps::set_of<std::string>> Hierarchy;
		//! Konstruktor domyślny
		Humanoid();
		//! \param Other Przejmowany humanoid
		Humanoid(const Humanoid & Other);
		//! \param Other Przejmowany humanoid
		Humanoid(Humanoid && Other);
		//! Desturktor
		~Humanoid() {}

		//! \return Domyślny humanoid H-Anim
		static const Humanoid & defaultHumanHumanoid();
		//! \return Domyslna hierarchia H-Anim
		static const Hierarchy & defaultHumanHierarchy();

		//! \param
		osg::Vec3 bboxCenter;
		//! \param
		osg::Vec3 bboxSize;
		//! \param
		osg::Vec3 center;
		//! \param
		std::list<std::string> info;
		//! \param
		sequence<Joint> joints;
		//! \param
		osg::Quat rotation;
		//! \param
		osg::Vec3 scale;
		//! \param
		osg::Quat scaleOrientation;
		//! \param
		sequence<Segment> segments;
		//! \param
		sequence<Site> sites;
		//! \param
		sequence<Object> skeleton;
		//! \param
		sequence<Object> skin;
		//! \param
		std::vector<osg::Vec3> skinCoord;
		//! \param
		std::vector<osg::Vec3> skinNormal;
		//! \param
		osg::Vec3 translation;
		//! \param
		std::string version;
		//! \param
		sequence<Site> viewpoints;
	};
}

#endif	// __HEADER_GUARD_HANIM__HUMANOID_H__
