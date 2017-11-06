/********************************************************************
	created:	2011/06/21
	created:	21:6:2011   8:44
	filename: 	ILog.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__ILOG_H__
#define HEADER_GUARD_CORE__ILOG_H__

#include <string>
#include <utils/SmartPtr.h>
#include <utils/Utils.h>
#include <loglib/Export.h>

namespace loglib
{
	class ILog;
	DEFINE_SMART_POINTERS(ILog);
    //! Interfejs do logowania informacji w aplikacji (w konsoli aplikacji)
	//! \version 0.9.1
	class LOGLIB_EXPORT ILog
	{
	public:
		//! Poziom ważności wiadomości.
		enum LogSeverity {
			Error,   ///< Błąd
			Warning, ///< Ostrzeżenie
			Info,    ///< Informacja
			Debug    ///< Testowanie
		};

	public:

		virtual ~ILog() {}

        //! Metoda do logowania informacji
        //! \param severity Poziom istotności logowanej informacji
        //! \param message Treść logowanej informacji (wraz ze znakami formatującymi)
		virtual void log(LogSeverity severity, const std::string& message) = 0;
		//! Metoda do logowania informacji
		//! \param severity Poziom istotności logowanej informacji
		//! \param message Treść logowanej informacji (wraz ze znakami formatującymi)
		//! \param fileName Plik źródłowy z którego pochodzi wiadomość
		//! \param lineNo Numer lini w pliku źródłowym która wygenerowała wiadomość
		virtual void log(LogSeverity severity, const std::string& message, const std::string & funcName, const std::string & fileName, int lineNo) = 0;

		//! \param name Nazwa logera relatywna do nazwy aktualnego logera
		//! \return Nowy loger podpiety pod aktualny
		virtual ILogPtr subLog(const std::string & name) const = 0;
	};

} // namespace core

#define LOG__(logger, severity, msg) do { logger->log(severity, UTILS_FORMAT_STRING(msg), UTILS_PORTABLE_FUNCTION_NAME, __FILE__, __LINE__); } while (0)

#define LOG_DEBUG(logger, msg)				LOG__(logger, loglib::ILog::Debug, msg)
//! Makro logujące błąd
#define LOG_ERROR(logger, msg)				LOG__(logger, loglib::ILog::Error, msg)
//! Makro logujące informację
#define LOG_INFO(logger, msg)				LOG__(logger, loglib::ILog::Info, msg)
//! Makro logujące ostrzeżenia
#define LOG_WARNING(logger, msg)			LOG__(logger, loglib::ILog::Warning, msg)

#define SUB_LOG_DEBUG(logger, name, msg)	LOG_DEBUG(logger->subLog(name), msg)
//! Makro logujące błąd
#define SUB_LOG_ERROR(logger, name, msg)	LOG_ERROR(logger->subLog(name), msg)
//! Makro logujące informację
#define SUB_LOG_INFO(logger, name, msg)		LOG_INFO(logger->subLog(name), msg)
//! Makro logujące ostrzeżenia
#define SUB_LOG_WARNING(logger, name, msg)	LOG_WARNING(logger->subLog(name), msg)




#endif
