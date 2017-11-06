/********************************************************************
	created:  2014/12/10	22:43:15
	filename: Segment.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_BIOVISIONFORMATS__JOINT_H__
#define __HEADER_GUARD_BIOVISIONFORMATS__JOINT_H__

#include <string>
#include <vector>
#include <list>
#include <utils/SmartPtr.h>
#include <osg/Vec3>
#include <kinematicUtils/Types.h>

namespace biovision
{
	struct Joint;
	DEFINE_SMART_POINTERS(Joint);

	struct Joint
	{
		//! Rodzic
		JointWeakPtr parent;
		//! Nazwa
		std::string name;
		//! Lokalne przesuniêcie wzglêdem rodzica
		osg::Vec3 offset;
		//! Kana³y danych
		std::vector<kinematicUtils::Channel> channels;
		//! Kolejnoœæ rotacji
		kinematicUtils::AxisOrder::Type rotationOrder;
		//! Dzieci
		std::list<JointPtr> joints;
	};
}

#endif	// __HEADER_GUARD_BIOVISIONFORMATS__JOINT_H__