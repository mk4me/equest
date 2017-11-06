#include "CorePCH.h"
#include "Log.h"
#include "Config.h"
#include <utils/Debug.h>
#include <corelib/Thread.h>
#include "ApplicationCommon.h"

#ifndef CORE_DISABLE_LOGGING
//------------------------------------------------------------------------------
#ifdef CORE_ENABLE_LOG4CXX

#include <log4cxx/logger.h>

std::string extendThreadMessage(const std::string & message)
{
	std::stringstream ss;

	auto tID = std::this_thread::get_id();

	if (tID == core::getApplication()->mainThreadID()) {
		ss << "Thread [ID: " << tID << ", Main] -> " << message;
	}
	else {
		ss << "Thread [ID: " << tID << ", owner: " << core::Thread::currentOwner() << ", destination: " << core::Thread::currentDestination() << "] -> " << message;
	}

	return ss.str();
}

class LogImpl : public loglib::ILog
{
public:

	LogImpl() : logger(log4cxx::Logger::getRootLogger()) {}

private:

	LogImpl(const std::string & name) : name(name), logger(log4cxx::Logger::getLogger(name)) {}

public:

	virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
	{
		auto localMessage = extendThreadMessage(message);
		log4cxx::spi::LocationInfo location(fileName.c_str(), funcName.c_str(), lineNo);
		switch(severity)
		{
		case Debug:
			logger->debug(localMessage, location);
			break;

		case Error:
			logger->error(localMessage, location);
			break;

		case Info:
			logger->info(localMessage, location);
			break;

		case Warning:
			logger->warn(localMessage, location);
			break;

		default:
			UTILS_ASSERT((false), "Nieznany poziom logowania");
			break;
		}
	}

	virtual void log(LogSeverity severity, const std::string& message)
	{
		auto localMessage = extendThreadMessage(message);
		switch(severity)
		{
		case Debug:
			logger->debug(localMessage);
			break;

		case Error:
			logger->error(localMessage);
			break;

		case Info:
			logger->info(localMessage);
			break;

		case Warning:
			logger->warn(localMessage);
			break;

		default:
			UTILS_ASSERT((false), "Nieznany poziom logowania");
			break;
		}		
	}

	virtual loglib::ILogPtr subLog(const std::string & name) const
	{
		return loglib::ILogPtr(new LogImpl(this->name.empty() == true ? name : this->name + "." + name));
	}

private:
	log4cxx::LoggerPtr logger;
	const std::string name;
};


#else // CORE_ENABLE_LOG4CXX

#include <osg/Notify>

class LogImpl : public loglib::ILog
{
public:

	LogImpl() : name("root"), prefix("[root]->") {}

private:

	LogImpl(const std::string & name) : name(name), prefix("[" + name + "]->") {}

public:

	virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
	{
		std::stringstream msg;
		msg << "[file: " << fileName << "][function: " << funcName << "][line: " <<lineNo << "]->" << message;
		log(severity, msg.str());
	}

	virtual void log(LogSeverity severity, const std::string& message)
	{
		auto localMessage = extendThreadMessage(message);
		switch(severity)
		{
		case Debug:
			OSG_DEBUG << prefix << localMessage << std::endl;
			break;

		case Error:
			OSG_FATAL << prefix << localMessage << std::endl;
			break;

		case Info:
			OSG_NOTICE << prefix << localMessage << std::endl;
			break;

		case Warning:
			OSG_WARN << prefix << localMessage << std::endl;
			break;

		default:
			UTILS_ASSERT((false), "Nieznany poziom logowania");
			break;
		}		
	}

	virtual core::LogPtr subLog(const std::string & name) const
	{
		return core::LogPtr(new LogImpl(this->name.empty() == true ? name : this->name + "." + name));
	}

private:
	const std::string name;
	const std::string prefix;
};

#endif // CORE_ENABLE_LOG4CXX

namespace core {

Log::Log() : logImpl(new LogImpl)
{

}

Log::~Log()
{

}

void Log::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
	logImpl->log(severity, message, funcName, fileName, lineNo);
}

void Log::log(LogSeverity severity, const std::string& message)
{
	logImpl->log(severity, message);
}

loglib::ILogPtr Log::subLog(const std::string & name) const
{
	return logImpl->subLog(name);
}

}

//------------------------------------------------------------------------------
#else  // CORE_DISABLE_LOGGING

namespace core {

Log::Log() {}

void Log::log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo)
{
		
}

void Log::log(LogSeverity severity, const std::string& message)
{
		
}

core::LogPtr Log::subLog(const std::string & name) const
{
	static const core::LogPtr ret = core::LogPtr(new Log);
	return ret;
}

}

#endif  // CORE_DISABLE_LOGGING
//------------------------------------------------------------------------------


