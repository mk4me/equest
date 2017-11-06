/********************************************************************
	created:  2015/10/01	15:11:01
	filename: CURLUtils.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__CURLUTILS_H__
#define __HEADER_GUARD_NETWORKUTILS__CURLUTILS_H__

#include <networkUtils/Export.h>
#include <curl/curl.h>
#include <utils/SmartPtr.h>

namespace networkUtils
{
	DEFINE_SMART_POINTERS(CURLM);
	DEFINE_SMART_POINTERS(CURL);

	NETWORKUTILS_EXPORT CURLPtr wrapCurlEasy(CURL * curl);

	NETWORKUTILS_EXPORT CURLMPtr wrapCurlMulti(CURLM * curl);

	NETWORKUTILS_EXPORT CURLPtr createCurlEasy();

	NETWORKUTILS_EXPORT CURLMPtr createCurlMulti();
}

#endif	// __HEADER_GUARD_NETWORKUTILS__CURLUTILS_H__