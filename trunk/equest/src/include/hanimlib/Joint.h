/********************************************************************
	created:  2014/12/05	14:04:28
	filename: Joint.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HANIMLIB__JOINT_H__
#define __HEADER_GUARD_HANIMLIB__JOINT_H__

#include <hanimlib/Object.h>
#include <hanimlib/Displacer.h>
#include <osg/Vec3>
#include <osg/Quat>
#include <array>
#include <vector>

namespace hAnim
{
	struct Joint : public Object
	{
		Joint();
		Joint(const Joint & Other);
		Joint(Joint && Other);

		virtual ~Joint();

		enum
		{
			X = 0,
			Y = 1,
			Z = 2
		};

		osg::Vec3 center;
		sequence<Object> children;
		sequence<hAnim::Displacer> displacers;
		std::array<osg::Vec3, 3> llimit;
		osg::Quat limitOrientation;
		std::string name;
		osg::Quat rotation;
		osg::Vec3 scale;
		osg::Quat scaleOrientation;
		std::vector<int> skinCoordIndex;
		std::vector<float> skinCoordWeight;
		std::vector<osg::Vec3> stiffness;
		osg::Vec3 translation;
		std::array<osg::Vec3, 3> ulimit;
	};	
}

#endif	// __HEADER_GUARD_HANIMLIB__JOINT_H__