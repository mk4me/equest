/********************************************************************
	created:	2013/06/05
	created:	5:6:2013   20:36
	filename: 	EMGConfigFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EMGCONFIGFILTER_H__
#define HEADER_GUARD_HMM__EMGCONFIGFILTER_H__


#include <plugins/c3d/C3DCollections.h>
#include "ConfiguredFilterCommand.h"

class EMGConfigFilter : public BuilderConfiguredFilterCommand
{
public:
    EMGConfigFilter();
    virtual ~EMGConfigFilter() {}

};
DEFINE_SMART_POINTERS(EMGConfigFilter);

#endif
