/********************************************************************
created:    2017/10/30
filename: TimeUtils.h
author:	    Wojciech Kniec

purpose:
*********************************************************************/
#ifndef HEADER_GUARD__TIMEUTILS_H__
#define HEADER_GUARD__TIMEUTILS_H__


class QTime;
class QString; 

namespace utils
{
	QTime convertToQTime(double timeInSeconds);
	double convertToTime(const QTime & time);
	QString convertToTimeString(double timeInSeconds);
	QString convertToTimeString(const QTime& time);
}

#endif