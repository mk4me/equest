#include <dflib/LoggerHelpers.h>
#include <iostream>

namespace df {

SourceLoggerHelper::SourceLoggerHelper(ISourceNode * source, IDFLogger * logger) : source(source), logger(logger)
{

}

void SourceLoggerHelper::logMessage(const char * message, IDFLogger::LogSeverity severity)
{
	if(logger != nullptr){
		logger->logMessage(source, message, severity);
	}else{
		std::cout << "Empty logger: Source message severity -> " << severity << " | message -> " << message << std::endl;
	}
}

SinkLoggerHelper::SinkLoggerHelper(ISinkNode * sink, IDFLogger * logger) : sink(sink), logger(logger)
{

}

void SinkLoggerHelper::logMessage(const char * message, IDFLogger::LogSeverity severity)
{
	if(logger != nullptr){
		logger->logMessage(sink, message, severity);
	}else{
		std::cout << "Empty logger: Sink message severity -> " << severity << " | message -> " << message << std::endl;
	}
}

ProcessingLoggerHelper::ProcessingLoggerHelper(IProcessingNode * proc, IDFLogger * logger) : proc(proc), logger(logger)
{

}

void ProcessingLoggerHelper::logMessage(const char * message, IDFLogger::LogSeverity severity)
{
	if(logger != nullptr){
		logger->logMessage(proc, message, severity);
	}else{
		std::cout << "Empty logger: Processor message severity -> " << severity << " | message -> " << message << std::endl;
	}
}

}