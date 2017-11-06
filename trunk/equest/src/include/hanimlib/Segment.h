/********************************************************************
	created:  2014/12/05	13:57:51
	filename: Segment.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HANIMLIB__SEGMENT_H__
#define __HEADER_GUARD_HANIMLIB__SEGMENT_H__

#include <hanimlib/Object.h>
#include <hanimlib/Displacer.h>
#include <osg/Vec3>
#include <array>
#include <string>

namespace hAnim {	

	/// \brief Klasa reprezentuje segment standardu h-anim
	/// Klasa nie jest powiązana z danymi, pomaga jedynie budować hierarchie
	struct Segment : public Object
	{
		Segment();
		Segment(const Segment & Other);
		Segment(Segment && Other);
		virtual  ~Segment();

		osg::Vec3 bboxCenter;
		osg::Vec3 bboxSize;
		osg::Vec3 centerOfMass;
		sequence<Object> children;
		std::list<osg::Vec3> coord;
		sequence<hAnim::Displacer> displacers;
		//! masa
		float mass;
		std::array<float,9> momentsOfInertia;
		std::string name;
	};	
}

#endif	// __HEADER_GUARD_HANIMLIB__SEGMENT_H__
