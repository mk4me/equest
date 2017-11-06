/********************************************************************
	created:  2014/12/12	09:09:47
	filename: Marker.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_VICONFORMATS__MARKER_H__
#define __HEADER_GUARD_VICONFORMATS__MARKER_H__

#include <string>
#include <osg/Vec4>

namespace vicon
{
	//! Struktura opisuje pojedynczy marker
	struct Marker {
		std::string name;   //<! nazwa markera
		float radius;      //<! promieñ markera
		osg::Vec4 color;    //<! kolor markera
	};
}

#endif	// __HEADER_GUARD_VICONFORMATS__MARKER_H__