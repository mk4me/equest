/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   9:33
	filename: 	SkeletalSystem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SKELETALSYSTEM_H__
#define HEADER_GUARD_SUBJECT__SKELETALSYSTEM_H__

#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/SKeletalData.h>
#include <plugins/subject/ISubjectSystem.h>

class SkeletalSystem : ISubjectSystem
{
private:
	kinematic::SkeletalModelConstPtr skeletalModel;
	kinematic::SkeletonMappingSchemeConstPtr mappingScheme;

public:
	kinematic::SkeletalModelConstPtr getSkeletalModel() const { return skeletalModel; }
	void setSkeletalModel(kinematic::SkeletalModelConstPtr val) { skeletalModel = val; }


public:
	virtual void setUp( MotionConstPtr motion ) 
	{
		JointAngleChannelPtr joints = motion->getJoints();
		if (joints) {
			joints->setSkeletal(getSkeletalModel(), joints->getSkeletalData());
		}
		
	}

};

#endif
