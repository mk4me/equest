/********************************************************************
	created:	2013/06/05
	created:	5:6:2013   20:39
	filename: 	KineticFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__KINETICFILTER_H__
#define HEADER_GUARD_HMM__KINETICFILTER_H__

#include <plugins/c3d/C3DCollections.h>
#include "ConfiguredFilterCommand.h"


class ForcesFilter : public BuilderConfiguredFilterCommand
{
public:
    ForcesFilter();
    virtual ~ForcesFilter() {}

};
DEFINE_SMART_POINTERS(ForcesFilter);

class MomentsFilter : public BuilderConfiguredFilterCommand
{
public:
    MomentsFilter();
    virtual ~MomentsFilter() {}

};
DEFINE_SMART_POINTERS(MomentsFilter);

class PowersFilter : public BuilderConfiguredFilterCommand
{
public:
    PowersFilter();
    virtual ~PowersFilter() {}

};
DEFINE_SMART_POINTERS(PowersFilter);


#endif
