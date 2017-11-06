#include <hmdbserviceslib/DateTimeUtils.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

namespace hmdbServices
{
	DateTime toTime(const std::string & time)
	{
		using tokenizer = boost::tokenizer<boost::char_separator<char> >;
		static boost::char_separator<char> sep("-T:.");

		DateTime ret;

		//musimy pociąć datę na kawałki
		tokenizer tokens(time, sep);
		if (tokens.begin() != tokens.end()){
			tokenizer::iterator it = tokens.begin();
			ret.setYear(boost::lexical_cast<int>(*it)); ++it;
			ret.setMonth(boost::lexical_cast<int>(*it)); ++it;
			ret.setDay(boost::lexical_cast<int>(*it)); ++it;
			if (it != tokens.end()){
				ret.setHour(boost::lexical_cast<int>(*it)); ++it;
				ret.setMinutes(boost::lexical_cast<int>(*it)); ++it;
				ret.setSeconds(boost::lexical_cast<int>(*it));
			}
			else{
				ret.setHour(0);
				ret.setMinutes(0);
				ret.setSeconds(0);
			}
		}

		return ret;
	}

	std::string toString(const struct tm & time)
	{
		char buffer[26];

		strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S.000Z", &time);
		buffer[25] = '\0';
		return std::string(buffer, sizeof(buffer));
	}

	std::string toString(const DateTime & time)
	{
		std::stringstream ss;

		boost::format fmt("%02d");
		std::string year = (fmt % (int)time.getYear()).str();
		std::string month = (fmt % (int)time.getMonth()).str();
		std::string day = (fmt % (int)time.getDay()).str();
		std::string hour = (fmt % (int)time.getHour()).str();
		std::string minutes = (fmt % (int)time.getMinutes()).str();
		std::string seconds = (fmt % (int)time.getSeconds()).str();

		ss << year << "-" << month << "-" << day << "T" << hour << ":" << minutes << ":" << seconds << ".000Z";

		return ss.str();
	}
}