/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   11:10
	filename: 	MuscularSystem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__MUSCULARSYSTEM_H__
#define HEADER_GUARD_SUBJECT__MUSCULARSYSTEM_H__

class MuscularSystem
{
public:
	virtual ~MuscularSystem() {}
};
typedef utils::shared_ptr<MuscularSystem> MuscularSystemPtr;
typedef utils::shared_ptr<const MuscularSystem> MuscularSystemConstPtr;

#endif
