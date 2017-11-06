#include <loglib/Logger.h>
#include <iostream>
#include <fstream>

loglib::ILogPtr loglib::Logger::_log = utils::make_shared<loglib::CoutLogger>("utils");




std::string severity2Str(loglib::ILog::LogSeverity severity)
{
	switch (severity) {
		case (loglib::ILog::Error)	: return std::string("Error");
		case (loglib::ILog::Warning): return std::string("Warning");
		case (loglib::ILog::Info)	: return std::string("Info");
		case (loglib::ILog::Debug)	: return std::string("Debug");
		default:			 return std::string("Unknown");
	};
}

std::string makeLogMessage(loglib::ILog::LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
	std::stringstream ss;
	ss << "(" << severity2Str(severity) << ") " << fileName << ":" << funcName << "(" << lineNo << "): " << message;
	return ss.str();
}


loglib::CoutLogger::CoutLogger(const std::string& prefix) : 
	prefix(prefix)
{

}



void loglib::CoutLogger::log(LogSeverity severity, const std::string& message)
{
	std::string begining = prefix.empty() ? "" : prefix + ": ";
	switch (severity) {
	case ILog::Error:
		std::cerr << begining << message << std::endl;
		break;

	default:
		std::cout << begining << message << std::endl;
	}
}

void loglib::CoutLogger::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
	log(severity, makeLogMessage(severity, message, funcName, fileName, lineNo));
}

loglib::ILogPtr loglib::CoutLogger::subLog(const std::string & name) const
{
	return utils::make_shared<CoutLogger>(name);
}




loglib::FileLogger::FileLogger(const std::string& prefix) :
	prefix(prefix)
{

}

void loglib::FileLogger::log(LogSeverity severity, const std::string& message)
{
	ofstream.open(path.string(), std::fstream::app | std::fstream::out);
	ofstream << message << std::endl;
	ofstream.close();
}

void loglib::FileLogger::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
	log(severity, makeLogMessage(severity, message, funcName, fileName, lineNo));
}

loglib::ILogPtr loglib::FileLogger::subLog(const std::string & name) const
{
	return utils::make_shared<FileLogger>(name);
}

std::ofstream loglib::FileLogger::ofstream;
utils::Filesystem::Path loglib::FileLogger::path;

loglib::MultiLogger::MultiLogger(std::vector<ILogPtr>&& loggers, const std::string& prefix) :
	prefix(prefix),
	loggers(loggers)
{

}

void loglib::MultiLogger::log(LogSeverity severity, const std::string& message)
{
	for (auto& log : loggers) {
		log->log(severity, message);
	}
}

void loglib::MultiLogger::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
	log(severity, makeLogMessage(severity, message, funcName, fileName, lineNo));
}

loglib::ILogPtr loglib::MultiLogger::subLog(const std::string & name) const
{
	std::vector<ILogPtr> loggers;
	for (auto& log : loggers) {
		loggers.push_back(log->subLog(name));
	}
	return utils::make_shared<MultiLogger>(std::move(loggers), name);
}

void loglib::NullLogger::log(LogSeverity severity, const std::string& message)
{
}

loglib::ILogPtr loglib::NullLogger::subLog(const std::string & name) const
{
	return utils::make_shared<NullLogger>();
}

void loglib::NullLogger::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
}
