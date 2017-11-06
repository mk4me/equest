/********************************************************************
	created:	2015/07/21
	created:	11:54
	filename: 	Logger.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_LOGLIB__Logger_H__
#define HEADER_GUARD_LOGLIB__Logger_H__

#include <vector>
#include <loglib/Export.h>
#include <loglib/ILog.h>
#include "utils/Filesystem.h"
#include <iosfwd>

namespace  loglib {
	
	//! Struktura przechowuje aktualny, globalny logger
	struct LOGLIB_EXPORT Logger
	{
	public:
		//! Ustawienie loga
		//! \param log Ustawiany globalny log, nie mo¿e byæ pusty
		static void setLog(ILogPtr log) {
			if (!log) {
				throw std::invalid_argument("Null log was passed");
			}
			_log = log;
		}

		//! \return Zwraca aktualny, globalny log
		static ILogPtr getLog()
		{
			return _log;
		}
	protected:
		//! globalny log 
		static ILogPtr _log;
	};

	//! Log wypisuje komunikaty na standardowe wyjœcie
	class LOGLIB_EXPORT CoutLogger : public ILog
	{
	public:
		CoutLogger() {}
		CoutLogger(const std::string& prefix);
		virtual ~CoutLogger() {}
		virtual void log(LogSeverity severity, const std::string& message);
		virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo);
		virtual ILogPtr subLog(const std::string & name) const;
	private:
		std::string prefix;
	};

	//! Pusty log, komunikaty nie trafiaj¹ nigdzie
	class LOGLIB_EXPORT NullLogger : public ILog
	{
	public:
		virtual void log(LogSeverity severity, const std::string& message);
		virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo);
		virtual ILogPtr subLog(const std::string & name) const;
	};

	//! Log zapisuje komunikaty do pliku
	class LOGLIB_EXPORT FileLogger : public ILog
	{
	public:
		FileLogger() {}
		FileLogger(const std::string& prefix);
		virtual ~FileLogger() {}
		virtual void log(LogSeverity severity, const std::string& message);
		virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo);
		virtual ILogPtr subLog(const std::string & name) const;

		static void setPath(const utils::Filesystem::Path& val) { path = val; }
	private:
		std::string prefix;
		// todo : otwieranie strumienia i zapis nie jest thread safe 
		static std::ofstream ofstream;
		static utils::Filesystem::Path path;
	};

	//! Z³o¿enie kilku logów
	class LOGLIB_EXPORT MultiLogger : public ILog
	{
	public:
		MultiLogger(std::vector<ILogPtr>&& loggers, const std::string& prefix = std::string());
		virtual ~MultiLogger() {}
		virtual void log(LogSeverity severity, const std::string& message);
		virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo);
		virtual ILogPtr subLog(const std::string & name) const;
	private:
		std::string prefix;
		std::vector<ILogPtr> loggers;
	};
}

//! Makro loguj¹ce pomocnicze informacje	
#define UTILS_LOG_DEBUG(msg)	   LOG_DEBUG(loglib::Logger::getLog(), msg)	
//! Makro loguj¹ce b³¹d					   
#define UTILS_LOG_ERROR(msg)	   LOG_ERROR(loglib::Logger::getLog(), msg)	
//! Makro loguj¹ce informacjê			   
#define UTILS_LOG_INFO(msg)		   LOG_INFO(loglib::Logger::getLog(), msg)	
//! Makro loguj¹ce ostrze¿enia			   
#define UTILS_LOG_WARNING(msg)	   LOG_WARNING(loglib::Logger::getLog(), msg)

#endif // HEADER_GUARD 
