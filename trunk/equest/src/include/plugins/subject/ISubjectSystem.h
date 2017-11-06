/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   10:53
	filename: 	ISubjectSystem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__ISUBJECTSYSTEM_H__
#define HEADER_GUARD_SUBJECT__ISUBJECTSYSTEM_H__

#include <plugins/subject/Motion.h>

class ISubjectSystem
{
public:
	virtual ~ISubjectSystem() {}

public:
	// todo zmienic na pure virtual;
	virtual void setUp(MotionConstPtr motion) {}
};
typedef utils::shared_ptr<ISubjectSystem> ISubjectSystemPtr;
typedef utils::shared_ptr<const ISubjectSystem> ISubjectSystemConstPtr;


#endif
