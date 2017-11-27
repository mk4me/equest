#include <utils/TimeUtils.h>
#include <QtCore/QTime>

QTime utils::convertToQTime(double timeInSeconds)
{
	int seconds = 0;

	if (timeInSeconds >= 0) {
		seconds = floor(timeInSeconds);
	}
	else {
		seconds = ceil(timeInSeconds);
	}

	return QTime(0, 0, seconds, (timeInSeconds - seconds) * 1000.0);
}

double utils::convertToTime(const QTime & time)
{
	return (double)(time.hour() * 3600 + time.minute() * 60 + time.second()) + time.msec() / 1000.0;
}

QString utils::convertToTimeString(double timeInSeconds)
{
	return convertToTimeString(convertToQTime(timeInSeconds));
}

QString utils::convertToTimeString(const QTime& time)
{
	static const QString labelFormat("mm:ss:zzz");
	return time.toString(labelFormat);
}
