/********************************************************************
	created:	2015/01/30
	created:	10:51
	filename: 	Viewer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TEST__Viewer_H__
#define HEADER_GUARD_TEST__Viewer_H__

#include "kinematiclib/SkeletonState.h"

namespace  kinematicTest {
	class Viewer {
		public:
			virtual ~Viewer() {}
			static void start();
			static std::vector<osg::Vec3> getPos(const kinematic::Skeleton &skeleton);
	};
}

#endif // HEADER_GUARD 
