/********************************************************************
	created:	2013/06/05
	created:	5:6:2013   10:57
	filename: 	MarkersFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__MARKERSFILTER_H__
#define HEADER_GUARD_HMM__MARKERSFILTER_H__

#include <plugins/c3d/C3DCollections.h>
#include "ConfiguredFilterCommand.h"


class MarkersFilter : public BuilderConfiguredFilterCommand
{
public:
    MarkersFilter();
	virtual ~MarkersFilter() {}

private:
};
DEFINE_SMART_POINTERS(MarkersFilter);

#endif
