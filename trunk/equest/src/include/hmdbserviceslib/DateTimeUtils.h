/********************************************************************
	created:  2014/05/19	11:15:42
	filename: DateTimeUtils.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_HMDBSERVICES__DATETIMEUTILS_H__
#define __HEADER_GUARD_HMDBSERVICES__DATETIMEUTILS_H__

#include <hmdbserviceslib/Export.h>
#include <hmdbserviceslib/DateTime.h>
#include <time.h>

namespace hmdbServices
{
	//! \param time Czas w formie stringu który chcemy rozpakować do naszgo formatu czasu
	HMDBSERVICES_EXPORT DateTime toTime(const std::string & time);
	//! \param time Czas któy konwertujemy do ciągu znaków w formacie UTC
	HMDBSERVICES_EXPORT std::string toString(const struct tm & time);
	//! \param time Czas któy konwertujemy do ciągu znaków w formacie UTC
	HMDBSERVICES_EXPORT std::string toString(const DateTime & time);
}

#endif	// __HEADER_GUARD_HMDBSERVICES__DATETIMEUTILS_H__