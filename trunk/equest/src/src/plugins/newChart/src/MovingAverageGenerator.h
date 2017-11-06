/********************************************************************
	created:  2016/08/11 11:21
	filename: MovingAverageGenerator.h
	author:	  Wojciech Knieć
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN_NEWCHART__MOVINGAVERAGEGENERATOR_H__
#define HEADER_GUARD_PLUGIN_NEWCHART__MOVINGAVERAGEGENERATOR_H__

#include "plugins/newChart/INewChartCurveGenerator.h"

class MovingAverageGenerator : public INewChartCurveGenerator
{
public:
	virtual ~MovingAverageGenerator();

	virtual std::string getName() const;

	virtual Params getDefaultParams() const;

	virtual std::pair<c3dlib::ScalarChannelReaderInterfacePtr, std::string> generate(const c3dlib::ScalarChannelReaderInterfaceConstPtr& sc, const Params&) const;

private:
	c3dlib::ScalarChannelReaderInterfacePtr createMovingAverage(c3dlib::ScalarChannelReaderInterfaceConstPtr origin, int sampleWindow) const;

};
DEFINE_SMART_POINTERS(MovingAverageGenerator);


#endif