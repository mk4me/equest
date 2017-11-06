/********************************************************************
	created:  2016/08/26 12:25
	filename: SliceGenerator.h
	author:	  Wojciech Knieć
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN_NEWCHART__SLICEGENERATOR_H__
#define HEADER_GUARD_PLUGIN_NEWCHART__SLICEGENERATOR_H__

#include "plugins/newChart/INewChartCurveGenerator.h"

struct SliceGenerator : public GenericGenerator<SliceGenerator, double, double>
{
public:
	static c3dlib::ScalarChannelReaderInterfacePtr generator(const c3dlib::ScalarChannelReaderInterfaceConstPtr& sc, double x1, double x2);

};
DEFINE_SMART_POINTERS(SliceGenerator);


#endif