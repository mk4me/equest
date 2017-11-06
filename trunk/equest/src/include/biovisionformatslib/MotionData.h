/********************************************************************
	created:  2014/12/10	22:57:09
	filename: MotionData.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_BIOVISIONFORMATS__MOTIONDATA_H__
#define __HEADER_GUARD_BIOVISIONFORMATS__MOTIONDATA_H__

#include <vector>

namespace biovision
{
	struct MotionData
	{
		using ChannelData = std::vector<double>;
		using FrameJointData = std::vector<ChannelData>;
		using Frames = std::vector<FrameJointData>;

		//! Czas pojedynczej ramki
		float frameTime;
		//! Ramki danych
		Frames frames;
	};
}

#endif	// __HEADER_GUARD_BIOVISIONFORMATS__MOTIONDATA_H__