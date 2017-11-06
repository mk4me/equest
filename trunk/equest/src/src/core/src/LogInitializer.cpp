#include "CorePCH.h"
#include "Config.h"
#include "LogInitializer.h"
#include <QtCore/QThread>
#include <osg/Notify>
#include <boost/algorithm/string.hpp>
#include <utils/Debug.h>
#include <coreui/CoreLogWidget.h>
#include "ApplicationCommon.h"

using namespace core;
using namespace coreUI;

#ifndef CORE_DISABLE_LOGGING
//------------------------------------------------------------------------------
#ifdef CORE_ENABLE_LOG4CXX

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/synchronized.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/helpers/transcoder.h>
#include <queue>

using namespace log4cxx;

void QtCustomMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString& msg)
{
	const static auto qtLogger = Logger::getLogger("qt");
    switch (type) {
        case QtDebugMsg:
			LOG4CXX_DEBUG(qtLogger, msg.toStdString());
            break;
        case QtWarningMsg:
			LOG4CXX_WARN(qtLogger, msg.toStdString());
            break;
        default:
            // pozostałe poziomy komunikatu są już tożsame errorowi
			LOG4CXX_ERROR(qtLogger, msg.toStdString());
            break;
    }
}

/** Strumień przekierowywujący logowania do naszego logera */
class OsgNotifyHandlerLog4cxx : public osg::NotifyHandler
{
public:	
	//! Destruktor wirtualny
	virtual ~OsgNotifyHandlerLog4cxx() {}

	  //! \param severity
	  //! \param message
	  virtual void notify(osg::NotifySeverity severity, const char *message)
	  {
		  const static auto osgLogger = Logger::getLogger("osg");
		  // z wiadomości usuwamy zbędne znaki (osg zawsze na koniec daje nową linię)
		  std::string msg(message);
		  boost::trim(msg);
		  switch (severity) {
		  case osg::FATAL:
			  LOG4CXX_ERROR(osgLogger, message);
			  break;
		  case osg::WARN:
			  LOG4CXX_WARN(osgLogger, message);
			  break;
		  case osg::ALWAYS:
		  case osg::NOTICE:
		  case osg::INFO:
			  LOG4CXX_INFO(osgLogger, message);
			  break;
		  case osg::DEBUG_INFO:
		  case osg::DEBUG_FP:
			  LOG4CXX_DEBUG(osgLogger, message);
			  break;
		  default:
			  UTILS_ASSERT(false, "Nieznany poziom osg. Wiadomość: %s", message);
			  break;
		  }
	  }
};

class LogWidgetAppender : public AppenderSkeleton
{
private:
    //! Konsola właściwa.
    CoreLogWidget* log;
    //! Kolejka wiadomości dla konsoli. Używana, gdy pojawiają się zdarzenia logowania,
    //! a konsoli jeszcze nie ma (inicjalizacja).
    std::queue<CoreLogWidgetEntryPtr> queuedEntries;

public:
    DECLARE_LOG4CXX_OBJECT(LogWidgetAppender)
    BEGIN_LOG4CXX_CAST_MAP()
        LOG4CXX_CAST_ENTRY(LogWidgetAppender)
        LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
    END_LOG4CXX_CAST_MAP()

public:
    //! Zerujący konstruktor
    LogWidgetAppender() :
    log(nullptr)
    {}
    //!
    virtual ~LogWidgetAppender()
    {
        UTILS_ASSERT(log == nullptr, "Wskaźnik powinien zostać jawnie wyzerowany metodą setLog!");
    }

public:
    //! \param log Widget loga.
    void setLog(CoreLogWidget* log)
    {
        // korzytamy z muteksa z klasy bazowej !
        helpers::synchronized sync(mutex);
        this->log = log;
        // opróżniamy kolejkę komunikatów
        if ( log ) {
            for ( ; !queuedEntries.empty(); queuedEntries.pop() ) {
                log->logOrQueueEntry(queuedEntries.front());
            }
        }
    }

// Appender
public:
    virtual void close()
	{
		setLog(nullptr);
		std::queue<CoreLogWidgetEntryPtr>().swap(queuedEntries);
	}

    virtual bool requiresLayout() const
    {
        return true;
    }

// AppenderSkeleton
protected:
    virtual void append(const spi::LoggingEventPtr& event, helpers::Pool& p)
    {
        LogString buf;
        layout->format(buf, event, p);

        CoreLogWidgetEntryPtr entry(new CoreLogWidgetEntry());
        int level = event->getLevel()->toInt();
        if ( level <= Level::DEBUG_INT ) {
            entry->severity = loglib::ILog::Debug;
        } else if ( level <= Level::INFO_INT ) {
            entry->severity = loglib::ILog::Info;
        } else if ( level <= Level::WARN_INT ) {
            entry->severity = loglib::ILog::Warning;
        } else {
            entry->severity = loglib::ILog::Error;
        }

        // rev - jakie flagi/ustawienia powoduja roznice? trzeba znalezc uniwersalne rozwiazanie
        // jeśli Utf16 jest ustawiane w konfiguracji, to nie można tego tak zahardcodować

		#if defined(_WINDOWS)
			#if LOG4CXX_LOGCHAR_IS_WCHAR
				entry->message = QString::fromUtf16( reinterpret_cast<const ushort*>(buf.c_str()) );
			#endif

			#if LOG4CXX_LOGCHAR_IS_UTF8
				entry->message = QString::fromUtf8( reinterpret_cast<const ushort*>(buf.c_str()) );
			#endif
			
			#if LOG4CXX_LOGCHAR_IS_UNICHAR
				entry->message = QString::fromStdString(buf);
			#endif
            
        #else
			
			#if LOG4CXX_LOGCHAR_IS_WCHAR
				#if WCHAR_MAX > 0xFFFFu
					entry->message = QString::fromUtf16( reinterpret_cast<const ushort*>(buf.c_str()) );
				#else
					entry->message = QString::fromUtf8( reinterpret_cast<const ushort*>(buf.c_str()) );
				#endif
			#endif

			#if LOG4CXX_LOGCHAR_IS_UTF8
				//entry->message = QString::fromUtf8( reinterpret_cast<const ushort*>(buf.c_str()) );
				entry->message = QString::fromUtf8( buf.c_str() );
			#endif

			#if LOG4CXX_LOGCHAR_IS_UNICHAR
				entry->message = QString::fromStdString(buf);
			#endif
        #endif

		entry->file = QString::fromLatin1(event->getLocationInformation().getFileName());
        entry->line = event->getLocationInformation().getLineNumber();
        entry->timestamp = QDate::currentDate();
        entry->theadId = QThread::currentThreadId();
		{
			// korzytamy z muteksa z klasy bazowej !
			helpers::synchronized sync(mutex);

			if ( log ) {
				// mamy konsolę - wysłamy jej komunikat
				log->logOrQueueEntry(entry);
			} else {
				// nie ma konsoli - kolejkujemy
				queuedEntries.push(entry);
			}
		}
    }
};

IMPLEMENT_LOG4CXX_OBJECT(LogWidgetAppender)
typedef log4cxx::helpers::ObjectPtrT<LogWidgetAppender> LogWidgetAppenderPtr;

//------------------------------------------------------------------------------

LogInitializer::LogInitializer( const utils::Filesystem::Path & configPath )
{
    LogWidgetAppender::registerClass();
    // załadowanie parametów logowania
    PropertyConfigurator::configure(configPath.string());
    osg::setNotifyHandler( new OsgNotifyHandlerLog4cxx());
    qInstallMessageHandler(QtCustomMessageHandler);
}

LogInitializer::~LogInitializer()
{
	// koniecznie trzeba przywrócić, inaczej będzie błąd
    qInstallMessageHandler(0);
	osg::setNotifyHandler( new osg::StandardNotifyHandler() );
	setLogWidget(nullptr);
}

void LogInitializer::setLogWidget( CoreLogWidget* widget )
{
    // pobranie wszystkich appenderów dla konsoli
    LoggerList loggers;
    Logger::getRootLogger()->getLoggerRepository()->getCurrentLoggers().swap(loggers);
    loggers.push_back( Logger::getRootLogger() );
    std::set<LogWidgetAppenderPtr> logAppenders;
    for(auto logger : loggers) {
        AppenderList appenders = logger->getAllAppenders();
        for(auto appender : appenders) {
            if ( LogWidgetAppenderPtr logAppender = appender ) {
                logAppenders.insert(logAppender);
            }
        }
    }
    // ustawienie konsoli
    for(auto logAppender : logAppenders) {
        logAppender->setLog(widget);
    }
}


#else // fallback do logowania OSG

#include <mutex>
#include <osg/ref_ptr>

void QtMessageHandler(QtMsgType type, const char *msg)
{
	switch (type) {
	case QtDebugMsg:
		OSG_DEBUG << "[qt]->" << msg << std::endl;
		break;
	case QtWarningMsg:
		OSG_WARN << "[qt]->" << msg << std::endl;
		break;
	default:
		// pozostałe poziomy komunikatu są już tożsame errorowi
		OSG_FATAL << "[qt]->" << msg << std::endl;
		break;
	}
}

/** Strumień przekierowywujący logowania do konsoli */
class OsgNotifyHandlerLogWidget : public osg::NotifyHandler
{
private:
    //! Konsola właściwa.
    CoreLogWidget* log;

	osg::ref_ptr<osg::NotifyHandler> defaultHandler;
    //! Kolejka wiadomości dla konsoli. Używana, gdy pojawiają się zdarzenia logowania,
    //! a konsoli jeszcze nie ma (inicjalizacja).
    std::queue<CoreLogWidgetEntryPtr> queuedEntries;
    typedef std::lock_guard<std::mutex> ScopedLock;
    std::mutex queueMutex;

public:
    //! \param log Log.
    OsgNotifyHandlerLogWidget(osg::NotifyHandler* handler) :
    log(nullptr), defaultHandler(handler)
    {}

	virtual ~OsgNotifyHandlerLogWidget() {}

	//! \return Domyślny handler.
	osg::NotifyHandler* getDefaultHandler()
	{
		return defaultHandler.get();
	}

    //! \param log
    void setLog(CoreLogWidget* log)
    {
		// nie ma konsoli - kolejkujemy
		ScopedLock lock(queueMutex);

        this->log = log;
        // opróżniamy kolejkę komunikatów
        if ( log ) {
            for ( ; !queuedEntries.empty(); queuedEntries.pop() ) {
                log->logOrQueueEntry(queuedEntries.front());
            }
        }
    }

    //! \param severity
    //! \param message
    virtual void notify(osg::NotifySeverity severity, const char *message)
    {
        // standardowe drukowanie
        defaultHandler->notify(severity, message);

        // z wiadomości usuwamy zbędne znaki (osg zawsze na koniec daje nową linię)
        std::string msg(message);
        boost::trim(msg);

        CoreLogWidgetEntryPtr entry(new CoreLogWidgetEntry());
        if ( severity >= osg::DEBUG_INFO ) {
            entry->severity = loglib::ILog::Debug;
        } else if ( severity >= osg::NOTICE ) {
            entry->severity = loglib::ILog::Info;
        } else if ( severity >= osg::WARN ) {
            entry->severity = loglib::ILog::Warning;
        } else {
            entry->severity = loglib::ILog::Error;
        }
        entry->message = QString::fromStdString(msg);
        entry->line = -1;
        entry->timestamp = QDate::currentDate();
        entry->theadId = QThread::currentThreadId();
		{
			ScopedLock lock(queueMutex);
			if ( log ) {
				// mamy konsolę - wysłamy jej komunikat
				log->logOrQueueEntry(entry);
			} else {
				queuedEntries.push(entry);
			}
		}
    }
};


LogInitializer::LogInitializer( const utils::Filesystem::Path & configPath )
{
    qInstallMsgHandler(QtMessageHandler);
    osg::setNotifyHandler( new OsgNotifyHandlerLogWidget(osg::getNotifyHandler()) );
}

LogInitializer::~LogInitializer()
{
	qInstallMsgHandler(0);
    OsgNotifyHandlerLogWidget* handler = dynamic_cast<OsgNotifyHandlerLogWidget*>(osg::getNotifyHandler());
    UTILS_ASSERT(handler);
    osg::setNotifyHandler( handler->getDefaultHandler() );
}


void LogInitializer::setLogWidget( CoreLogWidget* widget )
{
    OsgNotifyHandlerLogWidget* handler = dynamic_cast<OsgNotifyHandlerLogWidget*>(osg::getNotifyHandler());
    UTILS_ASSERT(handler);
    handler->setLog(widget);
}

#endif

#else //CORE_DEFINE_DISABLE_LOGGING

void QtMessageHandler(QtMsgType type, const char *msg)
{
}

/** Strumień przekierowywujący logowania do konsoli */
class OsgNotifyHandlerLogWidget : public osg::NotifyHandler
{
public:

	OsgNotifyHandlerLogWidget()
	{

	}

	  //! \param severity
	  //! \param message
	  virtual void notify(osg::NotifySeverity severity, const char *message)
	  {
		  
	  }
};


LogInitializer::LogInitializer( const utils::Filesystem::Path & configPath )
{
	qInstallMsgHandler(QtMessageHandler);
	osg::setNotifyHandler( new OsgNotifyHandlerLogWidget() );
}

LogInitializer::~LogInitializer()
{
	qInstallMsgHandler(0);
	osg::setNotifyHandler( new osg::StandardNotifyHandler() );
}


void LogInitializer::setLogWidget( CoreLogWidget* widget )
{
	
}

#endif

