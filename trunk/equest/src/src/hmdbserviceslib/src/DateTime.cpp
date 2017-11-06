/**
@author Marek Daniluk
*/

#include <iostream>
#include <string>
#include <hmdbserviceslib/DateTime.h>
#include <utils/Debug.h>
#include <stdexcept>
#include <time.h>

namespace hmdbServices
{
	Date::Date(const Date & date) : year(date.year), month(date.month), day(date.day)
	{
	}

	Date::Date() : year(0), month(0), day(0)
	{
	}

	Date::Date(int year, const unsigned char month, const unsigned char day)
		: year(year), month(month), day(day)
	{
		UTILS_ASSERT((month > 0 && month < 13), "Błędny miesiąc");
		UTILS_ASSERT((day > 0 && day < 32), "Błędny dzień");
	}

	Date::~Date()
	{
	}

	void Date::setYear(int year)
	{
		this->year = year;
	}

	void Date::setMonth(const unsigned char month)
	{
		if (month > 12 || month < 1)
			throw std::runtime_error("Incorrect date format.");
		this->month = month;
	}

	void Date::setDay(const unsigned char day)
	{
		if (day > 31 || day < 1)
			throw std::runtime_error("Incorrect date format.");
		this->day = day;
	}

	int Date::getYear() const
	{
		return year;
	}

	unsigned char Date::getMonth() const
	{
		return month;
	}

	unsigned char Date::getDay() const
	{
		return day;
	}

	std::string Date::toString() const
	{
		std::stringstream sstream;

		sstream.fill('0');

		sstream.flags(std::ios::right);
		sstream << year << "-";
		sstream.width(2); sstream << (unsigned int)month;
		sstream << "-";
		sstream.width(2); sstream << (unsigned int)day;

		return sstream.str();
	}

	Date Date::today()
	{
		Date ret;

		time_t date_now;
		tm* t;

		time(&date_now);
		t = localtime(&date_now);
		ret.year = t->tm_year + 1900;
		ret.month = t->tm_mon + 1;
		ret.day = t->tm_mday;

		return ret;
	}

	Date & Date::operator=(const Date & date)
	{
		year = date.year;
		month = date.month;
		day = date.day;
		return *this;
	}

	std::ostream& Date::operator <<(std::ostream& out) const
	{
		return out << toString();
	}

	//! Operator porównania
	//! \param date Data którą porównujemy
	bool Date::operator==(const Date & dater) const
	{
		bool ret = false;

		if (year == dater.year && month == dater.month && day == dater.day){
			ret = true;
		}

		return ret;
	}

	bool Date::operator!=(const Date & dater) const
	{
		return !(*this == dater);
	}

	bool Date::operator<(const Date & dater) const
	{
		bool ret = false;

		if (year > dater.year){
			//ret = false;
		}
		else if (year < dater.year){
			ret = true;
		}
		else if (month > dater.month){
			ret = false;
		}
		else if (month < dater.month){
			ret = true;
		}
		else if (day < dater.day){
			ret = true;
		}

		return ret;
	}

	bool Date::operator>(const Date & dater) const
	{
		return !(*this < dater) && !(*this == dater);
	}

	bool Date::operator<=(const Date & dater) const
	{
		return !(*this > dater);
	}

	bool Date::operator>=(const Date & dater) const
	{
		return !(*this < dater);
	}

	Time::Time(const Time & time) : hour(time.hour), minutes(time.minutes), seconds(time.seconds)
	{
	}

	Time::Time(const unsigned char hour,
		const unsigned char minutes,
		const unsigned char seconds) : hour(hour), minutes(minutes), seconds(seconds)
	{
		UTILS_ASSERT((hour > -1), "Błędna godzina");
		UTILS_ASSERT((minutes > -1 && minutes < 60), "Błędne minuty");
		UTILS_ASSERT((seconds > -1 && seconds < 60), "Błędne sekundy");
	}

	Time::~Time()
	{
	}

	Time Time::now()
	{
		Time ret;

		time_t date_now;
		tm* t;

		time(&date_now);
		t = localtime(&date_now);
		ret.hour = t->tm_hour;
		ret.minutes = t->tm_min;
		ret.seconds = t->tm_sec;

		return ret;
	}

	void Time::setHour(const unsigned char hour)
	{
		if (hour > 23 || hour < 0)
			throw std::runtime_error("Incorrect date format.");
		this->hour = hour;
	}

	void Time::setMinutes(const unsigned char minutes)
	{
		if (minutes > 59 || minutes < 0)
			throw std::runtime_error("Incorrect date format.");
		this->minutes = minutes;
	}

	void Time::setSeconds(const unsigned char seconds)
	{
		if (seconds > 59 || seconds < 0)
			throw std::runtime_error("Incorrect date format.");
		this->seconds = seconds;
	}

	unsigned char Time::getHour() const
	{
		return hour;
	}

	unsigned char Time::getMinutes() const
	{
		return minutes;
	}

	unsigned char Time::getSeconds() const
	{
		return seconds;
	}

	std::string Time::toString() const
	{
		std::stringstream sstream;

		sstream.fill('0');
		sstream.width(2);
		sstream.flags(std::ios::right);
		sstream << (unsigned int)hour << ":";
		sstream.width(2); sstream << (unsigned int)minutes;
		sstream << ":";
		sstream.width(2); sstream << (unsigned int)seconds;

		return sstream.str();
	}

	Time & Time::operator=(const Time & time)
	{
		hour = time.hour;
		minutes = time.minutes;
		seconds = time.seconds;
		return *this;
	}

	std::ostream& Time::operator <<(std::ostream& out) const
	{
		return out << toString();
	}

	bool Time::operator==(const Time & timer) const
	{
		bool ret = false;

		if (hour == timer.hour && minutes == timer.minutes && seconds == timer.seconds){
			ret = true;
		}

		return ret;
	}

	bool Time::operator!=(const Time & timer) const
	{
		return !(*this == timer);
	}

	bool Time::operator>(const Time & timer) const
	{
		bool ret = false;

		if (hour > timer.hour){
			ret = true;
		}
		else if (hour < timer.hour){
			ret = false;
		}
		else if (minutes > timer.minutes){
			ret = true;
		}
		else if (minutes < timer.minutes){
			ret = false;
		}
		else if (seconds > timer.seconds){
			ret = true;
		}

		return ret;
	}

	bool Time::operator<(const Time & timer) const
	{
		return !(*this > timer) && !(*this == timer);
	}

	bool Time::operator<=(const Time & timer) const
	{
		return !(*this > timer);
	}

	bool Time::operator>=(const Time & timer) const
	{
		return !(*this < timer);
	}

	DateTime::DateTime()
	{
	}

	DateTime::DateTime(int year, const unsigned char month,
		const unsigned char day, const unsigned char hour,
		const unsigned char minutes, const unsigned char seconds) :
		Date(year, month, day), Time(hour, minutes, seconds)
	{
	}

	DateTime::DateTime(const DateTime & dateTime) :
		Date(dateTime), Time(dateTime)
	{
	}

	DateTime::~DateTime()
	{
	}

	DateTime DateTime::now()
	{
		time_t date_now;
		tm* t;

		time(&date_now);
		t = localtime(&date_now);

		return DateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	}

	std::string DateTime::toString() const
	{
		std::string ret(Date::toString());
		ret += " " + Time::toString();
		return ret;
	}

	DateTime & DateTime::operator=(const DateTime & dateTime)
	{
		(Date &)(*this) = dateTime;
		(Time &)(*this) = dateTime;
		return *this;
	}

	/**
	Operator wyjscia dla DateTime pod cout
	*/
	std::ostream& DateTime::operator <<(std::ostream& out) const
	{
		return out << toString();
	}

	bool DateTime::operator==(const DateTime & dateTimer) const
	{
		bool ret = false;

		if ((const Date &)(*this) == (const Date &)(dateTimer) && (const Time &)(*this) == (const Time &)(dateTimer)){
			ret = true;
		}

		return ret;
	}

	bool DateTime::operator!=(const DateTime & dateTimer) const
	{
		return !(*this == dateTimer);
	}

	bool DateTime::operator<(const DateTime & dateTimer) const
	{
		bool ret = false;

		if ((const Date &)(*this) > (const Date &)(dateTimer)){
			ret = false;
		}
		else if ((const Date &)(*this) < (const Date &)(dateTimer)){
			ret = true;
		}
		else if ((const Time &)(*this) < (const Time &)(dateTimer)) {
			ret = true;
		}

		return ret;
	}

	bool DateTime::operator>(const DateTime & dateTimer) const
	{
		return !(*this < dateTimer) && (*this != dateTimer);
	}

	bool DateTime::operator<=(const DateTime & dateTimer) const
	{
		return !(*this > dateTimer);
	}

	bool DateTime::operator>=(const DateTime & dateTimer) const
	{
		return !(*this < dateTimer);
	}
}