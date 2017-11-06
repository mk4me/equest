/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:49
    filename: IDFLogger.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDFLOGGER_H__
#define HEADER_GUARD___IDFLOGGER_H__

namespace df {

	//! Forward declarations
	class ISourceNode;
	class ISinkNode;
	class IProcessingNode;
	class IInputPin;
	class IOutputPin;
	class IModelReader;

	//! Interfejs loguj¹cy przebieg przetwarzania danych
	class IDFLogger
	{
	public:
		//! Poziom istotnoœci komunikatowó
		enum LogSeverity
		{
			DFLOG_INFO,		//! Wiadomoœæ
			DFLOG_DEBUG,	//! Diagnostyczne
			DFLOG_ERROR,	//! B³¹d
			DFLOG_WARNING	//! Ostrze¿enie
		};

	public:
		//! Loguje info Ÿród³a danych
		//! \param node Wêze³ Ÿród³owy
		//! \param message Komunikat
		//! \param severity Poziom istotnoœci
		virtual void logMessage(ISourceNode * source, const char * message, LogSeverity severity) = 0;

		//! Loguje info odbiornika danych
		//! \param node Wêze³ odbiornika
		//! \param message Komunikat
		//! \param severity Poziom istotnoœci
		virtual void logMessage(ISinkNode * sink, const char * message, LogSeverity severity) = 0;

		//! Loguje info procesora danych
		//! \param node Procesor danych
		//! \param message Komunikat
		//! \param severity Poziom istotnoœci
		virtual void logMessage(IProcessingNode * processor, const char * message, LogSeverity severity) = 0;

		//! Loguje info modelu
		//! \param model Model danych
		//! \param message Komunikat
		//! \param severity Poziom istotnoœci
		virtual void logMessage(IModelReader * model, const char * message, LogSeverity severity) = 0;

		//! Loguje info pinu wejsciowego
		//! \param pin Pin wejœciowy
		//! \param message Komunikat
		//! \param severity Poziom istotnoœci
		virtual void logMessage(IInputPin * inputPin, const char * message, LogSeverity severity) = 0;

		//! Loguje info pinu wyjsciowego
		//! \param pin Pin wyjœciowy
		//! \param message Komunikat
		//! \param severity Poziom istotnoœci
		virtual void logMessage(IOutputPin * inputPin, const char * message, LogSeverity severity) = 0;

	};

	//! Interfejs pomocniczy przy logowaniu
	class IDFLoggerHelper
	{
	public:
		//! Logowanie wiadomoœci
		//! \param message Wiadomoœæ
		//! severity Poziom istotnoœci
		virtual void logMessage(const char * message, IDFLogger::LogSeverity severity) = 0;
	};
}

#endif	//	HEADER_GUARD___IDFLOGGER_H__
