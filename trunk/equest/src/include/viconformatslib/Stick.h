/********************************************************************
	created:  2014/12/12	09:09:05
	filename: Stick.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_VICONFORMATS__STICK_H__
#define __HEADER_GUARD_VICONFORMATS__STICK_H__

#include <string>
#include <osg/Vec4>

namespace vicon
{
	//! Struktura opisuje po³¹czenie miêdzy markerami
	struct Stick {
		std::string name1;  //<! nazwa pierwszego markera
		std::string name2;  //<! nazwa drugiego markera
		osg::Vec4 color;    //<! kolor po³¹czenia
	};
}

#endif	// __HEADER_GUARD_VICONFORMATS__STICK_H__