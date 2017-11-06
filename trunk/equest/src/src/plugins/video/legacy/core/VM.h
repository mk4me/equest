/********************************************************************
	created:	2010/02/11
	created:	11:2:2010   13:04
	filename: VM.h
	author:		Piotr Gwiazdowski

	purpose:	Jedyny nagłówek biblioteczny który kliencka aplikacja musi
            zaincludować aby mieć dostęp do podstawowej funkcjonalności.
*********************************************************************/
#ifndef __AVINOUT_H__
#define __AVINOUT_H__

////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <stdexcept>
#include <memory.h>
#include <map>

////////////////////////////////////////////////////////////////////////////////

#include <utils/Utils.h>
#include <utils/Profiler.h>
#include <utils/Debug.h>
#include <utils/Enum.h>

#include <plugins/video/core/ConfigVideo.h>
#include <plugins/video/core/Utils.h>
#include <plugins/video/core/PixelFormat.h>
#include <plugins/video/core/Picture.h>
#include <plugins/video/core/VideoError.h>
#include <plugins/video/core/VideoStream.h>

////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_H__
